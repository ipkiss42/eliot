cmake_minimum_required(VERSION 4.3.0)
project(eliot VERSION 2.3 LANGUAGES C CXX)
set(ELIOT_VERSION_STR "${PROJECT_VERSION}-git")

# Force C++17 support without GNU extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS ON)

include(GNUInstallDirs)

# --------------------------------------------------------------
# Options and build types
# --------------------------------------------------------------
option(ENABLE_DICTOOLS "build independent dictionary tools" ON)
option(ENABLE_TEXT "text interface support" ON)
set(ENABLE_NCURSES "AUTO" CACHE STRING "ncurses interface support (ON, OFF, or AUTO)")
option(ENABLE_QT5 "Qt5 interface support" ON)

# Configure default build behaviors for Debug/Profile
# Note: In CMake, use CMAKE_BUILD_TYPE=Debug instead of manual -g flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DEBUG)
endif()

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
check_include_file("execinfo.h" HAVE_EXECINFO_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file("netinet/in.h" HAVE_NETINET_IN_H)

# Functions
check_function_exists(wcwidth HAVE_WCWIDTH)

# Endianness & Types
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    set(WORDS_BIGENDIAN 1)
endif()

# Readline
check_include_file("readline/readline.h" HAVE_READLINE_READLINE_H)
if(HAVE_READLINE_READLINE_H)
    set(HAS_READLINE TRUE)
else()
    set(HAS_READLINE FALSE)
endif()

# --------------------------------------------------------------
# Checks for libraries & dependencies
# --------------------------------------------------------------
find_package(PkgConfig REQUIRED)

# Boost
find_package(Boost 1.36.0 REQUIRED)

# libconfig++
pkg_check_modules(LIBCONFIG libconfig++)
if(LIBCONFIG_FOUND)
    set(HAVE_LIBCONFIG 1)
else()
    set(HAVE_LIBCONFIG 0)
endif()

# pugixml
find_package(pugixml REQUIRED)

# spdlog
find_package(spdlog REQUIRED)

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

# Qt4 vs Qt5 logic
set(BUILD_QT FALSE)
# if(ENABLE_QT4 AND ENABLE_QT5)
#     message(FATAL_ERROR "Please deactivate Qt4 explicitly if you want to build with Qt5")
# endif()

if(ENABLE_QT5)
    # Using modern native CMake targets instead of pkg-config for Qt5
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network PrintSupport)
    set(BUILD_QT TRUE)
    # FIXME: hack?
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # Safely query the active Qt5 system translations folder via qmake location properties
    get_target_property(Qt5_Core_EXE Qt5::qmake LOCATION)
    execute_process(
        COMMAND ${Qt5_Core_EXE} -query QT_INSTALL_TRANSLATIONS
        OUTPUT_VARIABLE QT5_TRANS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # FIXME: using a QT4 prefix is inelegant
    set(QT4LOCALEDIR "${QT5_TRANS_DIR}/")
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

