
#include "debug.h"
#include "str.h"
#include "stream.h"
#include <algorithm>
#include <vector>

#if defined EN_OS_WIN32 //&& NV_CC_MSVC
#   define WIN32_LEAN_AND_MEAN
#   define VC_EXTRALEAN
#   include <windows.h>
#   include <direct.h>
#if defined(_MSC_VER) 
#       include <crtdbg.h>
#       if _MSC_VER < 1300
#           define DECLSPEC_DEPRECATED
#           include <dbghelp.h> // must be XP version of file
#       else
#           include <dbghelp.h>
#       endif
#   endif
#   pragma comment(lib,"dbghelp.lib")
#endif


namespace base
{
	namespace
	{
		static bool s_interactive = true;
		static MessageHandler * s_message_handler = NULL;
		static AssertHandler * s_assert_handler = NULL;


		static CRITICAL_SECTION s_handler_critical_section;

		static HANDLE s_handler_start_semaphore = NULL;
		static HANDLE s_handler_finish_semaphore = NULL;

		// The exception handler thread.
		static HANDLE s_handler_thread = NULL;

		static DWORD s_requesting_thread_id = 0;
		static EXCEPTION_POINTERS * s_exception_info = NULL;

		static inline int backtraceWithSymbols(CONTEXT * ctx, void * trace[], int maxcount, int skip = 0) {

			// Init the stack frame for this function
			STACKFRAME64 stackFrame = { 0 };

#if EN_CPU_X86_64
			DWORD dwMachineType = IMAGE_FILE_MACHINE_AMD64;
			stackFrame.AddrPC.Offset = ctx->Rip;
			stackFrame.AddrFrame.Offset = ctx->Rbp;
			stackFrame.AddrStack.Offset = ctx->Rsp;
#elif EN_CPU_X86
			DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
			stackFrame.AddrPC.Offset = ctx->Eip;
			stackFrame.AddrFrame.Offset = ctx->Ebp;
			stackFrame.AddrStack.Offset = ctx->Esp;
#else
#error "Platform not supported!"
#endif
			stackFrame.AddrPC.Mode = AddrModeFlat;
			stackFrame.AddrFrame.Mode = AddrModeFlat;
			stackFrame.AddrStack.Mode = AddrModeFlat;

			// Walk up the stack
			const HANDLE hThread = GetCurrentThread();
			const HANDLE hProcess = GetCurrentProcess();
			int i;
			for (i = 0; i < maxcount; i++)
			{
				// walking once first makes us skip self
				if (!StackWalk64(dwMachineType, hProcess, hThread, &stackFrame, ctx, NULL, &SymFunctionTableAccess64, &SymGetModuleBase64, NULL)) {
					break;
				}

				/*if (stackFrame.AddrPC.Offset == stackFrame.AddrReturn.Offset || stackFrame.AddrPC.Offset == 0) {
				break;
				}*/

				if (i >= skip) {
					trace[i - skip] = (PVOID)stackFrame.AddrPC.Offset;
				}
			}

			return i - skip;
		}

#pragma warning(push)
#pragma warning(disable:4748)
		static  int backtrace(void * trace[], int maxcount) {
			CONTEXT ctx = { 0 };
#if EN_CPU_X86 && !EN_CPU_X86_64
			ctx.ContextFlags = CONTEXT_CONTROL;
			_asm {
				call x
				x : pop eax
					mov ctx.Eip, eax
					mov ctx.Ebp, ebp
					mov ctx.Esp, esp
			}
#else
			RtlCaptureContext(&ctx); // Not implemented correctly in x86.
#endif

			return backtraceWithSymbols(&ctx, trace, maxcount, 1);
		}
#pragma warning(pop)

		struct MinidumpCallbackContext {
			ULONG64 memory_base;
			ULONG memory_size;
			bool finished;
		};


		static BOOL CALLBACK miniDumpWriteDumpCallback(PVOID context, const PMINIDUMP_CALLBACK_INPUT callback_input, PMINIDUMP_CALLBACK_OUTPUT callback_output)
		{
			switch (callback_input->CallbackType)
			{
			case MemoryCallback: {
				MinidumpCallbackContext* callback_context = reinterpret_cast<MinidumpCallbackContext*>(context);
				if (callback_context->finished)
					return FALSE;

				// Include the specified memory region.
				callback_output->MemoryBase = callback_context->memory_base;
				callback_output->MemorySize = callback_context->memory_size;
				callback_context->finished = true;
				return TRUE;
			}

								 // Include all modules.
			case IncludeModuleCallback:
			case ModuleCallback:
				return TRUE;

				// Include all threads.
			case IncludeThreadCallback:
			case ThreadCallback:
				return TRUE;

				// Stop receiving cancel callbacks.
			case CancelCallback:
				callback_output->CheckCancel = FALSE;
				callback_output->Cancel = FALSE;
				return TRUE;
			}

			// Ignore other callback types.
			return FALSE;
		}


		static bool writeMiniDump(EXCEPTION_POINTERS * pExceptionInfo)
		{
			// create the file
			HANDLE hFile = CreateFileA("crash.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				//nvDebug("*** Failed to create dump file.\n");
				return false;
			}

			MINIDUMP_EXCEPTION_INFORMATION * pExInfo = NULL;
			MINIDUMP_CALLBACK_INFORMATION * pCallback = NULL;

			if (pExceptionInfo != NULL) {
				MINIDUMP_EXCEPTION_INFORMATION ExInfo;
				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = NULL;
				pExInfo = &ExInfo;

				MINIDUMP_CALLBACK_INFORMATION callback;
				MinidumpCallbackContext context;

				// Find a memory region of 256 bytes centered on the
				// faulting instruction pointer.
				const ULONG64 instruction_pointer =
#if defined(_M_IX86)
					pExceptionInfo->ContextRecord->Eip;
#elif defined(_M_AMD64)
					pExceptionInfo->ContextRecord->Rip;
#else
#error Unsupported platform
#endif

					MEMORY_BASIC_INFORMATION info;

				if (VirtualQuery(reinterpret_cast<LPCVOID>(instruction_pointer), &info, sizeof(MEMORY_BASIC_INFORMATION)) != 0 && info.State == MEM_COMMIT)
				{
					// Attempt to get 128 bytes before and after the instruction
					// pointer, but settle for whatever's available up to the
					// boundaries of the memory region.
					const ULONG64 kIPMemorySize = 256;
					context.memory_base = std::max(reinterpret_cast<ULONG64>(info.BaseAddress), instruction_pointer - (kIPMemorySize / 2));
					ULONG64 end_of_range = std::min(instruction_pointer + (kIPMemorySize / 2), reinterpret_cast<ULONG64>(info.BaseAddress) + info.RegionSize);
					context.memory_size = static_cast<ULONG>(end_of_range - context.memory_base);
					context.finished = false;

					callback.CallbackRoutine = miniDumpWriteDumpCallback;
					callback.CallbackParam = reinterpret_cast<void*>(&context);
					pCallback = &callback;
				}
			}

			MINIDUMP_TYPE miniDumpType = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData | MiniDumpWithThreadInfo);

			// write the dump
			BOOL ok = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, miniDumpType, pExInfo, NULL, pCallback) != 0;
			CloseHandle(hFile);

			if (ok == FALSE) {
				//nvDebug("*** Failed to save dump file.\n");
				return false;
			}

			//nvDebug("\nDump file saved.\n");

			return true;
		}

		static void writeStackTrace(void * trace[], int size, int start, std::vector<const char*> lines)
		{
			StringBuilder builder(512);

			HANDLE hProcess = GetCurrentProcess();

			// Resolve PC to function names
			for (int i = start; i < size; i++)
			{
				// Check for end of stack walk
				DWORD64 ip = (DWORD64)trace[i];
				if (ip == NULL)
					break;

				// Get function name
#define MAX_STRING_LEN  (512)
				unsigned char byBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_STRING_LEN] = { 0 };
				IMAGEHLP_SYMBOL64 * pSymbol = (IMAGEHLP_SYMBOL64*)byBuffer;
				pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
				pSymbol->MaxNameLength = MAX_STRING_LEN;

				DWORD64 dwDisplacement;

				if (SymGetSymFromAddr64(hProcess, ip, &dwDisplacement, pSymbol))
				{
					pSymbol->Name[MAX_STRING_LEN - 1] = 0;

					// pSymbol->Name
					const char * pFunc = pSymbol->Name;

					// Get file/line number
					IMAGEHLP_LINE64 theLine = { 0 };
					theLine.SizeOfStruct = sizeof(theLine);

					DWORD dwDisplacement;
					if (!SymGetLineFromAddr64(hProcess, ip, &dwDisplacement, &theLine))
					{
						// Do not print unknown symbols anymore.
						break;
						//builder.format("unknown(%08X) : %s\n", (uint32)ip, pFunc);
					}
					else
					{
						/*
						const char* pFile = strrchr(theLine.FileName, '\\');
						if ( pFile == NULL ) pFile = theLine.FileName;
						else pFile++;
						*/
						const char * pFile = theLine.FileName;

						int line = theLine.LineNumber;

						builder.format("%s(%d) : %s\n", pFile, line, pFunc);
					}

					lines.push_back(builder.release());

					if (pFunc != NULL && strcmp(pFunc, "WinMain") == 0) {
						break;
					}
				}
			}
		}


		static void dumpCallstackImpl(MessageHandler *messageHandler, int callstackLevelsToSkip, ...)
		{
#if defined EN_OS_WIN32
			if (true)
			{
				void * trace[64];
				int size = backtrace(trace, 64);

				std::vector<const char*> lines;
				writeStackTrace(trace, size, callstackLevelsToSkip + 1, lines);     // + 1 to skip the call to dumpCallstack

				va_list empty;
				va_start(empty, callstackLevelsToSkip);
				va_end(empty);

				for (uint i = 0; i < lines.size(); i++) {
					messageHandler->log(lines[i], empty);
					delete lines[i];
				}
			}
#endif
		}
		struct Win32AssertHandler : public AssertHandler
		{
			// Flush the message queue. This is necessary for the message box to show up.
			static void flushMessageQueue()
			{
				MSG msg;
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					//if( msg.message == WM_QUIT ) break;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			// Assert handler method.
			virtual int assertion(const char * exp, const char * file, int line, const char * func, const char * msg, va_list arg)
			{
				int ret = EN_ABORT_EXIT;

				StringBuilder error_string;
				error_string.format("*** Assertion failed: %s\n    On file: %s\n    On line: %d\n", exp, file, line);
				if (func != NULL) {
					error_string.appendFormat("    On function: %s\n", func);
				}
				if (msg != NULL) {
					error_string.append("    Message: ");
					va_list tmp;
					va_copy(tmp, arg);
					error_string.appendFormatList(msg, tmp);
					va_end(tmp);
					error_string.append("\n");
				}

				// Print stack trace:
				debug::dumpInfo();

				if (debug::isDebuggerPresent()) {
					return EN_ABORT_DEBUG;
				}

				if (s_interactive) {
					flushMessageQueue();
					int action = MessageBoxA(NULL, error_string.str(), "Assertion failed", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TOPMOST);
					switch (action) {
					case IDRETRY:
						ret = EN_ABORT_DEBUG;
						break;
					case IDIGNORE:
						ret = EN_ABORT_IGNORE;
						break;
					case IDABORT:
					default:
						ret = EN_ABORT_EXIT;
						break;
					}
				}

				if (ret == EN_ABORT_EXIT) {
					exit(EXIT_FAILURE + 1);
				}

				return ret;
			}
		};

		struct MyMessageHandler : public MessageHandler {
			MyMessageHandler() {
				debug::setMessageHandler(this);
			}
			~MyMessageHandler() {
				debug::resetMessageHandler();
			}

			virtual void log(const char * str, va_list arg) {
				va_list val;
				va_copy(val, arg);
				vfprintf(stderr, str, arg);
				va_end(val);
			}
		};

	}

	MyMessageHandler messageHandler;


	void debug::dumpInfo()
	{
		if (true)
		{
			void * trace[64];
			int size = backtrace(trace, 64);

			enDebug("\nDumping stacktrace:\n");

			std::vector<const char *> lines;
			writeStackTrace(trace, size, 1, lines);

			for (math::uint i = 0; i < lines.size(); i++) {
				enDebug("%s", lines[i]);
				delete lines[i];
			}
		}
	}

	void debug::dumpCallstack(MessageHandler *messageHandler, int callstackLevelsToSkip /*= 0*/)
	{
		dumpCallstackImpl(messageHandler, callstackLevelsToSkip);
	}

	void debug::setMessageHandler(MessageHandler * messageHandler)
	{
		s_message_handler = messageHandler;
	}

	void debug::resetMessageHandler()
	{
		s_message_handler = NULL;

	}

	void debug::setAssertHandler(AssertHandler * assertHanlder)
	{
		s_assert_handler = assertHanlder;
	}

	void debug::resetAssertHandler()
	{
		s_assert_handler = NULL;
	}

	void debug::enableSigHandler(bool interactive)
	{

	}

	void debug::disableSigHandler()
	{

	}

	bool debug::isDebuggerPresent()
	{
		return false;
	}

	bool debug::attachToDebugger()
	{
#if defined EN_OS_WIN32
		if (isDebuggerPresent() == FALSE) {
			
			Path process(1024);
			process.copy("\"");
			GetSystemDirectoryA(process.str() + 1, 1024 - 1);

			process.appendSeparator();

			process.appendFormat("VSJitDebugger.exe\" -p %lu", ::GetCurrentProcessId());

			STARTUPINFOA sSi;
			memset(&sSi, 0, sizeof(sSi));

			PROCESS_INFORMATION sPi;
			memset(&sPi, 0, sizeof(sPi));

			BOOL b = CreateProcessA(NULL, process.str(), NULL, NULL, FALSE, 0, NULL, NULL, &sSi, &sPi);
			if (b != FALSE) {
				::WaitForSingleObject(sPi.hProcess, INFINITE);

				DWORD dwExitCode;
				::GetExitCodeProcess(sPi.hProcess, &dwExitCode);
				if (dwExitCode != 0) //if exit code is zero, a debugger was selected
					b = FALSE;
			}

			if (sPi.hThread != NULL) ::CloseHandle(sPi.hThread);
			if (sPi.hProcess != NULL) ::CloseHandle(sPi.hProcess);

			if (b == FALSE)
				return false;

			for (int i = 0; i < 5 * 60; i++) {
				if (isDebuggerPresent())
					break;
				::Sleep(200);
			}
		}
#endif  

		return true;
	}

	void debug::terminate(int code)
	{
#if defined EN_OS_WIN32
		EnterCriticalSection(&s_handler_critical_section);

		writeMiniDump(NULL);

		const int max_stack_size = 64;
		void * trace[max_stack_size];
		int size = backtrace(trace, max_stack_size);

		// @@ Use win32's CreateFile?
		FILE * fp = IO::fileOpen("crash.txt", "wb");
		if (fp != NULL) {
			std::vector<const char *> lines;
			writeStackTrace(trace, size, 0, lines);

			for (math::uint i = 0; i < lines.size(); i++) {
				fputs(lines[i], fp);
				delete lines[i];
			}

			// @@ Add more info to crash.txt?

			fclose(fp);
		}

		LeaveCriticalSection(&s_handler_critical_section);
#endif

		exit(code);
	}

}

using namespace base;

int enAbort(const char *exp, const char *file, int line, const char * func /*= NULL*/, const char * msg /*= NULL*/, ...)
{
#if defined EN_OS_WIN32
	static Win32AssertHandler s_default_assert_handler;
#else
#endif

	va_list arg;
	va_start(arg, msg);
	AssertHandler * handler = s_assert_handler != NULL ? s_assert_handler : &s_default_assert_handler;
	int result = handler->assertion(exp, file, line, func, msg, arg);
	va_end(arg);

	return result;
}

void enDebugPrint(const char *msg, ...)
{
	va_list arg;
	va_start(arg, msg);
	if (s_message_handler != NULL) {
		s_message_handler->log(msg, arg);
	}
	va_end(arg);
}
