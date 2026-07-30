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
# Asset installation (desktop entry & icons)
# --------------------------------------------------------------

install(FILES eliot.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES eliot.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)

# --------------------------------------------------------------
# Packaging / source tarball
# --------------------------------------------------------------
if(WIN32)
    configure_file(innosetup/eliot-setup.iss.in innosetup/eliot-setup.iss @ONLY)
endif()

