APR_VERSION := 1.6.5
APU_VERSION := 1.6.1
ARABICA_VERSION := 2016-January
BOOST_VERSION := 1.68.0
EXPAT_VERSION := 2.2.6
ICONV_VERSION := 1.15
LIBCONFIG_VERSION := 1.7.2
LIBXML2_VERSION := 2.9.8
LOG4CXX_VERSION := 0.10.0
QT_VERSION := 4.5.3
QT2_VERSION := 5.6.3
XERCES_VERSION := 3.2.2

HOST := x86_64-w64-mingw32
BUILD := i386-linux


BASE = $(shell pwd)
PREFIX = $(BASE)/inst
PREFIX_FLAG = $(PREFIX)/.flag
ARCHIVE_DIR = $(BASE)/archives
BUILD_DIR = $(BASE)/build
WGET = wget -c -P $(ARCHIVE_DIR)
TAR = tar -C $(BUILD_DIR)

# XXX: Hardcoded for mingw on linux, at the moment
CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
CPPFLAGS += -I$(PREFIX)/include
CONFIGURE = CC="$(CC)" CXX="$(CXX)" CPPFLAGS="$(CPPFLAGS)" ./configure --host=$(HOST) --build=$(BUILD) --prefix=$(PREFIX)

# Dependencies (incomplete list):
# expat <- apr <- apu <- log4cxx
LIBS := iconv boost expat apr apu libconfig log4cxx arabica qt2
TARGETS = $(LIBS:%=.%)

.PHONY: help all clean clean-% distclean distclean-%

all: $(TARGETS)

# Remove the extracted directory and the marker flag
clean-%:
	rm -rf $($(shell echo $* | tr a-z A-Z)_DIR) .$*

# Also remove the archive
distclean-%: clean-%
	rm -rf $($(shell echo $* | tr a-z A-Z)_ARCHIVE)

clean: $(LIBS:%=clean-%)

distclean:
	rm -rf $(PREFIX)
	rm -rf $(ARCHIVE_DIR)
	rm -rf $(BUILD_DIR)

help:
	@echo "Usage: make all"

$(PREFIX_FLAG):
	mkdir -p $(PREFIX) $(ARCHIVE_DIR) $(BUILD_DIR)
	touch $@


### APR (needed by log4cxx) ###

APR_DIR = $(BUILD_DIR)/apr-$(APR_VERSION)
APR_DIR_FLAG = $(APR_DIR)/.flag
APR_ARCHIVE = $(ARCHIVE_DIR)/apr-$(APR_VERSION).tar.bz2

$(APR_ARCHIVE): $(PREFIX_FLAG)
#	$(WGET) http://www-eu.apache.org/dist/apr/$(shell basename $@)
	$(WGET) http://archive.apache.org/dist/apr/$(shell basename $@)

$(APR_DIR_FLAG): $(APR_ARCHIVE)
	$(TAR) -xjf $<
	touch $@

.apr: $(APR_DIR_FLAG)
	(cd $(APR_DIR) && patch -p0 < $(BASE)/apr-bugzilla-56342.patch && patch -p0 < $(BASE)/apr.patch && $(CONFIGURE) --enable-static --disable-shared && make && make install)
	touch $@


### APU (needed by log4cxx) ###

APU_DIR = $(BUILD_DIR)/apr-util-$(APU_VERSION)
APU_DIR_FLAG = $(APU_DIR)/.flag
APU_ARCHIVE = $(ARCHIVE_DIR)/apr-util-$(APU_VERSION).tar.bz2

$(APU_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) http://www-eu.apache.org/dist/apr/$(shell basename $@)

$(APU_DIR_FLAG): $(APU_ARCHIVE)
	$(TAR) -xjf $<
	touch $@

.apu: $(APU_DIR_FLAG)
	(cd $(APU_DIR) && $(CONFIGURE) --with-apr=$(PREFIX) --enable-static --disable-shared && make && make install)
	touch $@


### Arabica ###

ARABICA_DIR = $(BUILD_DIR)/arabica-$(ARABICA_VERSION)
ARABICA_DIR_FLAG = $(ARABICA_DIR)/.flag
ARABICA_ARCHIVE = $(ARCHIVE_DIR)/$(ARABICA_VERSION).tar.gz

$(ARABICA_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/jezhiggins/arabica/archive/$(shell basename $@)

$(ARABICA_DIR_FLAG): $(ARABICA_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.arabica: $(ARABICA_DIR_FLAG)
	(cd $(ARABICA_DIR) && autoreconf -i && $(CONFIGURE) --enable-static --disable-shared --with-parser=expat --with-expat=$(PREFIX) --with-boost=$(PREFIX) --with-tests=no --with-dom=no && make install)
	touch $@


### Boost ###

BOOST_UNDERSCORE_VERSION = $(subst .,_,$(BOOST_VERSION))
BOOST_DIR = $(BUILD_DIR)/boost_$(BOOST_UNDERSCORE_VERSION)
BOOST_DIR_FLAG = $(BOOST_DIR)/.flag
BOOST_ARCHIVE = $(ARCHIVE_DIR)/boost_$(BOOST_UNDERSCORE_VERSION).tar.bz2

$(BOOST_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://dl.bintray.com/boostorg/release/$(BOOST_VERSION)/source/$(shell basename $@)

$(BOOST_DIR_FLAG): $(BOOST_ARCHIVE)
	$(TAR) -xjf $<
	touch $@

# We don't build any library, because we don't need them (and it is really
# hard to cross-compile with their crappy build system)
.boost: $(BOOST_DIR_FLAG)
	#(cd $(BOOST_DIR) && ./configure --prefix=$(PREFIX) && ./tools/jam/src/bin.linuxx86/bjam --toolset=gcc --prefix=$(PREFIX) --without-date_time --without-filesystem --without-graph --without-iostreams --without-program_options --without-python --without-regex --without-serialization --without-signals --without-test --without-thread --without-wave install)
	cp -r $(BOOST_DIR)/boost $(PREFIX)/include
	touch $@


### Expat ###

EXPAT_UNDERSCORE_VERSION = $(subst .,_,$(EXPAT_VERSION))
EXPAT_DIR = $(BUILD_DIR)/libexpat-R_$(EXPAT_UNDERSCORE_VERSION)
EXPAT_DIR_FLAG = $(EXPAT_DIR)/.flag
EXPAT_ARCHIVE = $(ARCHIVE_DIR)/R_$(EXPAT_UNDERSCORE_VERSION).tar.gz

$(EXPAT_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/libexpat/libexpat/archive/$(shell basename $@)

$(EXPAT_DIR_FLAG): $(EXPAT_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.expat: $(EXPAT_DIR_FLAG)
	(cd $(EXPAT_DIR)/expat && ./buildconf.sh && $(CONFIGURE) --without-xmlwf --enable-static --disable-shared && make install)
	touch $@


### iconv ###

ICONV_DIR = $(BUILD_DIR)/libiconv-$(ICONV_VERSION)
ICONV_DIR_FLAG = $(ICONV_DIR)/.flag
ICONV_ARCHIVE = $(ARCHIVE_DIR)/libiconv-$(ICONV_VERSION).tar.gz

$(ICONV_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://ftp.gnu.org/pub/gnu/libiconv/$(shell basename $@)

$(ICONV_DIR_FLAG): $(ICONV_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.iconv: $(ICONV_DIR_FLAG)
	(cd $(ICONV_DIR) && $(CONFIGURE) --enable-static --disable-shared && make && make install)
	touch $@


### libconfig ###

LIBCONFIG_DIR = $(BUILD_DIR)/libconfig-$(LIBCONFIG_VERSION)
LIBCONFIG_DIR_FLAG = $(LIBCONFIG_DIR)/.flag
LIBCONFIG_ARCHIVE = $(ARCHIVE_DIR)/v$(LIBCONFIG_VERSION).tar.gz

$(LIBCONFIG_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/hyperrealm/libconfig/archive/$(shell basename $@)

$(LIBCONFIG_DIR_FLAG): $(LIBCONFIG_ARCHIVE)
	$(TAR) -xvf $<
	touch $@

.libconfig: $(LIBCONFIG_DIR_FLAG)
	(cd $(LIBCONFIG_DIR) && autoreconf -i && patch -p0 < $(BASE)/libconfig++-ignore-docs.patch && patch -p0 < $(BASE)/libconfig++-static-build.patch && CFLAGS=-DLIBCONFIG_STATIC CXXFLAGS=-DLIBCONFIGXX_STATIC $(CONFIGURE) --disable-examples --enable-static --disable-shared && make && make install)
	touch $@


### LibXML 2 ###

LIBXML2_DIR = $(BUILD_DIR)/libxml2-$(LIBXML2_VERSION)
LIBXML2_DIR_FLAG = $(LIBXML2_DIR)/.flag
LIBXML2_ARCHIVE = $(ARCHIVE_DIR)/libxml2-$(LIBXML2_VERSION).tar.gz

$(LIBXML2_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) ftp://xmlsoft.org/libxml2/$(shell basename $@)

$(LIBXML2_DIR_FLAG): $(LIBXML2_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

# TODO: add more --without-xxx flags (apparently needed for build: pattern, debug)
# --with-legacy and --with-html are needed for arabica
.libxml2: $(LIBXML2_DIR_FLAG)
	(cd $(LIBXML2_DIR) && $(CONFIGURE) --enable-static --disable-shared --with-legacy --without-schematron --without-zlib --without-http --without-ftp --without-html --without-python --without-docbook --without-regexps && make install)
	touch $@


### log4cxx ###

LOG4CXX_DIR = $(BUILD_DIR)/apache-log4cxx-$(LOG4CXX_VERSION)
LOG4CXX_DIR_FLAG = $(LOG4CXX_DIR)/.flag
LOG4CXX_ARCHIVE = $(ARCHIVE_DIR)/apache-log4cxx-$(LOG4CXX_VERSION).tar.gz

$(LOG4CXX_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) http://www-eu.apache.org/dist/logging/log4cxx/$(LOG4CXX_VERSION)/$(shell basename $@)

$(LOG4CXX_DIR_FLAG): $(LOG4CXX_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.log4cxx: $(LOG4CXX_DIR_FLAG)
	(cd $(LOG4CXX_DIR) && patch -p0 < $(BASE)/log4cxx-no-example.patch && patch -p0 < $(BASE)/log4cxx-build-fixes.patch && CXXFLAGS=-Wno-narrowing $(CONFIGURE) --with-apr=$(PREFIX) --with-apu=$(PREFIX) --with-apr-util=$(PREFIX) --disable-doxygen --disable-dot --enable-static --disable-shared && make && make install)
	touch $@


### Qt ###

# FIXME: No automated way to build it at the moment :-(
QT_DIR = $(BUILD_DIR)/qt4-$(QT_VERSION)-win32-bin
QT_DIR_FLAG = $(QT_DIR)/.flag
QT_ARCHIVE = $(ARCHIVE_DIR)/qt4-$(QT_VERSION)-win32-bin.tar.bz2

$(QT_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) http://dl.sv.nongnu.org/releases-noredirect/eliot/other/$(shell basename $@)

$(QT_DIR_FLAG): $(QT_ARCHIVE)
	$(TAR) -xjf $<
	touch $@

.qt: $(QT_DIR_FLAG)
	#(cd $<; mkdir -p $(PREFIX)/bin; mkdir -p $(PREFIX)/include; mkdir -p $(PREFIX)/lib/pkgconfig; rm -f $(PREFIX)/lib/pkgconfig/Qt*; sed 's,@@PREFIX@@,$(PREFIX),' lib/pkgconfig/QtCore.pc.in > $(PREFIX)/lib/pkgconfig/QtCore.pc; sed 's,@@PREFIX@@,$(PREFIX),' lib/pkgconfig/QtGui.pc.in > $(PREFIX)/lib/pkgconfig/QtGui.pc; cp -r include/* $(PREFIX)/include; cp lib/*a $(PREFIX)/lib; mkdir -p $(PREFIX)/share/qt4/translations; cp -r share/translations/* $(PREFIX)/share/qt4/translations)
	(cd $(QT_DIR); cp -r bin include lib share $(PREFIX); sed 's,@@PREFIX@@,$(PREFIX),' $(PREFIX)/lib/pkgconfig/QtCore.pc.in > $(PREFIX)/lib/pkgconfig/QtCore.pc; sed 's,@@PREFIX@@,$(PREFIX),' lib/pkgconfig/QtGui.pc.in > $(PREFIX)/lib/pkgconfig/QtGui.pc; rm $(PREFIX)/lib/pkgconfig/*.in)
	touch $@

QT2_DIR = $(BUILD_DIR)/qtbase-opensource-src-$(QT2_VERSION)
QT2_DIR_FLAG = $(QT2_DIR)/.flag
QT2_ARCHIVE = $(ARCHIVE_DIR)/qtbase-opensource-src-$(QT2_VERSION).tar.xz

$(QT2_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://download.qt.io/official_releases/qt/5.6/$(QT2_VERSION)/submodules/$(shell basename $@)

$(QT2_DIR_FLAG): $(QT2_ARCHIVE)
	$(TAR) -xJf $<
	touch $@

.qt2: $(QT2_DIR_FLAG)
	(cd $(QT2_DIR) && CPPFLAGS="$(PREFIX)/include" ./configure -xplatform win32-g++ -device-option CROSS_COMPILE=$(HOST)- --prefix=$(PREFIX) -static -opensource -confirm-license -release -no-pkg-config -no-gif -qt-libjpeg -no-openssl -no-opengl -no-dbus -no-qml-debug -no-audio-backend -no-sql-sqlite -no-sql-odbc -no-pch -no-compile-examples -nomake examples)
	# Build
	(cd $(QT2_DIR) && make)
	# Fix .pc files to remove debug version (d)
	(cd $(QT2_DIR)/lib/pkgconfig && for i in *.pc; do sed -i -e 's/d\.a/.a/g' -e 's/d $$/ /' $$i; done)
	# Fix Qt5Core.pc file to include qwindows (QWindowsIntegrationPlugin) and Qt5Platform Support
	(cd $(QT2_DIR)/lib/pkgconfig; sed -i -e 's/ -lQt5Core/ -lqwindows -lQt5PlatformSupport -lQt5Core/g' Qt5Core.pc)
	# Install
	(cd $(QT2_DIR) && make install)
	# Keep only the qwindows plugin
	mv $(PREFIX)/plugins/platforms/libqwindows.a $(PREFIX)/lib/ && rm -rf $(PREFIX)/plugins
	touch $@


### Xerces ###

XERCES_DIR = $(BUILD_DIR)/xerces-c-$(XERCES_VERSION)
XERCES_DIR_FLAG = $(XERCES_DIR)/.flag
XERCES_ARCHIVE = $(ARCHIVE_DIR)/xerces-c-$(XERCES_VERSION).tar.gz

$(XERCES_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) http://www-eu.apache.org/dist/xerces/c/3/sources/$(shell basename $@)

$(XERCES_DIR_FLAG): $(XERCES_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.xerces: $(XERCES_DIR_FLAG)
	(cd $(XERCES_DIR) && $(CONFIGURE) --enable-static --disable-shared --disable-network && make install)
	touch $@

