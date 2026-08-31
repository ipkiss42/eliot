include("${VCPKG_ROOT_DIR}/triplets/community/x64-mingw-static.cmake")

# Turn both library linkages and C/C++ runtimes into strict static
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE static)

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/llvm-mingw-compiler.cmake")

# Force Boost CMake to generate Win32 native files instead of POSIX/pthread components
# We may not need all of these
set(VCPKG_CMAKE_CONFIGURE_OPTIONS
    "-DBOOST_THREAD_THREADAPI=win32"
    "-DBOOST_USE_WINAPI_VERSION=0x0A00"
    "-DBOOST_STACKTRACE_ENABLE_NOOP=ON"
    "-DBOOST_STACKTRACE_ENABLE_WINDBG=ON"
    "-DBOOST_STACKTRACE_ENABLE_WINDBG_CACHED=ON"
    "-DBOOST_STACKTRACE_ENABLE_ADDR2LINE=OFF"
    "-DBOOST_STACKTRACE_ENABLE_BACKTRACE=OFF"
    "-DBOOST_STACKTRACE_ENABLE_GHS=OFF"
)

# Disable the release build
set(VCPKG_BUILD_TYPE release)
