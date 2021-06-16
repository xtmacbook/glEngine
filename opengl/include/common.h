
#include <string>

#ifndef _common_h_
#define  _common_h_

static bool isGlewInit = false;

#define ELEMENTS(array) sizeof(array) / sizeof(array[0])
///////////////////////////////////////////////////////////////////
// common helper fun
///////////////////////////////////////////////////////////////////
unsigned int											stringToUint(const std::string &source);
int														stringToInt(const std::string &source);
float													stringToFloat(const std::string &source);
int														strCaseDiff(const char * s1, const char * s2);
std::wstring										    utf8toUtf16(const std::string&);

const char *											extension(const char * str);

template<class T> inline std::string					
	toString(const T& input)
{
	std::ostringstream converter;
	converter << input;
	return converter.str();
}

template <typename T>
inline void swap(T & a, T & b)
{
	T temp(a);
	a = b;
	b = temp;
}

#endif
