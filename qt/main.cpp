/*****************************************************************************
 * Eliot
 * Copyright (C) 2008-2012 Olivier Teulière
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

#include <iostream>
#include <clocale>
#include <string>
#include <exception>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#ifdef WIN32
#   include <windows.h>
#endif
#ifdef __APPLE__
#   include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef HAVE_EXECINFO_H
#   include <signal.h>
#   include <execinfo.h>
#endif

#include "logging.h"
#include "base_exception.h"
#include "stacktrace.h"
#include "main_window.h"

using namespace std;


#ifdef HAVE_EXECINFO_H
static void bt_sighandler(int);
#endif

// Custom QApplication to catch and log exceptions properly
// See http://forum.qtfr.org/viewtopic.php?id=7615
class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv)
        : QApplication(argc, argv)
    {}

    bool notify(QObject *receiver, QEvent *event) override
    {
        try
        {
            return QApplication::notify(receiver, event);
        }
        catch (const BaseException &e)
        {
            LOG_ROOT_ERROR("Exception caught: {}\n{}", e.what(), e.getStackTrace());
            return false;
        }
    }
};

int main(int argc, char **argv)
{
    initialize_logging();

#ifdef WIN32
    // If started from an active console terminal, re-attach our text output streams to it
    // This allows getting stdout messages in the terminal automatically.
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // Re-route standard input/output descriptors back to the command prompt window
        (void)freopen("CONOUT$", "w", stdout);
        (void)freopen("CONOUT$", "w", stderr);
        (void)freopen("CONIN$", "r", stdin);

        // Force C++ standard streams to stay in sync with the new descriptors
        std::ios_base::sync_with_stdio(true);
    }
#endif

#ifdef HAVE_EXECINFO_H
    // Install a custom signal handler to print a backtrace when crashing
    // See http://www.linuxjournal.com/article/6391 for inspiration
    signal(SIGSEGV, &bt_sighandler);
#endif

#if defined(ENABLE_NLS) && defined(__APPLE__)
    // On Mac, running Eliot from the dock does not automatically set the LANG
    // variable, so we do it ourselves.
    // Note: The following block of code is copied from VLC, and slightly
    // modified by me (original author: Pierre d'Herbemont)
    /* Check if $LANG is set. */
    if (NULL == getenv("LANG"))
    {
        // Retrieve the preferred language as chosen in System Preferences.app
        // (note that CFLocaleCopyCurrent() is not used because it returns the
        // preferred locale not language)
        CFArrayRef all_locales = CFLocaleCopyAvailableLocaleIdentifiers();
        CFArrayRef preferred_locales = CFBundleCopyLocalizationsForPreferences(all_locales, NULL);

        if (preferred_locales)
        {
            if (CFArrayGetCount(preferred_locales))
            {
                char psz_locale[50];
                CFStringRef user_language_string_ref = (CFStringRef) CFArrayGetValueAtIndex(preferred_locales, 0);
                CFStringGetCString(user_language_string_ref, psz_locale, sizeof(psz_locale), kCFStringEncodingUTF8);
                setenv("LANG", psz_locale, 1);
            }
            CFRelease(preferred_locales);
        }
        CFRelease(all_locales);
    }
#endif

#ifdef ENABLE_NLS
    // Set locale via LC_ALL
    setlocale(LC_ALL, "");
#ifdef __APPLE__
    // FIXME: Ugly hack: we hardcode the encoding to UTF-8, because I don't
    // know how to retrieve it properly when LANG is not set
    setlocale(LC_CTYPE, "UTF-8");
#endif
#endif

    MyApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/eliot.xpm"));
    // Used for QSettings
    app.setApplicationName(PACKAGE_NAME);
    app.setOrganizationName("eliot");

#ifdef ENABLE_NLS
    // Set the message domain
#ifdef WIN32
    QString appDir = QCoreApplication::applicationDirPath();
    const string localeDir = appDir.toStdString() + "\\locale";
#elif defined(__APPLE__)
    const char *bundlePath = CFStringGetCStringPtr(CFURLCopyFileSystemPath(
            CFBundleCopyBundleURL(CFBundleGetMainBundle()), kCFURLPOSIXPathStyle), CFStringGetSystemEncoding());
    const string localeDir = string(bundlePath) + "/Contents/Resources/locale";
#else
    static const string localeDir = LOCALEDIR;
#endif
    bindtextdomain(PACKAGE, localeDir.c_str());
    // Force messages to UTF-8
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);

    // Translations for Qt's own strings
    QTranslator translator;
    // Set the path for the translation file
#ifdef WIN32
    QString path = QString::fromLocal8Bit(localeDir + "\\qt");
#else
    QString path = QString(QT_TRANSLATIONS_DIR);
#endif

    QString lang = QLocale::system().name();
    translator.load(path + "/qt_" + lang);
    app.installTranslator(&translator);
#endif

    try
    {
        MainWindow qmain;
        qmain.show();
        return app.exec();
    }
    catch (const BaseException &e)
    {
        cerr << "Exception caught: " << e.what() << "\n" << e.getStackTrace();
    }
    catch (const std::exception &e)
    {
        cerr << "Exception caught: " << e.what();
    }
    catch (...)
    {
        cerr << "Unknown exception caught";
    }
    return 1;
}

#ifdef HAVE_EXECINFO_H
static void bt_sighandler(int signum)
{
    LOG_ROOT_FATAL("Segmentation fault!");
    LOG_ROOT_FATAL("Backtrace:\n{}", StackTrace::GetStack());

    // Restore the default handler to generate a nice core dump
    signal(signum, SIG_DFL);
    raise(signum);
}
#endif

