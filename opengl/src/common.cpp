#include "common.h"
#include "log.h"
#include "sys.h"
#include <sstream>
#include <vector>

float stringToFloat(const std::string &source)
{
	std::stringstream ss(source.c_str());
	float result;
	ss >> result;
	return result;
}

unsigned int stringToUint(const std::string &source)
{
	std::stringstream ss(source.c_str());
	unsigned int result;
	ss >> result;
	return result;
}

int stringToInt(const std::string &source)
{
	std::stringstream ss(source.c_str());
	int result;
	ss >> result;
	return result;
}

const char * extension(const char * str)
{
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

int strCaseDiff(const char * s1, const char * s2)
{
#ifdef EN_OS_WIN32
	return _stricmp(s1, s2);
#else
	return strcasecmp(s1, s2);
#endif
}

std::wstring utf8toUtf16(const std::string& str)
{
	if (str.empty())
		return std::wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
	{
		LOGE("Failed converting UTF-8 string to UTF-16");
		return std::wstring();
	}

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
	{
		LOGE("Failed converting UTF-8 string to UTF-16");
		return std::wstring();
	}

	return std::wstring(&buffer[0], charsConverted);
}


