# Eliot
# Copyright (C) 2026  Antoine Fraboulet & Olivier Teulière
# Authors: Antoine Fraboulet <antoine.fraboulet@free.fr>
#          Olivier Teulière <ipkiss @@ gmail.com>
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
# Target: eliottxt
# --------------------------------------------------------------
if(ENABLE_TEXT)
    set(ELIOTTXT_SOURCES game_io.cpp game_io.h eliottxt.cpp)
    add_executable(eliottxt ${ELIOTTXT_SOURCES})

    target_include_directories(eliottxt PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/dic
        ${CMAKE_SOURCE_DIR}/game
    )

    target_link_libraries(eliottxt PRIVATE game Intl::Intl Iconv::Iconv)

    if(HAVE_READLINE)
        target_link_libraries(eliottxt PRIVATE PkgConfig::readline)
    endif()
endif()


# --------------------------------------------------------------
# Target: eliotcurses
# --------------------------------------------------------------
if(BUILD_NCURSES)
    set(ELIOTCURSES_SOURCES curses_intf.cpp curses_intf.h)
    add_executable(eliotcurses ${ELIOTCURSES_SOURCES})

    target_include_directories(eliotcurses PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/dic
        ${CMAKE_SOURCE_DIR}/game
    )

    target_link_libraries(eliotcurses PRIVATE
        game
        Intl::Intl
        Iconv::Iconv
        Curses::Curses
    )

    # Force static linking on Windows
    if(WIN32)
        target_compile_definitions(eliotcurses PRIVATE NCURSES_STATIC=1)
    endif()

    # Installation configuration
    install(TARGETS eliotcurses DESTINATION bin)
endif()
