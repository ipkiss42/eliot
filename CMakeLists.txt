cmake_minimum_required(VERSION 4.3.0)
project(eliot VERSION 2.3 LANGUAGES C CXX)
set(ELIOT_VERSION_STR "${PROJECT_VERSION}-git")

# Default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "RelWithDebInfo")

    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Use C++23 support (without extensions)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# --------------------------------------------------------------
# Options and build types
# --------------------------------------------------------------
option(ENABLE_DICTOOLS "build independent dictionary tools" ON)
option(ENABLE_TEXT "text interface support" ON)
set(ENABLE_NCURSES "AUTO" CACHE STRING "ncurses interface support (ON, OFF, or AUTO)")
option(ENABLE_QT6 "Qt6 interface support" ON)

# Configure default build behaviors for Debug/Profile
set(DEBUG_INFO_ENABLED $<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>)
add_compile_definitions($<${DEBUG_INFO_ENABLED}:DEBUG>)

# Profile build logic
option(ENABLE_PROFILE "profile mode" OFF)
if(ENABLE_PROFILE)
    add_compile_definitions(PROFILE)
    add_compile_options(-pg)
    add_link_options(-pg)
endif()

# --------------------------------------------------------------
# System checks & compiler flags
# --------------------------------------------------------------
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(TestBigEndian)

# Check compiler flags
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wall" COMPILER_HAS_WALL)
if(COMPILER_HAS_WALL)
    add_compile_options(-Wall)
endif()

check_cxx_compiler_flag("-Wextra" COMPILER_HAS_WEXTRA)
if(COMPILER_HAS_WEXTRA)
    add_compile_options(-Wextra)
endif()

# --------------------------------------------------------------
# Checks for headers and functions
# --------------------------------------------------------------
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)

# Functions
check_function_exists(wcwidth HAVE_WCWIDTH)

# --------------------------------------------------------------
# Checks for libraries & dependencies
# --------------------------------------------------------------
find_package(PkgConfig REQUIRED)

# Boost
find_package(Boost 1.36.0 REQUIRED)

# libconfig++
# Their cmake support is really bad in version 1.8.2,
# so keep using pkg-config for now.
pkg_check_modules(LIBCONFIG++ libconfig++ IMPORTED_TARGET)
if(LIBCONFIG++_FOUND)
    set(HAVE_LIBCONFIG 1)
else()
    set(HAVE_LIBCONFIG 0)
endif()

# pugixml
find_package(pugixml REQUIRED)

# spdlog
find_package(spdlog REQUIRED)

# Readline
pkg_check_modules(readline readline)
if(readline_FOUND)
    set(HAVE_READLINE TRUE)
else()
    set(HAVE_READLINE FALSE)
endif()

# Ncurses
set(BUILD_NCURSES FALSE)
if(NOT ENABLE_NCURSES STREQUAL "OFF")
    set(CURSES_NEED_NCURSES TRUE)
    set(CURSES_NEED_WIDE TRUE)

    find_package(Curses MODULE)

    # Check specifically for wide-character ncurses (ncursesw)
    if(CURSES_FOUND)
        set(BUILD_NCURSES TRUE)

        check_include_file("ncursesw/ncurses.h" HAVE_NCURSESW_NCURSES_H)
        check_include_file("ncursesw.h" HAVE_NCURSESW_H)
        check_include_file("ncurses.h" HAVE_NCURSES_H)
    else()
        # Enforce strict validation: If the user explicitly requested it via "ON", hard-fail
        if(ENABLE_NCURSES STREQUAL "ON")
            message(FATAL_ERROR "Could not find the wide-character ncursesw library on your system, but ENABLE_NCURSES=ON was explicitly requested.")
        else()
            # If it was left on "AUTO", log a quiet warning and skip building the target gracefully
            message(STATUS "Ncursesw not detected. Skipping ncurses console interface targets gracefully.")
        endif()
    endif()
endif()

set(BUILD_QT FALSE)
if(ENABLE_QT6)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network PrintSupport)
    set(BUILD_QT TRUE)

    get_target_property(Qt6_Core_EXE Qt6::qmake LOCATION)
    execute_process(
        COMMAND ${Qt6_Core_EXE} -query QT_INSTALL_TRANSLATIONS
        OUTPUT_VARIABLE QT6_TRANS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(QT_TRANSLATIONS_DIR "${QT6_TRANS_DIR}/")
endif()

# --------------------------------------------------------------
# Remaining build options & interface support
# --------------------------------------------------------------
option(ENABLE_TEXT "text interface support" ON)

# --------------------------------------------------------------
# Localization & Internationalization (gettext / iconv)
# --------------------------------------------------------------
find_package(Gettext REQUIRED)
# CMake handles external gettext via FindIntl
find_package(Intl REQUIRED)

find_package(Iconv REQUIRED)
if(NOT Iconv_FOUND)
    message(FATAL_ERROR "libiconv is needed for Eliot to work properly")
endif()
# FIXME: hack
set(ICONV_CONST "")

if(Gettext_FOUND AND Intl_FOUND)
    set(ENABLE_NLS 1)
    message(STATUS "Native Language Support (NLS) enabled.")
else()
    set(ENABLE_NLS 0)
    message(WARNING "Gettext or Intl libraries missing. Native Language Support (NLS) disabled.")
endif()

# --------------------------------------------------------------
# Environment & compilation metadata
# --------------------------------------------------------------
# Capture the building user
execute_process(
    COMMAND whoami
    OUTPUT_VARIABLE ELIOT_COMPILE_BY
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Capture the building host machine
execute_process(
    COMMAND hostname
    OUTPUT_VARIABLE ELIOT_COMPILE_HOST
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# --------------------------------------------------------------
# Operating System Specific Checks & Link Flags
# --------------------------------------------------------------
if(WIN32)
    # Link winsock2 library globally for platform networks (ntohl, etc.)
    # FIXME: hack!
    link_libraries(ws2_32)
endif()

# --------------------------------------------------------------
# Output architecture
# --------------------------------------------------------------

if(NOT HAVE_LIBCONFIG)
    message(WARNING
        "\n@@@@@@@@@@@\n"
        "Warning!\n"
        "libconfig++ was not detected on your system: saving and loading the configuration will be impossible!\n"
        "libconfig++ can be downloaded here: http://www.hyperrealm.com/main.php?s=libconfig\n"
        "@@@@@@@@@@@"
    )
endif()

# --------------------------------------------------------------
# Output architecture
# --------------------------------------------------------------
configure_file(config.h.cmake config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Process subdirectories
add_subdirectory(dic)
add_subdirectory(game)
add_subdirectory(utils)
add_subdirectory(qt)
add_subdirectory(po)
add_subdirectory(extras)

# Packaging commands (win32/macos)
include(Packaging.cmake)
