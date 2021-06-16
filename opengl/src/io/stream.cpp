
#include "stream.h"

namespace IO
{
	bool File::existDirectory(const char*dir)
	{
		DWORD dwAttrib = GetFileAttributes(dir);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	bool File::createDirectory(const char*dir)
	{
		if (!CreateDirectory(dir, NULL))
		{
			printf("CreateDirectory failed (%d)\n", GetLastError());
			return false;
		}
		return true;
	}

	bool File::existFile(const char*file)
	{
		return false;
	}
}


