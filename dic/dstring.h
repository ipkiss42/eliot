/*****************************************************************************
 * Eliot
 * Copyright (C) 2026 Olivier Teulière
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

#ifndef DSTRING_H_
#define DSTRING_H_

#include <compare>
#include <string>
#include <utility>

/** Canonical display representation produced by dictionary/header/tile rendering. */
class DDString
{
public:
    DDString() = default;

    /// Adopt text already known to use the display representation; no validation.
    static DDString fromTrusted(std::wstring s)
    {
        return DDString(std::move(s));
    }

    bool empty() const noexcept { return m_data.empty(); }
    DDString &operator+=(const DDString &s) { m_data += s.m_data; return *this; }
    auto operator<=>(const DDString &) const = default;

    const std::wstring &raw() const & noexcept { return m_data; }
    std::wstring raw() && noexcept { return std::move(m_data); }
    std::wstring raw() const && { return m_data; }

    // STRING_MIGRATION: owning compatibility for unmigrated wide consumers.
    // Remove after display producers and consumers have all been migrated.
    operator std::wstring() const & { return m_data; }
    operator std::wstring() && noexcept { return std::move(m_data); }

private:
    std::wstring m_data;
    explicit DDString(std::wstring s) : m_data(std::move(s)) {}
};

inline DDString operator+(DDString lhs, const DDString &rhs)
{
    lhs += rhs;
    return lhs;
}

#endif
