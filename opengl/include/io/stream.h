
//
//  stream.h
//  OpenGL
//
//  Created by xt on 19/8/10.
//  Copyright (c) 2019  xt. All rights reserved.
//

#ifndef __OpenGL__Stream__
#define __OpenGL__Stream__

#include "decl.h"
#include "type.h"
#include <stdio.h> 

using namespace math;
namespace IO
{
	// Portable version of fopen.
	inline FILE * fileOpen(const char * fileName, const char * mode)
	{
#if EN_CC_MSVC && _MSC_VER >= 1400
		FILE * fp;
		if (fopen_s(&fp, fileName, mode) == 0) {
			return fp;
		}
		return NULL;
#else
		return fopen(fileName, mode);
#endif
	}

	class LIBENIGHT_EXPORT Stream
	{
	public:
		enum ByteOrder {
			LittleEndian = false,
			BigEndian = true,
		};

		static ByteOrder getSystemByteOrder() 
		{
#if EN_LITTLE_ENDIAN
			return LittleEndian;
#else
			return BigEndian;
#endif
		}

		void setByteOrder(ByteOrder bo) { m_byteOrder = bo; }

		ByteOrder byteOrder() const { return m_byteOrder; }

		/// Serialize the given data.
		virtual uint serializeData(void * data, uint len) = 0;

		virtual void seek(uint pos) = 0;

		//return current position
		virtual uint tell() const = 0;

		virtual uint size() const = 0;

		virtual bool isError() const = 0;

		virtual void clearError() = 0;

		virtual bool isAtEnd() const = 0;

		virtual bool isSeekable() const = 0;

		virtual bool isLoading() const = 0;

		virtual bool isSaving() const = 0;

		void advance(uint offset) { seek(tell() + offset); }

		Stream():m_byteOrder(LittleEndian) {};
		virtual ~Stream() {};

		friend Stream & operator<<(Stream & s, bool & c) 
		{
#if EN_OS_DARWIN && !EN_CC_CPP11
			uint8 b = c ? 1 : 0;
			s.serializeData(&b, 1);
			c = (b == 1);
#else
			s.serializeData(&c, 1);
#endif
			return s;
		}

		friend Stream & operator<<(Stream & s, char & c) 
		{
			s.serializeData(&c, 1);
			return s;
		}
		
		friend Stream & operator<<(Stream & s, uint8 & c) 
		{
			s.serializeData(&c, 1);
			return s;
		}
		
		friend Stream & operator<<(Stream & s, int8 & c) 
		{
			s.serializeData(&c, 1);
			return s;
		}
		friend Stream & operator<<(Stream & s, uint16 & c) 
		{
			return s.byteOrderSerialize(&c, 2);
		}
		
		friend Stream & operator<<(Stream & s, int16 & c) 
		{
			return s.byteOrderSerialize(&c, 2);
		}
		friend Stream & operator<<(Stream & s, uint32 & c) {
			return s.byteOrderSerialize(&c, 4);
		}
		friend Stream & operator<<(Stream & s, int32 & c) {
			return s.byteOrderSerialize(&c, 4);
		}
		friend Stream & operator<<(Stream & s, uint64 & c) {
			return s.byteOrderSerialize(&c, 8);
		}
		friend Stream & operator<<(Stream & s, int64 & c) {
			return s.byteOrderSerialize(&c, 8);
		}
		friend Stream & operator<<(Stream & s, float & c) {
			return s.byteOrderSerialize(&c, 4);
		}
		friend Stream & operator<<(Stream & s, double & c) {
			return s.byteOrderSerialize(&c, 8);
		}

	protected:
			Stream & byteOrderSerialize(void * v, uint len) {
				if (m_byteOrder == getSystemByteOrder()) {
					serializeData(v, len);
				}
				else {
					for (uint i = len; i > 0; i--) {
						serializeData((uint8 *)v + i - 1, 1);
					}
				}
				return *this;
			}
	private:
		ByteOrder m_byteOrder;
	};

	class LIBENIGHT_EXPORT StdStream : public Stream
	{
		EN_FORBID_COPY(StdStream);
	public:

		/// Ctor.
		StdStream(FILE * fp, bool autoclose) : m_fp(fp), m_autoclose(autoclose) { }

		/// Dtor. 
		virtual ~StdStream()
		{
			if (m_fp != NULL && m_autoclose) {
#if EN_OS_WIN32
				_fclose_nolock(m_fp);
#else
				fclose(m_fp);
#endif
			}
		}

			void	setFile(FILE* fp) { m_fp = fp; }

		/** @name Stream implementation. */
		//@{
		virtual void seek(uint pos)
		{
#if EN_OS_WIN32
			_fseek_nolock(m_fp, pos, SEEK_SET);
#else
			fseek(m_fp, pos, SEEK_SET);
#endif
		}

		virtual uint tell() const
		{
#if EN_OS_WIN32
			return _ftell_nolock(m_fp);
#else
			return (uint)ftell(m_fp);
#endif
		}

		virtual uint size() const
		{
#if EN_OS_WIN32
			uint pos = _ftell_nolock(m_fp);
			_fseek_nolock(m_fp, 0, SEEK_END);
			uint end = _ftell_nolock(m_fp);
			_fseek_nolock(m_fp, pos, SEEK_SET);
#else
			uint pos = (uint)ftell(m_fp);
			fseek(m_fp, 0, SEEK_END);
			uint end = (uint)ftell(m_fp);
			fseek(m_fp, pos, SEEK_SET);
#endif
			return end;
		}

		virtual bool isError() const
		{
			return m_fp == NULL || ferror(m_fp) != 0;
		}

		virtual void clearError()
		{
			clearerr(m_fp);
		}

		// @@ The original implementation uses feof, which only returns true when we attempt to read *past* the end of the stream. 
		// That is, if we read the last byte of a file, then isAtEnd would still return false, even though the stream pointer is at the file end. This is not the intent and was inconsistent with the implementation of the MemoryStream, a better 
		// implementation uses use ftell and fseek to determine our location within the file.
		virtual bool isAtEnd() const
		{
			if (m_fp == NULL) return true;
#if EN_OS_WIN32
			uint pos = _ftell_nolock(m_fp);
			_fseek_nolock(m_fp, 0, SEEK_END);
			uint end = _ftell_nolock(m_fp);
			_fseek_nolock(m_fp, pos, SEEK_SET);
#else
			uint pos = (uint)ftell(m_fp);
			fseek(m_fp, 0, SEEK_END);
			uint end = (uint)ftell(m_fp);
			fseek(m_fp, pos, SEEK_SET);
#endif
			return pos == end;
		}

		/// Always true.
		virtual bool isSeekable() const { return true; }
		//@}

	protected:

		FILE * m_fp;
		bool m_autoclose;

	};

	class LIBENIGHT_EXPORT StdInputStream :public StdStream
	{
	public:
		EN_FORBID_COPY(StdInputStream);

		StdInputStream(const char * name) : StdStream(fileOpen(name, "rb"), /*autoclose=*/true) { }

		StdInputStream(FILE * fp, bool autoclose = true) : StdStream(fp, autoclose)
		{
		}

		virtual bool isLoading() const
		{
			return true;
		}

		virtual bool isSaving() const
		{
			return false;
		}

		virtual uint serializeData(void * data, uint len)
		{
#if EN_OS_WIN32
			return (uint)_fread_nolock(data, 1, len, m_fp);
#elif EN_OS_LINUX
			return (uint)fread_unlocked(data, 1, len, m_fp);
#elif EN_OS_DARWIN
			// @@ No error checking, always returns len.
			for (uint i = 0; i < len; i++) {
				((char *)data)[i] = getc_unlocked(m_fp);
			}
			return len;
#else
			return (uint)fread(data, 1, len, m_fp);
#endif

		}

	};

	class LIBENIGHT_EXPORT StdOutputStream : public StdStream
	{
		EN_FORBID_COPY(StdOutputStream);
	public:

		StdOutputStream(const char * name) : StdStream(fileOpen(name, "wb"), /*autoclose=*/true) { }

		StdOutputStream(FILE * fp, bool autoclose) : StdStream(fp, autoclose)
		{
		}

		/** @name Stream implementation. */
		//@{
		/// Write data.
		virtual uint serializeData(void * data, uint len)
		{
#if EN_OS_WIN32
			return (uint)_fwrite_nolock(data, 1, len, m_fp);
#elif EN_OS_LINUX
			return (uint)fwrite_unlocked(data, 1, len, m_fp);
#elif EN_OS_DARWIN
			// @@ No error checking, always returns len.
			for (uint i = 0; i < len; i++) {
				putc_unlocked(((char *)data)[i], m_fp);
			}
			return len;
#else
			return (uint)fwrite(data, 1, len, m_fp);
#endif
		}

		virtual bool isLoading() const
		{
			return false;
		}

		virtual bool isSaving() const
		{
			return true;
		}
		//@}

	};
		class File
	{
	public:
		static bool existFile(const char*file);
		static bool existDirectory(const char*dir);
		static bool createDirectory(const char*file);
	};
};

#endif
