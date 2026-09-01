#pragma once

#define VERSION "@ELIOT_VERSION_STR@"
#define PACKAGE "@PROJECT_NAME@"
#define PACKAGE_VERSION "@ELIOT_VERSION_STR@"
#define PACKAGE_NAME "@PROJECT_NAME@"
#define PACKAGE_STRING "@PROJECT_NAME@ @ELIOT_VERSION_STR@"

#define ELIOT_COMPILE_BY "@ELIOT_COMPILE_BY@"
#define ELIOT_COMPILE_HOST "@ELIOT_COMPILE_HOST@"

#define LOCALEDIR "@CMAKE_INSTALL_FULL_LOCALEDIR@"
#define QT_TRANSLATIONS_DIR "@QT_TRANSLATIONS_DIR@"

#define ICONV_CONST @ICONV_CONST@

/* Define to 1 to enable Native Language Support (NLS) */
#cmakedefine01 ENABLE_NLS

#cmakedefine HAVE_READLINE 1

#cmakedefine HAVE_WCWIDTH 1

/* Ncurses wide-character header availability mapping */
#cmakedefine HAVE_NCURSESW_NCURSES_H 1
#cmakedefine HAVE_NCURSESW_H 1
#cmakedefine HAVE_NCURSES_H 1

// vim:ft=c:
