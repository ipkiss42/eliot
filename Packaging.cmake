# Eliot
# Copyright (C) 2026  Olivier Teulière
# Authors: Olivier Teulière <ipkiss @@ gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

set(WIN32_PACKAGE_DIR    "${CMAKE_BINARY_DIR}/eliot-${ELIOT_VERSION_STR}")
set(WIN32_ZIP_FILE       "${CMAKE_BINARY_DIR}/eliot-${ELIOT_VERSION_STR}-win32.zip")
set(WIN32_EXE_FILE       "${CMAKE_BINARY_DIR}/eliot-${ELIOT_VERSION_STR}-win32.exe")
set(WIN32_INSTALLER_DIR  "${CMAKE_BINARY_DIR}/eliot-installer-${ELIOT_VERSION_STR}")
set(LANGUAGES            fr ca cs es id it sr gl en en@quot en@boldquot)

# --- DYNAMIC TOOLCHAIN PATH RESOLUTION ---
# Given CMAKE_CXX_COMPILER like /opt/llvm-mingw/llvm-mingw-ucrt/bin/x86_64-w64-mingw32-clang++,
# infer two variables (assuming a clang-like layout
#   * LLVM_MINGW_BASE=/opt/llvm-mingw/llvm-mingw-ucrt
#   * MINGW_TARGET_TRIPLE=x86_64-w64-mingw32
get_filename_component(TOOLCHAIN_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
get_filename_component(LLVM_MINGW_BASE "${TOOLCHAIN_BIN_DIR}" DIRECTORY)

get_filename_component(COMPILER_FILENAME "${CMAKE_CXX_COMPILER}" NAME)
string(REGEX MATCH "^[a-zA-Z0-9_]+-w64-mingw32" MINGW_TARGET_TRIPLE "${COMPILER_FILENAME}")

set(LLVM_MINGW_TARGET_BIN "${LLVM_MINGW_BASE}/${MINGW_TARGET_TRIPLE}/bin")
message(STATUS "Resolved Runtime Folder: ${LLVM_MINGW_TARGET_BIN}")

# ------------------------------------------------------------------------------
# Target: package-win32-dir
# ------------------------------------------------------------------------------
add_custom_target(package-win32-dir
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${WIN32_PACKAGE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WIN32_PACKAGE_DIR}"

    # Copy binary outputs
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:compdic>" "${WIN32_PACKAGE_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:listdic>" "${WIN32_PACKAGE_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:eliot>"   "${WIN32_PACKAGE_DIR}/"

    # Strip debugging symbols
    COMMAND ${CMAKE_STRIP} "${WIN32_PACKAGE_DIR}/compdic${CMAKE_EXECUTABLE_SUFFIX}"
    COMMAND ${CMAKE_STRIP} "${WIN32_PACKAGE_DIR}/listdic${CMAKE_EXECUTABLE_SUFFIX}"
    COMMAND ${CMAKE_STRIP} "${WIN32_PACKAGE_DIR}/eliot${CMAKE_EXECUTABLE_SUFFIX}"

    COMMENT "Assembling Windows binary layout directory tree..."
)

# Populate translation directories in parallel
foreach(lang ${LANGUAGES})
    add_custom_command(TARGET package-win32-dir POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${WIN32_PACKAGE_DIR}/locale/${lang}/LC_MESSAGES"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/po/${lang}.mo" "${WIN32_PACKAGE_DIR}/locale/${lang}/LC_MESSAGES/eliot.mo"
    )
endforeach()

# Process asset copies and cross-compiler runtime dependencies dynamically
add_custom_command(TARGET package-win32-dir POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WIN32_PACKAGE_DIR}/locale/qt"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${QT4LOCALEDIR}" "${WIN32_PACKAGE_DIR}/locale/qt/"

    # Copy text documentation files
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS"
        "${CMAKE_CURRENT_SOURCE_DIR}/COPYING"
        "${CMAKE_CURRENT_SOURCE_DIR}/NEWS"
        "${CMAKE_CURRENT_SOURCE_DIR}/THANKS"
        "${WIN32_PACKAGE_DIR}/"

    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/extras/reports" "${WIN32_PACKAGE_DIR}/reports"

    # Copy compiler DLLs (assuming clang)
    COMMAND ${CMAKE_COMMAND} -E echo "Extracting runtime dependencies from: ${MINGW_BIN_DIR}"
    COMMAND /bin/sh -c "cp ${LLVM_MINGW_TARGET_BIN}/libunwind.dll ${WIN32_PACKAGE_DIR}/"
    COMMAND /bin/sh -c "cp ${LLVM_MINGW_TARGET_BIN}/libc++.dll ${WIN32_PACKAGE_DIR}/"
)

# ------------------------------------------------------------------------------
# Target: package-win32-zip
# ------------------------------------------------------------------------------
find_program(ZIP_EXECUTABLE NAMES zip)
if(ZIP_EXECUTABLE)
    add_custom_target(package-win32-zip
        DEPENDS package-win32-dir
        COMMAND ${CMAKE_COMMAND} -E rm -f "${WIN32_ZIP_FILE}"
        COMMAND ${ZIP_EXECUTABLE} -r -9 "${WIN32_ZIP_FILE}" "eliot-${ELIOT_VERSION_STR}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${WIN32_PACKAGE_DIR}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Windows ZIP archive generated at: ${WIN32_ZIP_FILE}"
    )
endif()

# ------------------------------------------------------------------------------
# Target: package-win32-exe (InnoSetup Installer Layer)
# ------------------------------------------------------------------------------
# Prepare the win32 package (exe version)
# This target supposes that a 'iscc' script is available:
# see https://katastrophos.net/andre/blog/2009/03/16/setting-up-the-inno-setup-compiler-on-debian/
# It also supposes the ELIOT_DIC_DIR environment variable points to a folder
# containing the dictionaries, with the correct structure.
find_program(ISCC_EXECUTABLE NAMES iscc)
if(ISCC_EXECUTABLE)
    add_custom_target(package-win32-exe
        DEPENDS package-win32-dir

        # Make sure that the environment variable is present,
        # and check quickly the directory
        COMMAND ${CMAKE_COMMAND} -E echo "Validating dictionary path constraints..."
        COMMAND test -d "$ENV{ELIOT_DIC_DIR}" || (echo "Error: ELIOT_DIC_DIR environment variable is missing!" && exit 1)
        COMMAND test -d "$ENV{ELIOT_DIC_DIR}/english" || (echo "Error: Invalid dictionary layout structure!" && exit 1)

        # Prepare a temporary directory for InnoSetup
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${WIN32_INSTALLER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${WIN32_INSTALLER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E rename "${WIN32_PACKAGE_DIR}" "${WIN32_INSTALLER_DIR}/eliot"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/extras/innosetup" "${WIN32_INSTALLER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/extras/innosetup" "${WIN32_INSTALLER_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{ELIOT_DIC_DIR}" "${WIN32_INSTALLER_DIR}/dictionaries"

        # Run the compiler
        COMMAND ${ISCC_EXECUTABLE} "${WIN32_INSTALLER_DIR}/eliot-setup.iss"
        COMMAND ${CMAKE_COMMAND} -E copy "${WIN32_INSTALLER_DIR}/Output/setup.exe" "${WIN32_EXE_FILE}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${WIN32_INSTALLER_DIR}"

        COMMENT "Windows installer setup executable generated at: ${WIN32_EXE_FILE}"
    )
endif()


# WARNING: completely untested after the migration from Autotools (Makefile.am) to CMake.
if(APPLE)
    # Define variables
    set(MACOSX_PACKAGE_DIR "${CMAKE_BINARY_DIR}/eliot-${PROJECT_VERSION}/Eliot.app")
    set(DMG_FILE "${CMAKE_BINARY_DIR}/eliot-${PROJECT_VERSION}-macos.dmg")

    # -------------------------------------------------------------
    # Target: package-macosx
    # -------------------------------------------------------------
    add_custom_target(package-macosx
        # Clean previous staging setup
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/eliot-${PROJECT_VERSION}"

        # Re-create base bundle directory architecture
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MACOSX_PACKAGE_DIR}/Contents/MacOS"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MACOSX_PACKAGE_DIR}/Contents/Resources"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MACOSX_PACKAGE_DIR}/Contents/Frameworks"

        # Copy plist and core icon files
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/extras/macosx/Info.plist" "${MACOSX_PACKAGE_DIR}/Contents/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/extras/macosx/eliot-64.icns" "${MACOSX_PACKAGE_DIR}/Contents/Resources/"

        # Copy the compiled application binary and rename it to match the bundle
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:eliot>" "${MACOSX_PACKAGE_DIR}/Contents/MacOS/Eliot"
        COMMAND strip "${MACOSX_PACKAGE_DIR}/Contents/MacOS/Eliot"

        COMMENT "Assembling initial App Bundle layout structure..."
    )

    # Handle translations
    foreach(lang IN LISTS LANGUAGES)
        add_custom_command(TARGET package-macosx POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${MACOSX_PACKAGE_DIR}/locale/${lang}/LC_MESSAGES"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/po/${lang}.gmo" "${MACOSX_PACKAGE_DIR}/locale/${lang}/LC_MESSAGES/eliot.mo"
        )
    endforeach()

    # Copy Qt localized frameworks and support templates (qt_menu.nib)
    add_custom_command(TARGET package-macosx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MACOSX_PACKAGE_DIR}/locale/qt"
        # The true fallback allows copying if the globs match, ignoring if they are missing
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT4LOCALEDIR}/qt_*.qm" "${MACOSX_PACKAGE_DIR}/locale/qt/" OR_IF_MISSING
        COMMAND ${CMAKE_COMMAND} -E copy_directory "/opt/local/share/qt/resources/qt_menu.nib" "${MACOSX_PACKAGE_DIR}/Contents/Resources/qt_menu.nib"
    )

    # Replace otool loop and install_name_tool loop
    #
    # We dynamically generate a script at build time that leverages CMake's BundleUtilities.
    # It analyzes the Eliot binary, recursively, gathers all MacPorts/Homebrew libs,
    # copies them into Contents/Frameworks, and fixes up the @executable_path linkages safely.
    add_custom_command(TARGET package-macosx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E write_regscript "${CMAKE_CURRENT_BINARY_DIR}/fixup.cmake"
            "include(BundleUtilities)\n"
            "fixup_bundle(\"${MACOSX_PACKAGE_DIR}\" \"\" \"/opt/local/lib\")\n"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/fixup.cmake"
        COMMENT "Resolving shared library dependencies and rewriting install names automatically..."
    )

    # -------------------------------------------------------------
    # Target: package-macosx-dmg
    # -------------------------------------------------------------
    add_custom_target(package-macosx-dmg
        DEPENDS package-macosx
        COMMAND ${CMAKE_COMMAND} -E rm -f "${DMG_FILE}"
        COMMAND hdiutil create "${DMG_FILE}" -verbose -scrub -srcfolder "${CMAKE_BINARY_DIR}/eliot-${PROJECT_VERSION}"
        COMMENT "MacOS disk image generated at: ${DMG_FILE}"
    )

endif()
