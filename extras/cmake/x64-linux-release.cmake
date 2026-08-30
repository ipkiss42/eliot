# CMake triplet to disable debug builds (release only)
# This is essentially to half the build time.
include("${VCPKG_ROOT_DIR}/triplets/x64-linux.cmake")

# Block the debug build
set(VCPKG_BUILD_TYPE release)
