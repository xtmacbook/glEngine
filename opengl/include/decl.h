
#if defined(_MSC_VER) && defined(ENGINE_USE_DLLS)
  #ifdef LIBENGINE_EXPORTS
    #define LIBENIGHT_EXPORT __declspec(dllexport)
	#define LIBENIGHT_EXTERN
  #else
    #define LIBENIGHT_EXPORT __declspec(dllimport)
	#define LIBENIGHT_EXTERN extern
  #endif
#else
  #define LIBENIGHT_EXPORT
#endif
