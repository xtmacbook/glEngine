
#include "str.h"
#include "debug.h"
#include "util.h"
#include "common.h"

#include <math.h>   // log
#include <stdio.h>  // vsnprintf
#include <string.h> // strlen, strcmp, etc.

#if defined EN_OS_WIN32 
#include <stdarg.h> // vsnprintf
#endif

using namespace base;

namespace
{
	static char * strAlloc(math::uint size)
	{
		return (char*)malloc(size);
	}

	static char * strReAlloc(char * str, math::uint size)
	{
		return (char*)realloc(str, size);
	}

	static void strFree(char * str)
	{
		return free(str);
	}

	// helper function for integer to string conversion.
	static char * i2a(math::uint i, char *a, math::uint r)
	{
		if (i / r > 0) {
			a = i2a(i / r, a, r);
		}
		*a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i % r];
		return a + 1;
	}

	// Locale independent functions.
	static inline char toUpper(char c) {
		return (c<'a' || c>'z') ? (c) : (c + 'A' - 'a');
	}
	static inline char toLower(char c) {
		return (c<'A' || c>'Z') ? (c) : (c + 'a' - 'A');
	}
	static inline bool isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
	static inline bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
	static inline bool isAlnum(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
	}

}

namespace base
{
	math::uint strLen(const char * str)
	{
		enDebugAssert(str != NULL);
		return (strlen(str));
	}

	int strDiff(const char * s1, const char * s2)
	{
		enDebugCheck(s1 != NULL);
		enDebugCheck(s2 != NULL);
		return strcmp(s1, s2);
	}

	int strCaseDiff(const char * s1, const char * s2)
	{
		enDebugCheck(s1 != NULL);
		enDebugCheck(s1 != NULL);
#if defined EN_OS_WIN32 
		return _stricmp(s1, s2);
#else
		return strcasecmp(s1, s2);
#endif
	}

	bool strEqual(const char * s1, const char * s2)
	{
		if (s1 == s2) return true;
		if (s1 == NULL || s2 == NULL) return false;
		return strcmp(s1, s2) == 0;
	}

	bool strCaseEqual(const char * s1, const char * s2)
	{
		if (s1 == s2) return true;
		if (s1 == NULL || s2 == NULL) return false;
		return strCaseDiff(s1, s2) == 0;
	}

	bool strBeginsWith(const char * str, const char * prefix)
	{
		return strncmp(str, prefix, strlen(prefix)) == 0;
	}

	bool strEndsWith(const char * str, const char * suffix)
	{
		math::uint ml = strLen(str);
		math::uint sl = strLen(suffix);
		if (ml < sl) return false;
		return strncmp(str + ml - sl, suffix, sl) == 0;
	}

	// @@ Add asserts to detect overlap between dst and src?
	void strCpy(char * dst, math::uint size, const char * src)
	{
		enDebugCheck(dst != NULL);
		enDebugCheck(src != NULL);
#if defined EN_OS_WIN32  && _MSC_VER >= 1400
		strcpy_s(dst, size, src);
#else
		NV_UNUSED(size);
		strcpy(dst, src);
#endif
	}

	void strCpy(char * dst, math::uint size, const char * src, math::uint len)
	{
		enDebugCheck(dst != NULL);
		enDebugCheck(src != NULL);
#if defined EN_OS_WIN32  && _MSC_VER >= 1400
		strncpy_s(dst, size, src, len);
#else
		int n = min(len + 1, size);
		strncpy(dst, src, n);
		dst[n - 1] = '\0';
#endif
	}

	void strCat(char * dst, math::uint size, const char * src)
	{
		enDebugCheck(dst != NULL);
		enDebugCheck(src != NULL);
#if defined EN_OS_WIN32  && _MSC_VER >= 1400
		strcat_s(dst, size, src);
#else
		NV_UNUSED(size);
		strcat(dst, src);
#endif
	}

	const char * strSkipWhiteSpace(const char * str)
	{
		enDebugCheck(str != NULL);
		while (*str == ' ') str++;
		return str;
	}

	char * strSkipWhiteSpace(char * str)
	{
		enDebugCheck(str != NULL);
		while (*str == ' ') str++;
		return str;
	}


	/** Pattern matching routine. I don't remember where did I get this. */
	bool strMatch(const char * str, const char * pat)
	{
		enDebugCheck(str != NULL);
		enDebugCheck(pat != NULL);

		char c2;

		while (true) {
			if (*pat == 0) {
				if (*str == 0) return true;
				else         return false;
			}
			if ((*str == 0) && (*pat != '*')) return false;
			if (*pat == '*') {
				pat++;
				if (*pat == 0) return true;
				while (true) {
					if (strMatch(str, pat)) return true;
					if (*str == 0) return false;
					str++;
				}
			}
			if (*pat == '?') goto match;
			if (*pat == '[') {
				pat++;
				while (true) {
					if ((*pat == ']') || (*pat == 0)) return false;
					if (*pat == *str) break;
					if (pat[1] == '-') {
						c2 = pat[2];
						if (c2 == 0) return false;
						if ((*pat <= *str) && (c2 >= *str)) break;
						if ((*pat >= *str) && (c2 <= *str)) break;
						pat += 2;
					}
					pat++;
				}
				while (*pat != ']') {
					if (*pat == 0) {
						pat--;
						break;
					}
					pat++;
				}
				goto match;
			}

			if (*pat == EN_PATH_SEPARATOR) {
				pat++;
				if (*pat == 0) return false;
			}
			if (*pat != *str) return false;

		match:
			pat++;
			str++;
		}
	}

	bool isNumber(const char * str) {
		while (*str != '\0') {
			if (!isDigit(*str)) return false;
			str++;
		}
		return true;
	}

	 
	StringBuilder::StringBuilder() : m_size(0), m_str(NULL)
	{
	}

	StringBuilder::StringBuilder(math::uint size_hint) : m_size(size_hint)
	{
		enDebugCheck(m_size > 0);
		m_str = strAlloc(m_size);
		*m_str = '\0';
	}

	StringBuilder::StringBuilder(const StringBuilder & s) : m_size(0), m_str(NULL)
	{
		copy(s);
	}

	StringBuilder::StringBuilder(const char * s) : m_size(0), m_str(NULL)
	{
		if (s != NULL) {
			copy(s);
		}
	}

	StringBuilder::StringBuilder(const char * s, math::uint len) : m_size(0), m_str(NULL)
	{
		copy(s, len);
	}

	StringBuilder::~StringBuilder()
	{
		strFree(m_str);
	}


	StringBuilder & StringBuilder::format(const char * fmt, ...)
	{
		enDebugCheck(fmt != NULL);
		va_list arg;
		va_start(arg, fmt);

		formatList(fmt, arg);

		va_end(arg);

		return *this;
	}


	StringBuilder & StringBuilder::formatList(const char * fmt, va_list arg)
	{
		enDebugCheck(fmt != NULL);

		if (m_size == 0) {
			m_size = 64;
			m_str = strAlloc(m_size);
		}

		va_list tmp;
		va_copy(tmp, arg);
#if defined EN_OS_WIN32  && _MSC_VER >= 1400
		int n = vsnprintf_s(m_str, m_size, _TRUNCATE, fmt, tmp);
#else
		int n = vsnprintf(m_str, m_size, fmt, tmp);
#endif
		va_end(tmp);

		while (n < 0 || n >= int(m_size)) {
			if (n > -1) {
				m_size = n + 1;
			}
			else {
				m_size *= 2;
			}

			m_str = strReAlloc(m_str, m_size);

			va_copy(tmp, arg);
#if defined EN_OS_WIN32  && _MSC_VER >= 1400
			n = vsnprintf_s(m_str, m_size, _TRUNCATE, fmt, tmp);
#else
			n = vsnprintf(m_str, m_size, fmt, tmp);
#endif
			va_end(tmp);
		}

		enDebugCheck(n < int(m_size));

		// Make sure it's null terminated.
		enDebugCheck(m_str[n] == '\0');
		//str[n] = '\0';

		return *this;
	}


	StringBuilder & StringBuilder::append(char c)
	{
		return append(&c, 1);
	}

	StringBuilder & StringBuilder::append(const char * s)
	{
		return append(s, math::uint(strlen(s)));
	}

	StringBuilder & StringBuilder::append(const char * s, math::uint len)
	{
		enDebugCheck(s != NULL);

		math::uint offset = length();
		const math::uint size = offset + len + 1;
		reserve(size);
		strCpy(m_str + offset, len + 1, s, len);

		return *this;
	}

	StringBuilder & StringBuilder::append(const StringBuilder & str)
	{
		return append(str.m_str, str.length());
	}


	StringBuilder & StringBuilder::appendFormat(const char * fmt, ...)
	{
		enDebugCheck(fmt != NULL);

		va_list arg;
		va_start(arg, fmt);

		appendFormatList(fmt, arg);

		va_end(arg);

		return *this;
	}


	StringBuilder & StringBuilder::appendFormatList(const char * fmt, va_list arg)
	{
		enDebugCheck(fmt != NULL);

		va_list tmp;
		va_copy(tmp, arg);

		if (m_size == 0) {
			formatList(fmt, arg);
		}
		else {
			StringBuilder tmp_str;
			tmp_str.formatList(fmt, tmp);
			append(tmp_str.str());
		}

		va_end(tmp);

		return *this;
	}

	StringBuilder & StringBuilder::appendSpace(math::uint n)
	{
		if (m_str == NULL) {
			m_size = n + 1;
			m_str = strAlloc(m_size);
			memset(m_str, ' ', m_size);
			m_str[n] = '\0';
		}
		else {
			const math::uint len = strLen(m_str);
			if (m_size < len + n + 1) {
				m_size = len + n + 1;
				m_str = strReAlloc(m_str, m_size);
			}
			memset(m_str + len, ' ', n);
			m_str[len + n] = '\0';
		}

		return *this;
	}


	StringBuilder & StringBuilder::number(int i, int base)
	{
		enCheck(base >= 2);
		enCheck(base <= 36);

		// @@ This needs to be done correctly.
		// length = floor(log(i, base));
		math::uint len = math::uint(log(float(i)) / log(float(base)) + 2); // one more if negative
		reserve(len);

		if (i < 0) {
			*m_str = '-';
			*i2a(math::uint(-i), m_str + 1, base) = 0;
		}
		else {
			*i2a(i, m_str, base) = 0;
		}

		return *this;
	}


	StringBuilder & StringBuilder::number(math::uint i, int base)
	{
		enCheck(base >= 2);
		enCheck(base <= 36);

		math::uint len = math::uint(log(float(i)) / log(float(base)) - 0.5f + 1);
		reserve(len);

		*i2a(i, m_str, base) = 0;

		return *this;
	}


	StringBuilder & StringBuilder::reserve(math::uint size_hint)
	{
		enCheck(size_hint != 0);
		if (size_hint > m_size) {
			m_str = strReAlloc(m_str, size_hint);
			m_size = size_hint;
		}
		return *this;
	}


	StringBuilder & StringBuilder::copy(const char * s)
	{
		enCheck(s != NULL);
		const math::uint str_size = math::uint(strlen(s)) + 1;
		reserve(str_size);
		memcpy(m_str, s, str_size);
		return *this;
	}

	StringBuilder & StringBuilder::copy(const char * s, math::uint len)
	{
		enCheck(s != NULL);
		const math::uint str_size = len + 1;
		reserve(str_size);
		strCpy(m_str, str_size, s, len);
		return *this;
	}


	StringBuilder & StringBuilder::copy(const StringBuilder & s)
	{
		if (s.m_str == NULL) {
			enCheck(s.m_size == 0);
			reset();
		}
		else {
			reserve(s.m_size);
			strCpy(m_str, s.m_size, s.m_str);
		}
		return *this;
	}

	void StringBuilder::removeChar(char c)
	{
		char * src = strchr(m_str, c);
		if (src) {
			char * dst = src;
			src++;
			while (*src) {
				*dst++ = *src++;
			}
			*dst = '\0';
		}
	}

	bool StringBuilder::endsWith(const char * str) const
	{
		math::uint l = math::uint(strlen(str));
		math::uint ml = math::uint(strlen(m_str));
		if (ml < l) return false;
		return strncmp(m_str + ml - l, str, l) == 0;
	}

	bool StringBuilder::beginsWith(const char * str) const
	{
		size_t l = strlen(str);
		return strncmp(m_str, str, l) == 0;
	}

	char * StringBuilder::reverseFind(char c)
	{
		int length = (int)strlen(m_str) - 1;
		while (length >= 0 && m_str[length] != c) {
			length--;
		}
		if (length >= 0) {
			return m_str + length;
		}
		else {
			return NULL;
		}
	}


	void StringBuilder::reset()
	{
		m_size = 0;
		strFree(m_str);
		m_str = NULL;
	}

	char * StringBuilder::release()
	{
		char * str = m_str;
		m_size = 0;
		m_str = NULL;
		return str;
	}

	void StringBuilder::acquire(char * str)
	{
		if (str) {
			m_size = strLen(str) + 1;
			m_str = str;
		}
		else {
			m_size = 0;
			m_str = NULL;
		}
	}

	// Swap strings.
	void swap(StringBuilder & a, StringBuilder & b) {
		::swap(a.m_size, b.m_size);
		::swap(a.m_str, b.m_str);
	}

	const char * Path::fileName() const
	{
		return fileName(m_str);
	}

	const char * Path::fileName(const char *str)
	{
		enCheck(str != NULL);

		int length = (int)strlen(str) - 1;
		while (length >= 0 && str[length] != '\\' && str[length] != '/') {
			length--;
		}

		return &str[length + 1];
	}

	const char * Path::extension() const
	{
		return extension(m_str);
	}

	const char * Path::extension(const char *str)
	{
		enCheck(str != NULL);

		int length, l;
		l = length = (int)strlen(str);
		while (length > 0 && str[length] != '.') {
			length--;
			if (str[length] == '\\' || str[length] == '/') {
				return &str[l]; // no extension
			}
		}
		if (length == 0) {
			return &str[l];
		}
		return &str[length];
	}

	void Path::translatePath(char pathSeparator /*= EN_PATH_SEPARATOR*/)
	{
		if (!isNull()) {
			translatePath(m_str, pathSeparator);
		}
	}

	void Path::translatePath(char * path, char pathSeparator /*= EN_PATH_SEPARATOR*/)
	{
		if (path != NULL) {
			for (int i = 0;; i++) {
				if (path[i] == '\0') break;
				if (path[i] == '\\' || path[i] == '/') path[i] = pathSeparator;
			}
		}
	}

	void Path::appendSeparator(char pathSeparator /*= EN_PATH_SEPARATOR*/)
	{
		enCheck(!isNull());

		const math::uint l = length();

		if (m_str[l] != '\\' && m_str[l] != '/') {
			char separatorString[] = { pathSeparator, '\0' };
			append(separatorString);
		}
	}

	void Path::stripFileName()
	{
		enCheck(m_str != NULL);

		int length = (int)strlen(m_str) - 1;
		while (length > 0 && m_str[length] != '/' && m_str[length] != '\\') {
			length--;
		}
		if (length) {
			m_str[length + 1] = 0;
		}
		else {
			m_str[0] = 0;
		}
	}

	void Path::stripExtension()
	{
		enCheck(m_str != NULL);

		int length = (int)strlen(m_str) - 1;
		while (length > 0 && m_str[length] != '.') {
			length--;
			if (m_str[length] == EN_PATH_SEPARATOR) {
				return; // no extension
			}
		}
		if (length > 0) {
			m_str[length] = 0;
		}
	}

	char Path::separator()
	{
		return EN_PATH_SEPARATOR;
	}

}
