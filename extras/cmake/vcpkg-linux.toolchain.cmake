# Locate the vcpkg internal toolchain
set(VCPKG_RESOLVED_ROOT "$ENV{VCPKG_ROOT}")
message(STATUS "vcpkg toolchain: Using explicit VCPKG_ROOT from environment: ${VCPKG_RESOLVED_ROOT}")
set(INTERNAL_TOOLCHAIN_FILE "${VCPKG_RESOLVED_ROOT}/scripts/buildsystems/vcpkg.cmake")
if(NOT EXISTS "${INTERNAL_TOOLCHAIN_FILE}")
    message(FATAL_ERROR "Could not locate the vcpkg CMake toolchain file at: ${INTERNAL_TOOLCHAIN_FILE}")
endif()

# Locate custom triplets
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/extras/cmake" CACHE PATH "" FORCE)

# Only turn on manifest mode if a local vcpkg.json file exists
# This prevents vcpkg from breaking during CMake's internal TryCompile checks.
if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg.json")
    set(VCPKG_MANIFEST_MODE ON CACHE BOOL "" FORCE)
else()
    set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "" FORCE)
endif()

# XXX: temporary. This saves time locally until I find the right vcpkg.json and toolchain setups
set(VCPKG_CLEAN_BUILDTREES OFF CACHE BOOL "" FORCE)

## Load the vcpkg toolchain
include("${INTERNAL_TOOLCHAIN_FILE}")

