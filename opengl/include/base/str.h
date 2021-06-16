#pragma once

#ifndef __STR_H_
#define __STR_H_

#include "decl.h"
#include "type.h" 
#include "sys.h"
#if defined EN_OS_WIN32 || defined EN_OS_WIN64
#include "defsVcWin32.h"
#endif // EN_OS_WIN32

#if defined EN_OS_WIN32
#define EN_PATH_SEPARATOR '\\'
#else
#define EN_PATH_SEPARATOR '/'
#endif

namespace base
{
	math::uint strHash(const char * str, math::uint h) ;

	/// String hash based on Bernstein's hash.
	inline math::uint strHash(const char * data, math::uint h = 5381)
	{
		math::uint i = 0;
		while (data[i] != 0) {
			h = (33 * h) ^ math::uint(data[i]);
			i++;
		}
		return h;
	}

	struct Hash {  //functor fang han shu 
		math::uint operator()(const char * str) const { return strHash(str); }
	};

	LIBENIGHT_EXPORT math::uint strLen(const char * str) ;                       // Asserts on NULL strings.

	LIBENIGHT_EXPORT int strDiff(const char * s1, const char * s2) ;       // Asserts on NULL strings.
	LIBENIGHT_EXPORT int strCaseDiff(const char * s1, const char * s2) ;   // Asserts on NULL strings.
	LIBENIGHT_EXPORT bool strEqual(const char * s1, const char * s2) ;     // Accepts NULL strings.
	LIBENIGHT_EXPORT bool strCaseEqual(const char * s1, const char * s2) ; // Accepts NULL strings.

	struct Equal{
		bool operator()(const char * a, const char * b) const { return strEqual(a, b); }
	};

	LIBENIGHT_EXPORT bool strBeginsWith(const char * dst, const char * prefix) ;
	LIBENIGHT_EXPORT bool strEndsWith(const char * dst, const char * suffix) ;

	LIBENIGHT_EXPORT void strCpy(char * dst,math::uint size, const char * src);
	LIBENIGHT_EXPORT void strCpy(char * dst,math::uint size, const char * src, math::uint len);
	LIBENIGHT_EXPORT void strCat(char * dst,math::uint size, const char * src);

	LIBENIGHT_EXPORT const char * strSkipWhiteSpace(const char * str);
	LIBENIGHT_EXPORT char * strSkipWhiteSpace(char * str);

	LIBENIGHT_EXPORT bool strMatch(const char * str, const char * pat) ;

	LIBENIGHT_EXPORT bool isNumber(const char * str) ;

	template <int count> void strCpySafe(char(&buffer)[count], const char *src) {
		strCpy(buffer, count, src);
	}

	template <int count> void strCatSafe(char(&buffer)[count], const char * src) {
		strCat(buffer, count, src);
	}


	class LIBENIGHT_EXPORT StringBuilder
	{
	public:

		StringBuilder();
		explicit StringBuilder(math::uint size_hint);
		StringBuilder(const char * str);
		StringBuilder(const char * str, math::uint len);
		StringBuilder(const StringBuilder & other);

		~StringBuilder();

		StringBuilder & format(const char * format, ...) __attribute__((format(printf, 2, 3)));
		StringBuilder & formatList(const char * format, va_list arg);

		StringBuilder & append(char c);
		StringBuilder & append(const char * str);
		StringBuilder & append(const char * str, math::uint len);
		StringBuilder & append(const StringBuilder & str);
		StringBuilder & appendFormat(const char * format, ...) __attribute__((format(printf, 2, 3)));
		StringBuilder & appendFormatList(const char * format, va_list arg);

		StringBuilder & appendSpace(math::uint n);

		StringBuilder & number(int i, int base = 10);
		StringBuilder & number(math::uint i, int base = 10);

		StringBuilder & reserve(math::uint size_hint);
		StringBuilder & copy(const char * str);
		StringBuilder & copy(const char * str, math::uint len);
		StringBuilder & copy(const StringBuilder & str);

		void removeChar(char c);

		bool endsWith(const char * str) const;
		bool beginsWith(const char * str) const;

		char * reverseFind(char c);

		void reset();
		inline bool isNull() const { return m_size == 0; }

		inline const char * str() const { return m_str; }
		inline char * str() { return m_str; }

		char * release();        
		void acquire(char *);   

								/// Implement value semantics.
		StringBuilder & operator=(const StringBuilder & s) {
			return copy(s);
		}

		/// Implement value semantics.
		StringBuilder & operator=(const char * s) {
			return copy(s);
		}

		/// Equal operator.
		bool operator==(const StringBuilder & s) const {
			return strMatch(s.m_str, m_str);
		}

		/// Return the exact length.
		math::uint length() const { return isNull() ? 0 : strLen(m_str); }

		/// Return the size of the string container.
		math::uint capacity() const { return m_size; }

		/// Return the hash of the string.
		math::uint hash() const { return isNull() ? 0 : strHash(m_str); }

		// Swap strings.
		friend void swap(StringBuilder & a, StringBuilder & b);

	protected:

		/// Size of the string container.
		math::uint m_size;

		/// String.
		char * m_str;
	};

	class LIBENIGHT_EXPORT Path : public StringBuilder
	{
	public:
		Path() : StringBuilder() {}
		explicit Path(int size_hint) : StringBuilder(size_hint) {}
		Path(const char * str) : StringBuilder(str) {}
		Path(const Path & path) : StringBuilder(path) {}

		const char * fileName() const;
		const char * extension() const;

		void translatePath(char pathSeparator = EN_PATH_SEPARATOR);

		void appendSeparator(char pathSeparator = EN_PATH_SEPARATOR);

		void stripFileName();
		void stripExtension();

		// statics
		static char separator();
		static const char * fileName(const char *);
		static const char * extension(const char *);

		static void translatePath(char * path, char pathSeparator = EN_PATH_SEPARATOR);
	};
}

#endif