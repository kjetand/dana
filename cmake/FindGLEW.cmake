set(GLEW_LOCATION "" CACHE PATH "Location GLEW directory")

find_library(GLEW_LIBRARY
  NAMES GLEW glew glew32
  PATHS
  ${GLEW_LOCATION}/lib
  "/usr/lib"
  "/usr/lib64"
  )

find_path(GLEW_INCLUDE_DIR "GL/glew.h"
  ${GLEW_LOCATION}/include
  "/usr/include"
  )

set(GLEW_FOUND "NO")
if(GLEW_LIBRARY)
  if(GLEW_INCLUDE_DIR)
    set(GLEW_FOUND "YES")
  endif(GLEW_INCLUDE_DIR)
endif(GLEW_LIBRARY)