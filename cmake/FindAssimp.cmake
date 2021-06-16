
#/////////////////////////////////// assimp ///////////////////////////
IF (WIN32)
	FIND_PATH( ASSIMP_INCLUDE_PATH  
			 $ENV{PROGRAMFILES}/assimp/Importer.h
			${ASSIMP_ROOT_DIR}/include
		DOC "The directory where ASSIMP resides")

    FIND_LIBRARY( ASSIMP_LIBRARY
				NAMES assimp
        PATHS
				$ENV{PROGRAMFILES}/assimp/lib
        DOC "The ASSIMP library")
ELSE (WIN32)
	FIND_PATH( ASSIMP_INCLUDE_PATH /assimp/Importer.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		${ASSMIP_ROOT_DIR}/include/
		DOC "The directory where ASSIMP resides")
     
	# Prefer the static library.
	FIND_LIBRARY( ASSIMP_LIBRARY
		NAMES assimp
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${ASSMMP_ROOT_DIR}/lib/
		DOC "The ASSIMP library")
	 
ENDIF (WIN32)

SET(ASSIMP_FOUND "NO")
IF (ASSIMP_INCLUDE_PATH AND ASSIMP_LIBRARY)
	SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} )
	SET(ASSIMP_FOUND "YES")
ENDIF (ASSIMP_INCLUDE_PATH AND ASSIMP_LIBRARY)

