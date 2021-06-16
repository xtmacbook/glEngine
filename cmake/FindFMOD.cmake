
#/////////////////////////////////// fmod ///////////////////////////
IF (WIN32)
	FIND_PATH( FMOD_INCLUDE_PATH fmod/fmod.h
			 $ENV{PROGRAMFILES}/fmod/include
			${FMOD_ROOT_DIR}/include
		DOC "The directory where fmod resides")

    FIND_LIBRARY( FMOD_LIBRARY
				NAMES fmod
        PATHS
				$ENV{PROGRAMFILES}/fmod/lib
        DOC "The FMOD library")
ELSE (WIN32)
	FIND_PATH( FMOD_INCLUDE_PATH fmod.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		${FMOD_ROOT_DIR}/api/lowlevel/inc
		DOC "The directory where FMOD resides")
    FIND_PATH (FMOD_STUDIO_PATH  
            fmod_studio.h
            ${FMOD_ROOT_DIR}/api/studio/inc
            DOC "The directory where FMOD inc"
            )
	# Prefer the static library.
	FIND_LIBRARY( FMOD_LIBRARY
		NAMES fmod.a fmod
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${FMOD_ROOT_DIR}/api/lowlevel/lib
		DOC "The FMOD library")
	FIND_PATH(FMOD_STUDIO_LIBRARY  
	    libfmodstudio.dylib
	    ${FMOD_ROOT_DIR}/api/studio/lib
	    lib/
	    DOC "The FMOD studio library"
	    )	
ENDIF (WIN32)

SET(FMOD_FOUND "NO")
IF (FMOD_INCLUDE_PATH AND FMOD_LIBRARY)
	SET(FMOD_LIBRARIES ${FMOD_LIBRARY} ${FMOD_STUDIO_LIBRARY})
	SET(FMOD_FOUND "YES")
ENDIF (FMOD_INCLUDE_PATH AND FMOD_LIBRARY)

