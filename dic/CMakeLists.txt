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
# Define local library target
# --------------------------------------------------------------
add_library(dic STATIC
    logging.h logging.cpp
    base_exception.cpp base_exception.h
    dic_exception.cpp dic_exception.h
    header.cpp header.h
    dic_internals.h
    tile.cpp tile.h
    dic.cpp dic.h
    dic_search.cpp
    csv_helper.cpp csv_helper.h
    encoding.cpp encoding.h
    automaton.cpp automaton.h
    regexp.cpp regexp.h
    grammar.cpp grammar.h
    compdic.cpp compdic.h
    listdic.cpp listdic.h
    debug.cpp debug.h
)

# Set up target
target_include_directories(dic PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(dic
    PUBLIC
        spdlog::spdlog
    PRIVATE
        Iconv::Iconv
        Intl::Intl
)


# Stacktrace support via Boost.Stacktrace
target_link_libraries(dic PUBLIC ${CMAKE_DL_LIBS})
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # -rdynamic allows having real function names instead of hex codes
    target_link_options(dic INTERFACE -rdynamic)
endif()


# --------------------------------------------------------------
# Conditional tool compilation (if BUILD_DICTOOLS)
# --------------------------------------------------------------
if(ENABLE_DICTOOLS)

    # --- target: compdic ---
    add_executable(compdic compdicmain.cpp)
    target_link_libraries(compdic PRIVATE
        dic
        Intl::Intl
        Iconv::Iconv
        Boost::headers
    )

    # --- target: listdic ---
    add_executable(listdic listdicmain.cpp)
    target_link_libraries(listdic PRIVATE
        dic
        Intl::Intl
        Iconv::Iconv
    )
    target_include_directories(listdic PRIVATE ${CMAKE_SOURCE_DIR})

    # --- target: regexp ---
    add_executable(regexp regexpmain.cpp)
    # Uncomment if you want to restore the old debug flag:
    # target_compile_definitions(regexp PRIVATE DEBUG_RE)
    target_link_libraries(regexp PRIVATE
        dic
        Intl::Intl
        Iconv::Iconv
    )
    target_include_directories(regexp PRIVATE ${CMAKE_SOURCE_DIR})

    # --- Installation (bin_PROGRAMS equivalent) ---
    install(TARGETS compdic listdic regexp DESTINATION bin)

endif()

