
#ifndef __THREAD_H__
#define __THREAD_H__

#include "decl.h"
#include <string>
#include <wtypes.h>

#define TMP_STR_BUF_SIZE 64

namespace base
{
	class LIBENIGHT_EXPORT source_location
	{
	public:

		source_location(
			const char *function_name,
			const char *file_name,
			const int line_number)
			: _function_name(function_name)
			, _file_name(file_name)
			, _line_number(line_number)
		{}

		std::string to_str() const {
			char line[TMP_STR_BUF_SIZE];
			return std::string(_function_name) + "@" + _file_name + '(' + _itoa(_line_number, line, 10) + ") ";
		}

		const char* function_name() const { return _function_name; }
		const char* file_name() const { return _file_name; }
		int line_number() const { return _line_number; }

	private:
		const char* const _function_name;
		const char* const _file_name;
		const int _line_number;

		source_location(const source_location&);
		void operator = (const source_location&);
	};
	class LIBENIGHT_EXPORT Thread
	{
	protected:

		void* _handle;

	public:

		Thread();

		virtual ~Thread();

		virtual void start(const source_location &loc);

		virtual void run() {}

		virtual void stop(const source_location &loc);

		static void* thread_fnc(void* data);

		bool wait_for_end(const unsigned int time = -1);

		void terminate();
	};

	typedef struct TlsWin32
	{
		bool				allocated_;
		DWORD               index_;

	} TlsWin32;

	bool createTls(TlsWin32* tls);

	bool platformSetTls(TlsWin32* tls, void* value);
	void destoryTls(TlsWin32* tls);

	void* platformGetTls(TlsWin32* tls);
} // end of namspace base


#endif  
