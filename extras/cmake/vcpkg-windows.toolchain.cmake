include("${CMAKE_CURRENT_LIST_DIR}/llvm-mingw-compiler.cmake")

# Point to the triplet defined in x64-linux-release.cmake
set(VCPKG_TARGET_TRIPLET x64-mingw-static-release CACHE STRING "" FORCE)
set(VCPKG_HOST_TRIPLET x64-linux-release CACHE STRING "" FORCE)
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/extras/cmake" CACHE PATH "" FORCE)

if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg.json")
    set(VCPKG_MANIFEST_MODE ON CACHE BOOL "" FORCE)
else()
    set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "" FORCE)
endif()

# Load the main vcpkg build systems script from your VCPKG_ROOT
if(NOT DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_RESOLVED_ROOT "$ENV{HOME}/vcpkg")
else()
    set(VCPKG_RESOLVED_ROOT "$ENV{VCPKG_ROOT}")
endif()

include("${VCPKG_RESOLVED_ROOT}/scripts/buildsystems/vcpkg.cmake")
