/*****************************************************************************
 * Eliot
 * Copyright (C) 2005-2007 Olivier Teulière
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

#include "config.h"

#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cerrno>
#include <iconv.h>
#include <optional>
#include <string>

#include "encoding.h"
#include "dic_exception.h"

using namespace std;


#if !HAVE_WCWIDTH
// wcwidth replacement (for win32 in particular)
// Inspired from the gnulib package, without some of the refinements
static inline int wcwidth(wchar_t c)
{
    // Assume all the printable characters have width 1
    return c == 0 ? 0 : (iswprint(c) ? 1 : -1);
}
#endif


int wtoi(const wchar_t *iWStr)
{
    return wcstol(iWStr, nullptr, 10);
}


#define _MAX_SIZE_FOR_STACK_ 30
wstring convertToWc(const string& iStr)
{
    // Get the needed length (we _can't_ use string::size())
    size_t len = mbstowcs(nullptr, iStr.c_str(), 0);
    if (len == (size_t)-1)
        return L"";

    // Change the allocation method depending on the length of the string
    if (len < _MAX_SIZE_FOR_STACK_)
    {
        // Without multi-thread, we can use static storage
        static wchar_t tmp[_MAX_SIZE_FOR_STACK_];
        len = mbstowcs(tmp, iStr.c_str(), len + 1);
        return tmp;
    }
    else
    {
        auto *tmp = new wchar_t[len + 1];
        len = mbstowcs(tmp, iStr.c_str(), len + 1);
        wstring res = tmp;
        delete[] tmp;
        return res;
    }
}


string convertToMb(const wstring& iWStr)
{
    // Get the needed length (we _can't_ use wstring::size())
    size_t len = wcstombs(nullptr, iWStr.c_str(), 0);
    if (len == (size_t)-1)
        return "";

    // Change the allocation method depending on the length of the string
    if (len < _MAX_SIZE_FOR_STACK_)
    {
        // Without multi-thread, we can use static storage
        static char tmp[_MAX_SIZE_FOR_STACK_];
        len = wcstombs(tmp, iWStr.c_str(), len + 1);
        return tmp;
    }
    else
    {
        char *tmp = new char[len + 1];
        len = wcstombs(tmp, iWStr.c_str(), len + 1);
        string res = tmp;
        delete[] tmp;
        return res;
    }
}
#undef _MAX_SIZE_FOR_STACK_


string convertToMb(wchar_t iWChar)
{
    return convertToMb(wstring(1, iWChar));
}


string truncString(const string &iStr, unsigned int iMaxWidth)
{
    // Heuristic: the width of a character cannot exceed the number of
    // bytes used to represent it (even in UTF-8)
    if (iStr.size() <= iMaxWidth)
        return iStr;
    return truncAndConvert(convertToWc(iStr), iMaxWidth);
}


static std::optional<std::pair<unsigned int, wstring>> getWidth(
    const std::wstring &iWstr,
    std::optional<unsigned int> iMaxWidth,
    std::string_view)
{
    unsigned int width = 0;
    for (unsigned int i = 0; i < iWstr.size(); ++i)
    {
        int n = wcwidth(iWstr[i]);
        if (n == -1)
        {
            // XXX: Should we throw an exception instead? Just ignore the problem?
#if 0
            std::string errorMsg = std::format("{}: non printable character: {}", iFuncName, iWstr[i]);
            std::println(stderr, "{}", errorMsg);
            throw DicException(errorMsg);
#endif
            return nullopt;
        }
        if (iMaxWidth.has_value() && width + n > iMaxWidth.value()) {
            return std::make_pair(width, iWstr.substr(0, i));
        }
        width += n;
    }
    return std::make_pair(width, iWstr);
}


string truncAndConvert(const wstring &iWstr, unsigned int iMaxWidth)
{
    auto widthandNewStr = getWidth(iWstr, iMaxWidth, "truncAndConvert");
    if (!widthandNewStr.has_value())
        return convertToMb(iWstr);

    auto &[_, newStr] = widthandNewStr.value();
    return convertToMb(newStr);
}


string truncOrPad(const string &iStr, unsigned int iMaxWidth, char iChar)
{
    wstring wstr = convertToWc(iStr);
    auto widthandNewStr = getWidth(wstr, iMaxWidth,"truncOrPad");
    if (!widthandNewStr.has_value())
        return convertToMb(wstr);

    auto &[width, newStr] = widthandNewStr.value();
    if (iMaxWidth > width)
        return convertToMb(newStr) + string(iMaxWidth - width, iChar);
    else
        return convertToMb(newStr);
}


string padAndConvert(const wstring &iWstr, unsigned int iLength,
                     bool iLeftPad, char c)
{
    const string &str = convertToMb(iWstr);
    auto widthandNewStr = getWidth(iWstr, nullopt,"padAndConvert");
    if (!widthandNewStr.has_value())
        return str;

    auto &[width, newStr] = widthandNewStr.value();
    if (width >= iLength)
        return str;
    else
    {
        // Padding is needed
        string s(iLength - width, c);
        if (iLeftPad)
            return s + str;
        else
            return str + s;
    }
}


string centerAndConvert(const wstring &iWstr, unsigned int iLength, char c)
{
    const string &str = convertToMb(iWstr);

    auto widthandNewStr = getWidth(iWstr, nullopt,"centerAndConvert");
    if (!widthandNewStr.has_value())
        return str;

    auto &[width, newStr] = widthandNewStr.value();
    if (width >= iLength)
        return str;
    else
    {
        // Padding is needed
        string s((iLength - width) / 2, c);
        string res = s + str + s;
        // If the string cannot be centered perfectly, pad again
        // (on the left if iLength is even, on the right otherwise:
        //  this tends to align numbers of 1 or 2 digits in a nice way)
        // Note: if needed, we could add the iLeftPad argument
        if ((iLength - width) % 2)
        {
            if (iLength % 2)
                res.append(1, c);
            else
                res.insert(res.begin(), c);
        }
        return res;
    }
}


wstring toUpper(std::wstring_view iWstr)
{
    std::wstring str(iWstr);
    std::ranges::transform(str, str.begin(), ::towupper);
    return str;
}


wstring toLower(std::wstring_view iWstr)
{
    std::wstring str(iWstr);
    std::ranges::transform(str, str.begin(), ::towlower);
    return str;
}


static unsigned int readFromUTF8(wchar_t *oString, unsigned int iWideSize,
                                 const char *iBuffer, unsigned int iBufSize,
                                 const string &iContext)
{
    iconv_t handle = iconv_open("WCHAR_T", "UTF-8");
    if (handle == (iconv_t)(-1))
        throw DicException("readFromUTF8: iconv_open failed");
    size_t inChars = iBufSize;
    size_t outChars = iWideSize * sizeof(wchar_t);
    // Use the ICONV_CONST trick because the declaration of iconv()
    // differs depending on the implementations...
    ICONV_CONST char *in = const_cast<ICONV_CONST char*>(iBuffer);
    char *out = (char*)oString;
    size_t res = iconv(handle, &in, &inChars, &out, &outChars);
    iconv_close(handle);
    // Problem during encoding conversion?
    if (res == (size_t)(-1))
    {
        throw DicException("readFromUTF8: iconv failed (" +
                           iContext + "): " + string(strerror(errno)));
    }
    return iWideSize - outChars / sizeof(wchar_t);
}


wstring readFromUTF8(const string &iString, const string &iContext)
{
    const int size = iString.size();
    // Temporary buffer for output
    // We will have at most as many characters as in the UTF-8 string
    auto *wideBuf = new wchar_t[size];
    unsigned int number;
    try
    {
        number = readFromUTF8(wideBuf, size, iString.data(), size, iContext);
    }
    catch (...)
    {
        // Make sure not to leak
        delete[] wideBuf;
        throw;
    }
    // Copy the string
    wstring res(wideBuf, number);
    delete[] wideBuf;
    return res;
}


unsigned int writeInUTF8(const wstring &iWString, char *oBuffer,
                         unsigned int iBufSize, const string &iContext)
{
    iconv_t handle = iconv_open("UTF-8", "WCHAR_T");
    if (handle == (iconv_t)(-1))
        throw DicException("writeInUTF8: iconv_open failed");
    size_t length = iWString.size();
    size_t inChars = sizeof(wchar_t) * length;
    size_t outChars = iBufSize;
    // Use the ICONV_CONST trick because the declaration of iconv()
    // differs depending on the implementations...
    // FIXME: bonus ugliness for doing 2 casts at once, and accessing string
    // internals...
    ICONV_CONST char *in = (ICONV_CONST char*)(&iWString[0]);
    char *out = oBuffer;
    size_t res = iconv(handle, &in, &inChars, &out, &outChars);
    iconv_close(handle);
    // Problem during encoding conversion?
    if (res == (size_t)(-1))
    {
        throw DicException("writeInUTF8: iconv failed (" +
                           iContext + "): " + string(strerror(errno)));
    }
    // Return the number of written bytes
    return iBufSize - outChars;
}


string writeInUTF8(const wstring &iWString, const string &iContext)
{
    // Temporary buffer for output
    // Each character will take at most 4 bytes in the UTF-8 string
    unsigned int bufSize = iWString.size() * 4;
    char *buf = new char[bufSize];
    unsigned int number;
    try
    {
        number = writeInUTF8(iWString, buf, bufSize, iContext);
    }
    catch (...)
    {
        // Make sure not to leak
        delete[] buf;
        throw;
    }
    // Copy the string
    string res(buf, number);
    delete[] buf;
    return res;
}

