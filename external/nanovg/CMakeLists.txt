project(nanovg)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Werror")
endif()

set(SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/nanovg.c"
  )
set(HEADER_FILES
  "${PROJECT_SOURCE_DIR}/nanovg.h"
  "${PROJECT_SOURCE_DIR}/nanovg_gl.h"
  "${PROJECT_SOURCE_DIR}/nanovg_gl_utils.h"
  "${PROJECT_SOURCE_DIR}/fontstash.h"
  "${PROJECT_SOURCE_DIR}/stb_image.h"
  "${PROJECT_SOURCE_DIR}/stb_truetype.h"
  )

message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
message(STATUS "HEADER_FILES: ${HEADER_FILES}")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME})