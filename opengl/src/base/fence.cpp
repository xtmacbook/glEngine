#include "fence.h"
namespace base
{

	FenceGL3x::FenceGL3x()
	{
		syncObj_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
	}

	FenceGL3x::~FenceGL3x()
	{
		glDeleteSync(syncObj_);
	}

	void FenceGL3x::serverWait()
	{
		glWaitSync(syncObj_, GL_SYNC_FLUSH_COMMANDS_BIT,1);
	}

	ClientWaitResult FenceGL3x::clientWait()
	{
		GLenum waitReturn = glClientWaitSync(syncObj_, GL_SYNC_FLUSH_COMMANDS_BIT,1);

		if (waitReturn == GL_ALREADY_SIGNALED)
			return AlreadySignaled;
		if (waitReturn == GL_CONDITION_SATISFIED)
			return ConSignaled;
		
		return TimeoutExpired;
	}

	SynchronizationStatus FenceGL3x::status()
	{
		GLsizei len;
		GLint val;

		glGetSynciv(syncObj_, GL_SYNC_STATUS, 1,&len,&val);
		if (val == GL_SIGNALED) return SynchronizationStatus::Signaled;
		return SynchronizationStatus::Unsignaled;
	}

	ClientWaitResult FenceGL3x::clientWait(int timeoutInNanoseconds)
	{
		GLenum waitReturn =  glClientWaitSync(syncObj_, GL_SYNC_FLUSH_COMMANDS_BIT,timeoutInNanoseconds);

		if (waitReturn == GL_ALREADY_SIGNALED)
			return AlreadySignaled;
		if (waitReturn == GL_CONDITION_SATISFIED)
			return ConSignaled;

		return TimeoutExpired;
	}
}



