cmake_minimum_required(VERSION 3.16)
project(prism VERSION 1.0.0 LANGUAGES C)

add_executable(prism
    "src/main.c"
    "src/logger.h"
    "src/logger.c"
    "src/bvh.h"
    "src/bvh.c"
    "src/memory_util.h"
    "src/memory_util.c"
    "src/math/vector.h"
    "src/math/vector.c"
    "src/math/bbox.h"
    "src/math/bbox.c"
 "src/geometry/triangle_mesh.h" "src/geometry/triangle_mesh.c")

set_target_properties(prism
    PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
)

target_include_directories(prism PRIVATE src)

# Enable pthreads:

# set(CMAKE_THREAD_PREFER_PTHREAD true)
# set(THREADS_PREFER_PTHREAD_FLAG true)
# find_package(Threads REQUIRED)

# target_link_libraries(prism PRIVATE Threads::Threads)

# Allows for greater optimization:

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
if(ipo_result)
    set_target_properties(prism PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "IPO is supported and enabled.")
else()
    message(WARNING "IPO is not supported: ${ipo_output}. This may lead to worse performance.")
endif()