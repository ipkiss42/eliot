set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Define the cross-compilers
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

set(PKG_CONFIG_EXECUTABLE "x86_64-w64-mingw32-pkg-config" CACHE FILEPATH "pkg-config executable")

# Force find_package to search the target Windows folders, not host Linux folders
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Force the C++ version (-std=gnu++17)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ Standard" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "Require C++ Standard" FORCE)
set(CMAKE_CXX_EXTENSIONS ON CACHE STRING "Allow compiler extensions (gnu++17)" FORCE)
