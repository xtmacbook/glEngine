
//
//  contextMap.h
//  OpenGL
//
//  Created by xt on 7/12/16
//  Copyright (c) 2016Äê xt. All rights reserved.
//

#include "common.h"
#include "type.h"
#include "mesh.h"
#include "gls.h"
#include "log.h"
#define  _Q_TEST_

#ifdef _Q_TEST_
#include "glQuery.h"
#endif

using namespace math;

//WGL_CONTEXT_DEBUG_BIT_ARB

/**
 * persistent:you are not frequently mapping and unmapping the buffer. You map it persistently when you create the buffer, and keep it mapped until it's time to delete the buffer.
 */
template <class T>
class  FlashBuffer :public base::BaseObject
{
public:

	enum ContextMapMode
	{
		ModeWrite = 0, //stand glMapBufferRange

		ModeInvalidateBuffer, //orphan buffer re-speification and no need the sync object for synchronize,and would create new object if the current obj is used
		
		ModeUnsynchronized,   //buffer update nedd sync object

		ModeBufferData,    //use normal buffer data and no the glMapBufferRange
		ModeBufferSubData, 
		      
		ModeFlushExplicit,

		ModePersistentCoheren, //persistent GL version >= 4.4
		ModePersistentFlush, //persistent
		ModePersistenMemoryBarrier //now no implement //7
	};

	struct PersistentBufferRange
	{
		PersistentBufferRange()
		{
			begin_ = 0;
			sync_ = 0;
		}
		size_t begin_;
		GLsync sync_;
	};

	FlashBuffer(unsigned int blockSize);
	~FlashBuffer();

	void setTarget(GLenum t);
	void mapBuffer();
	void flush_data();
	void create_buffers(unsigned int flag,bool unbind = true);

	void waitBuffer(void);
	void lockBuffer(void);
	void lockBuffer(GLsync& syncObj);
	void waitBuffer(GLsync& syncObj);

	T*   getBufferPtr();
	void setDataSize(unsigned);

	unsigned int blockSize()const;
	
	void			bind();
	void			unbind();

	math::uint64	timestamp();
	void		    swapQueryTime();

	inline unsigned VBO() { return vbo_; }
private:
	unsigned vbo_ = 0;
	PersistentBufferRange gSyncRanges_[3]; //now persisten use 3 section
	T*		data_ptr_;
	T*		persistentRangeMapStartPtr_;
	unsigned int  gPersistentRangeIndex_;

	unsigned		data_ptr_size_;
	ContextMapMode	mode_;
	unsigned int			target_;

	unsigned int buffer_pool_size_;

#ifdef _Q_TEST_
	GLQuery			query_;
#endif // _Q_TEST_

};

template <class T>
void FlashBuffer<T>::swapQueryTime()
{
#ifdef _Q_TEST_
	query_.swapQueryBuffers();
#endif // _Q_TEST_
}

template <class T>
math::uint64 FlashBuffer<T>::timestamp() 
{
#ifdef _Q_TEST_
	return query_.time();
#else 
	return 0;
#endif
}

template <class T>
void FlashBuffer<T>::bind()
{
	glBindBuffer(target_, vbo_);
}

template <class T>
void FlashBuffer<T>::unbind()
{
	glBindBuffer(target_, 0);
}


template <class T>
unsigned int FlashBuffer<T>::blockSize() const
{
	return buffer_pool_size_;
}

template <class T>
void FlashBuffer<T>::setDataSize(unsigned s)
{
	data_ptr_size_ = s;
}

template <class T>
T* FlashBuffer<T>::getBufferPtr()
{
	return data_ptr_;
}

template <class T>
void FlashBuffer<T>::setTarget(GLenum t)
{
	target_ = t;
}

template <class T>
void FlashBuffer<T>::waitBuffer(GLsync& syncObj)
{
	if (syncObj)
	{
		while (1)
		{
			//glClientWaitSync: To block all CPU operations until a sync object is signaled
			GLenum waitReturn = glClientWaitSync(syncObj, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
			if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
				return;
		}
	}
}

template <class T>
void FlashBuffer<T>::lockBuffer(GLsync& syncObj)
{
	if (syncObj) glDeleteSync(syncObj);
	syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

template <class T>
void FlashBuffer<T>::lockBuffer(void)
{
	if (ModePersistentCoheren == mode_ || ModePersistentFlush == mode_)
	{
		lockBuffer(gSyncRanges_[gPersistentRangeIndex_].sync_);
		gPersistentRangeIndex_ = (gPersistentRangeIndex_ + 1) % 3;
	}
	else if (ModeUnsynchronized == mode_)
	{
		lockBuffer(gSyncRanges_[0].sync_);
	}
}

template <class T>
void FlashBuffer<T>::waitBuffer(void)
{
	waitBuffer(gSyncRanges_[gPersistentRangeIndex_].sync_);
}

template <class T>
void FlashBuffer<T>::create_buffers(unsigned int model,  bool unbind /*= true*/)
{
	glGenBuffers(1, &vbo_);
	glBindBuffer(target_, vbo_);

	mode_ = (ContextMapMode)model;

	if (ModeInvalidateBuffer == mode_ ||
		ModeBufferSubData == mode_    ||
		ModeBufferData == mode_ ||
		ModeWrite == mode_)
	{
		glBufferData(target_, buffer_pool_size_ * sizeof(T), 0, GL_STREAM_DRAW);
	}
	else if (ModeUnsynchronized == mode_)
	{
		gSyncRanges_[0].begin_ = 0;
		glBufferData(target_, buffer_pool_size_ * sizeof(T), 0, GL_STREAM_DRAW);
	}
	else if (ModePersistentCoheren == mode_ || ModePersistentFlush == mode_)
	{
		gPersistentRangeIndex_ = 0;

		gSyncRanges_[0].begin_ = 0; //for 3 sections and every section start point
		gSyncRanges_[1].begin_ = buffer_pool_size_ ;
		gSyncRanges_[2].begin_ = buffer_pool_size_  * 2;

		if (ModePersistentCoheren == mode_)
		{
			GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			glBufferStorage(target_, buffer_pool_size_ * sizeof(T) * 3, 0, flags); //glBufferStorage to create and initialize immutable  data store
			data_ptr_ = reinterpret_cast<T*>(glMapBufferRange(target_, 0,
				buffer_pool_size_ * sizeof(T) * 3, flags));
		}
		else
		{
			GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			glBufferStorage(target_, buffer_pool_size_ * sizeof(T) * 3, 0, flags);
			data_ptr_ = reinterpret_cast<T*>(glMapBufferRange(target_, 0,
				buffer_pool_size_ * sizeof(T) * 3, flags | GL_MAP_FLUSH_EXPLICIT_BIT));
		}

		persistentRangeMapStartPtr_ = data_ptr_;
	}

	else 
	{

	}
	if(unbind) glBindBuffer(target_, 0);
	
	CHECK_GL_ERROR;
}

template <class T>
void FlashBuffer<T>::flush_data()
{
	CHECK_GL_ERROR;

	glBindBuffer(target_, vbo_);
#ifdef _Q_TEST_
	query_.begin();
#endif
	CHECK_GL_ERROR;

	if (ModeInvalidateBuffer == mode_ || 
		ModeUnsynchronized == mode_   ||
		ModeWrite == mode_)
	{
		glUnmapBuffer(target_);
	}
	else if (ModeFlushExplicit == mode_)
	{
		glFlushMappedBufferRange(target_, 0, data_ptr_size_);
		glUnmapBuffer(target_);
	}
	else if (ModeBufferSubData == mode_)
	{
		glBufferSubData(target_, 0, data_ptr_size_, data_ptr_);
	}
	else if (ModeBufferData == mode_)
	{
		glBufferData(target_, data_ptr_size_, 0, GL_STREAM_DRAW);
		glBufferSubData(target_, 0, data_ptr_size_, data_ptr_);
	}
	else if (ModePersistentCoheren == mode_ || ModePersistentFlush == mode_)
	{
		if (ModePersistentFlush == mode_)
		{
			glFlushMappedBufferRange(target_, gSyncRanges_[gPersistentRangeIndex_].begin_,
				buffer_pool_size_ * sizeof(T));
		}
	}

	CHECK_GL_ERROR;
#ifdef _Q_TEST_
	query_.end();
	CHECK_GL_ERROR;
#endif
	glBindBuffer(target_, 0);
}

template <class T>
void FlashBuffer<T>::mapBuffer()
{
	glBindBuffer(target_, vbo_);
	if (ModeInvalidateBuffer == mode_)
	{
		data_ptr_ = reinterpret_cast<T*>(
			glMapBufferRange(
				target_, 0,
				buffer_pool_size_ * sizeof(T),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		data_ptr_size_ = 0;
	}
	else if (ModeUnsynchronized == mode_)
	{
		waitBuffer(gSyncRanges_[0].sync_);
		data_ptr_ = reinterpret_cast<T*>(glMapBufferRange(target_, 0,
			buffer_pool_size_ * sizeof(T), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT));
	}
	else if (ModeBufferSubData == mode_ || ModeBufferData == mode_)
	{
		// map the buffer object into client's memory
		// Note that glMapBuffer() causes sync issue.
		// If GPU is working with this buffer, glMapBuffer() will wait(stall)
		// for GPU to finish its job. To avoid waiting (stall), you can call
		// first glBufferData() with NULL pointer before glMapBuffer().
		// If you do that, the previous data in PBO will be discarded and
		// glMapBuffer() returns a new allocated pointer immediately
		// even if GPU is still working with the previous data.

		if (!data_ptr_)
			data_ptr_ = new T[buffer_pool_size_];
		data_ptr_size_ = 0;
	}
	else if (ModePersistentCoheren == mode_ || ModePersistentFlush == mode_)
	{
		waitBuffer(gSyncRanges_[gPersistentRangeIndex_].sync_);
		size_t startID = gSyncRanges_[gPersistentRangeIndex_].begin_;
		data_ptr_ = persistentRangeMapStartPtr_ + startID;
	}
	else if (ModeFlushExplicit == mode_)
	{
		data_ptr_ = reinterpret_cast<T*>(glMapBufferRange(
			target_,
			0,
			buffer_pool_size_ * sizeof(T),
			GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
	}
	else if (ModeWrite == mode_)
	{
		data_ptr_ = reinterpret_cast<T*>(glMapBufferRange(
			target_,
			0,
			buffer_pool_size_ * sizeof(T),
			GL_MAP_WRITE_BIT));
	}
	glBindBuffer(target_, vbo_);
}

template <class T>
FlashBuffer<T>::~FlashBuffer()
{
	if (ModePersistentCoheren == mode_)
	{
		glUnmapBuffer(target_);
	}

	if (vbo_ != 0)
	{
		glDeleteBuffers(1, &vbo_);
	}

#ifdef _Q_TEST_
	query_.deleteQuery();
#endif
}

template <class T>
FlashBuffer<T>::FlashBuffer(unsigned int bs):buffer_pool_size_(bs)
{
	data_ptr_ = NULL;
	persistentRangeMapStartPtr_ = NULL;
#ifdef _Q_TEST_
	query_.genQueries();
	CHECK_GL_ERROR;
#endif // _Q_TEST_

}


