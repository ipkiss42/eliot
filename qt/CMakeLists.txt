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

if(BUILD_QT)

    # --------------------------------------------------------------
    # Sources
    # --------------------------------------------------------------
    set(ELIOT_QT_SOURCES
        qtcommon.cpp qtcommon.h
        game_signals.cpp game_signals.h
        play_model.cpp play_model.h
        tile_widget.cpp tile_widget.h
        rack_widget.cpp rack_widget.h
        timer_widget.cpp timer_widget.h
        tile_layout.cpp tile_layout.h
        validator_factory.cpp validator_factory.h
        misc_helpers.cpp misc_helpers.h
        custom_popup.cpp custom_popup.h
        arbitration_widget.cpp arbitration_widget.h
        arbit_assignments.cpp arbit_assignments.h
        bag_widget.cpp bag_widget.h
        dic_tools_widget.cpp dic_tools_widget.h
        players_table_helper.cpp players_table_helper.h
        fav_players.cpp fav_players.h
        hints_dialog.cpp hints_dialog.h
        new_game.cpp new_game.h
        score_widget.cpp score_widget.h
        dic_wizard.cpp dic_wizard.h
        board_widget.cpp board_widget.h
        history_widget.cpp history_widget.h
        stats_widget.cpp stats_widget.h
        play_word_mediator.cpp play_word_mediator.h
        tables_dialog.cpp tables_dialog.h
        topping_widget.cpp topping_widget.h
        training_widget.cpp training_widget.h
        player_widget.cpp player_widget.h
        prefs_dialog.cpp prefs_dialog.h
        update_checker.cpp update_checker.h
        aux_window.cpp aux_window.h
        main_window.cpp main_window.h
        main.cpp
    )

    set(UI_FILES
        ui/dic_tools_widget.ui
        ui/dic_wizard_info_page.ui
        ui/dic_wizard_letters_def_page.ui
        ui/dic_wizard_conclusion_page.ui
        ui/arbitration_widget.ui
        ui/arbit_assignments.ui
        ui/bag_widget.ui
        ui/main_window.ui
        ui/new_game.ui
        ui/player_widget.ui
        ui/prefs_dialog.ui
        ui/tables_dialog.ui
        ui/topping_widget.ui
        ui/training_widget.ui
    )

    # --------------------------------------------------------------
    # Native Qt build automations
    # --------------------------------------------------------------
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    # Disable the automated tracker since we are writing our own explicit rules
    # TODO: get rid of this (and the foreach()) by adapting all includes to the
    # default generated name (and making sure that gettext can still detect the
    # generated files)
    set(CMAKE_AUTOUIC OFF)
    # Force the UI compiler to wrap layout strings in Q_("Context", "Text")
#     set(CMAKE_AUTOUIC_OPTIONS "-tr" "Q_")


    set(GENERATED_UI_HEADERS "")

    set(UIC_EXECUTABLE Qt5::uic)

    # Generate a hardcoded header block at configure time
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ui_prefix.txt" "// Generated natively by Eliot\n#include \"qtcommon.h\"\n")

    foreach(ui_file ${UI_FILES})
        get_filename_component(ui_name ${ui_file} NAME_WE)

        # Match what the source code expects: <ui/filename.ui.h>
        set(output_header "${CMAKE_CURRENT_BINARY_DIR}/ui/${ui_name}.ui.h")

        add_custom_command(
            OUTPUT ${output_header}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/ui"

            # Run uic to compile the layout into a temporary raw scratch file
            COMMAND ${UIC_EXECUTABLE} -tr "Q_" "${CMAKE_CURRENT_SOURCE_DIR}/${ui_file}" -o "${output_header}.tmp"

            # Initialize the real target header file with the custom prefix
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/ui_prefix.txt" "${output_header}"

            # Append the raw uic code
            COMMAND ${CMAKE_COMMAND} -E cat "${output_header}.tmp" >> "${output_header}"

            # Delete the temporary file
            COMMAND ${CMAKE_COMMAND} -E remove "${output_header}.tmp"

            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ui_file}"
            COMMENT "Generating localized UI layout header: ui/${ui_name}.ui.h"
        )
        list(APPEND GENERATED_UI_HEADERS ${output_header})
    endforeach()

    # --------------------------------------------------------------
    # Windows application icon integration
    # --------------------------------------------------------------
    if(WIN32)
        set(WIN32_RC "${CMAKE_CURRENT_BINARY_DIR}/win32icon.rc")
        file(WRITE ${WIN32_RC} "IDI_ICON1 ICON DISCARDABLE \"${CMAKE_CURRENT_SOURCE_DIR}/images/eliot.ico\"\n")
        list(APPEND ELIOT_QT_SOURCES ${WIN32_RC})
    endif()

    # --------------------------------------------------------------
    # Target setup & compile options
    # --------------------------------------------------------------
    add_executable(eliot ${ELIOT_QT_SOURCES} ${GENERATED_UI_HEADERS} eliot.qrc)

    target_include_directories(eliot PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/dic
        ${CMAKE_SOURCE_DIR}/game
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(eliot PRIVATE
        game
        dic
        Intl::Intl
        Iconv::Iconv
    )

    target_link_libraries(eliot PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::PrintSupport)

    if(HAVE_LIBCONFIG)
        target_link_libraries(eliot PRIVATE ${LIBCONFIG_LIBRARIES})
    endif()

    set_target_properties(eliot PROPERTIES ENABLE_EXPORTS TRUE)
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        target_link_options(eliot PRIVATE "-rdynamic")
    endif()

    install(TARGETS eliot DESTINATION bin)

endif()
