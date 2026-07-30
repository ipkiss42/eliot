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

# --------------------------------------------------------------
# Gettext localization build rules
# --------------------------------------------------------------

# Ensure the native CMake Gettext engine module is active
include(FindGettext)

file(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.po")
set(COMPILED_MO_FILES "")

foreach(po_file ${PO_FILES})
    string(REGEX REPLACE "\\.po$" "" lang ${po_file})

    set(absolute_mo "${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo")

    add_custom_command(
        OUTPUT "${absolute_mo}"
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o "${absolute_mo}" "${CMAKE_CURRENT_SOURCE_DIR}/${po_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${po_file}"
        COMMENT "Compiling translation: ${po_file} -> ${lang}.mo"
    )
    list(APPEND COMPILED_MO_FILES "${absolute_mo}")

    # Drop eliot.mo into the language subdirectory layout
    install(
        FILES "${absolute_mo}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/locale/${lang}/LC_MESSAGES"
        RENAME "eliot.mo"
    )
endforeach()

add_custom_target(translations ALL DEPENDS ${COMPILED_MO_FILES})
