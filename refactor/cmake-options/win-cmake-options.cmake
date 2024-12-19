# Define NOUSE_HAKO_MASTER macro
add_definitions(-DNOUSE_HAKO_MASTER)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wunknown-pragmas -Wmissing-prototypes -Wtrigraphs -Wimplicit-int")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fPIC")

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type settings
set(BUILD_TYPE "release")
if (debug)
    set(BUILD_TYPE "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

# Platform-specific settings
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows-specific compiler and linker options
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /DWIN32 /D_WINDOWS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /D_WINDOWS")
    add_compile_definitions(WIN32 TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS-specific options
    add_compile_definitions(MACOSX TRUE)
else()
    # Linux/Unix-specific options
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# GCOV settings for code coverage
set(GCOV "disabled")
if (gcov)
    set(GCOV "enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()
