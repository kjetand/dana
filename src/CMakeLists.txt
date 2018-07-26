set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Werror")
endif()

set(SRC "${PROJECT_SOURCE_DIR}/src")
set(INC "${SRC}/include/dana")

set(SOURCE_FILES
  "${SRC}/canvas.cpp"
  "${SRC}/pencil.cpp"
  "${SRC}/image.cpp"
  "${SRC}/events.cpp"
)
set(HEADER_FILES
  "${INC}/canvas.h"
  "${INC}/pencil.h"
  "${INC}/util.h"
  "${INC}/types.h"
  "${INC}/image.h"
  "${INC}/events.h"
  "${SRC}/include/dana.h")

message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
message(STATUS "HEADER_FILES: ${HEADER_FILES}")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${DANA_LIBRARIES})
