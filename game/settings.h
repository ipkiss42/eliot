/*****************************************************************************
 * Eliot
 * Copyright (C) 2007-2012 Olivier Teulière
 * Authors: Olivier Teulière <ipkiss @@ gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************/

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <filesystem>
#include <memory>
#include <string>

#include "logging.h"

namespace toml::inline v3 { class table; }


class Settings
{
    DEFINE_LOGGER();
public:
    /// Access to the singleton
    static Settings& Instance();
    /// Destroy the singleton cleanly
    static void Destroy();

    /// Return the config file directory path
    static std::filesystem::path GetConfigFileDir();

    ~Settings();

    /// Save the current value of the settings to a configuration file
    void save() const;

    void setBool(const std::string &iName, bool iValue);
    bool getBool(const std::string &iName) const;

    void setInt(const std::string &iName, int iValue);
    int getInt(const std::string &iName) const;

private:

    /// Singleton instance
    static Settings *m_instance;
    Settings();

    /// Name of the file used to store the settings
    std::filesystem::path m_fileName;

    // Structured in-memory configuration table managed by toml++
    std::unique_ptr<toml::table> m_conf;

    template<class T>
    requires std::same_as<T, int> || std::same_as<T, bool>
    void setValue(const std::string &iName, T iValue);
};

#endif

