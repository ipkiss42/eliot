BOOST_VERSION := 1.91.0
EXPAT_VERSION := 2.2.9
GETTEXT_VERSION := 1.0
ICONV_VERSION := 1.19
PUGIXML_VERSION := 1.16
QT5_VERSION := 5.6.3
QT6_VERSION := 6.11.1
SPDLOG_VERSION := 1.17.0
TOMLPLUSPLUS_VERSION := 3.4.0
XERCES_VERSION := 3.2.2

HOST := x86_64-w64-mingw32
BUILD := i386-linux

# Adjust for your installation
TOOLCHAIN_PREFIX := /opt/llvm-mingw/llvm-mingw-ucrt/
# XXX: Hardcoded for mingw on linux, at the moment
CC = $(HOST)-clang
CXX = $(HOST)-clang++

BASE = $(shell pwd)
PREFIX = $(BASE)/inst
PREFIX_FLAG = $(PREFIX)/.flag
ARCHIVE_DIR = $(BASE)/archives
BUILD_DIR = $(BASE)/build
WGET = wget -c -P $(ARCHIVE_DIR)
TAR = tar -C $(BUILD_DIR)

CXXFLAGS = -std=c++17
CPPFLAGS += -I$(PREFIX)/include
CONFIGURE = CC="$(CC)" CXX="$(CXX)" CPPFLAGS="$(CPPFLAGS)" CXXFLAGS="$(CXXFLAGS)" ./configure --host=$(HOST) --build=$(BUILD) --prefix=$(PREFIX) --enable-static --disable-shared
CMAKE = cmake -B . -DCMAKE_TOOLCHAIN_FILE=$(BASE)/../cmake/llvm-mingw-compiler.cmake -DCMAKE_INSTALL_PREFIX="$(PREFIX)"

# Dependencies (incomplete list):
LIBS := iconv gettext boost tomlplusplus pugixml spdlog qt6
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


### Boost ###

BOOST_UNDERSCORE_VERSION = $(subst .,_,$(BOOST_VERSION))
BOOST_DIR = $(BUILD_DIR)/boost_$(BOOST_UNDERSCORE_VERSION)
BOOST_DIR_FLAG = $(BOOST_DIR)/.flag
BOOST_ARCHIVE = $(ARCHIVE_DIR)/boost_$(BOOST_UNDERSCORE_VERSION).tar.bz2

$(BOOST_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://archives.boost.io/release/$(BOOST_VERSION)/source/$(shell basename $@)

$(BOOST_DIR_FLAG): $(BOOST_ARCHIVE)
	$(TAR) -xjf $<
	touch $@

# We don't build any library, because we don't need them (and it would take
# a lot of time)
.boost: $(BOOST_DIR_FLAG)
	(cd $(BOOST_DIR) \
		&& ./bootstrap.sh \
		&& ./b2 install --prefix=$(PREFIX) --with-headers)
# 	cp -r $(BOOST_DIR)/boost $(PREFIX)/include
	touch $@


### gettext ###

GETTEXT_DIR = $(BUILD_DIR)/gettext-$(GETTEXT_VERSION)
GETTEXT_DIR_FLAG = $(GETTEXT_DIR)/.flag
GETTEXT_ARCHIVE = $(ARCHIVE_DIR)/gettext-$(GETTEXT_VERSION).tar.gz

$(GETTEXT_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://ftp.gnu.org/pub/gnu/gettext/$(shell basename $@)

$(GETTEXT_DIR_FLAG): $(GETTEXT_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.gettext: $(GETTEXT_DIR_FLAG)
	(cd $(GETTEXT_DIR) \
		&& $(CONFIGURE) --disable-java --disable-java --disable-modula2 --disable-curses --disable-d --disable-openmp --disable-acl --disable-relocatable --without-emacs --without-included-libxml \
		&& make install)
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
	(cd $(ICONV_DIR) \
		&& $(CONFIGURE) \
		&& make install)
	touch $@


### tomlplusplus ###

TOMLPLUSPLUS_DIR = $(BUILD_DIR)/tomlplusplus-$(TOMLPLUSPLUS_VERSION)
TOMLPLUSPLUS_DIR_FLAG = $(TOMLPLUSPLUS_DIR)/.flag
TOMLPLUSPLUS_ARCHIVE = $(ARCHIVE_DIR)/v$(TOMLPLUSPLUS_VERSION).tar.gz

$(TOMLPLUSPLUS_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/marzer/tomlplusplus/archive/$(shell basename $@)

$(TOMLPLUSPLUS_DIR_FLAG): $(TOMLPLUSPLUS_ARCHIVE)
	$(TAR) -xvf $<
	touch $@

.tomlplusplus: $(TOMLPLUSPLUS_DIR_FLAG)
	(cd $(TOMLPLUSPLUS_DIR) \
		&& $(CMAKE) \
		&& make install)
	touch $@


### pugixml ###

PUGIXML_DIR = $(BUILD_DIR)/pugixml-$(PUGIXML_VERSION)
PUGIXML_DIR_FLAG = $(PUGIXML_DIR)/.flag
PUGIXML_ARCHIVE = $(ARCHIVE_DIR)/pugixml-$(PUGIXML_VERSION).tar.gz

$(PUGIXML_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/zeux/pugixml/releases/download/v$(PUGIXML_VERSION)/$(shell basename $@)

$(PUGIXML_DIR_FLAG): $(PUGIXML_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.pugixml: $(PUGIXML_DIR_FLAG)
	# Use header-only mode to avoid a weird linking error with seekg(std::fpos<int>)
	(cd $(PUGIXML_DIR) \
		&& sed -i 's@// \(#define PUGIXML_HEADER_ONLY\)@\1@' src/pugiconfig.hpp \
		&& sed -i 's@// \(#define PUGIXML_NO_XPATH\)@\1@' src/pugiconfig.hpp \
		&& $(CMAKE) -DPUGIXML_INSTALL_SOURCE=ON \
		&& make install)
	touch $@


### Qt ###

QT5_DIR = $(BUILD_DIR)/qtbase-opensource-src-$(QT5_VERSION)
QT5_DIR_FLAG = $(QT5_DIR)/.flag
QT5_ARCHIVE = $(ARCHIVE_DIR)/qtbase-opensource-src-$(QT5_VERSION).tar.xz

$(QT5_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://download.qt.io/official_releases/qt/5.6/$(QT5_VERSION)/submodules/$(shell basename $@)

$(QT5_DIR_FLAG): $(QT5_ARCHIVE)
	$(TAR) -xJf $<
	touch $@

.qt5: $(QT5_DIR_FLAG)
	(cd $(QT5_DIR) && CPPFLAGS="$(PREFIX)/include" ./configure -xplatform win32-g++ -device-option CROSS_COMPILE=$(HOST)- --prefix=$(PREFIX) -static -opensource -confirm-license -release -no-pkg-config -no-gif -qt-libjpeg -no-openssl -no-opengl -no-dbus -no-qml-debug -no-audio-backend -no-sql-sqlite -no-sql-odbc -no-pch -no-compile-examples -nomake examples)
	# Build
	(cd $(QT5_DIR) && make)
	# Fix .pc files to remove debug version (d)
	(cd $(QT5_DIR)/lib/pkgconfig && for i in *.pc; do sed -i -e 's/d\.a/.a/g' -e 's/d $$/ /' $$i; done)
	# Fix Qt5Core.pc file to include qwindows (QWindowsIntegrationPlugin) and Qt5Platform Support
	(cd $(QT5_DIR)/lib/pkgconfig; sed -i -e 's/ -lQt5Core/ -lqwindows -lQt5PlatformSupport -lQt5Core/g' Qt5Core.pc)
	# Install
	(cd $(QT5_DIR) && make install)
	# Keep only the qwindows plugin
	mv $(PREFIX)/plugins/platforms/libqwindows.a $(PREFIX)/lib/ && rm -rf $(PREFIX)/plugins
	touch $@


QT6_DIR = $(BUILD_DIR)/qtbase-everywhere-src-$(QT6_VERSION)
QT6_DIR_FLAG = $(QT6_DIR)/.flag
QT6_ARCHIVE = $(ARCHIVE_DIR)/qtbase-everywhere-src-$(QT6_VERSION).tar.xz

$(QT6_ARCHIVE): $(PREFIX_FLAG)
	# FIXME: "6.11" hardcoded
	$(WGET) https://download.qt.io/archive/qt/6.11/$(QT6_VERSION)/submodules/qtbase-everywhere-src-$(QT6_VERSION).tar.xz

$(QT6_DIR_FLAG): $(QT6_ARCHIVE)
	$(TAR) -xJf $<
	touch $@

.qt6: $(QT6_DIR_FLAG)
	(cd $(QT6_DIR) && mkdir -p build && cd build && \
		cmake .. \
		-G Ninja \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=$(PREFIX) \
		-DCMAKE_TOOLCHAIN_FILE=$(BASE)/../cmake/llvm-mingw-compiler.cmake \
		-DCMAKE_FIND_ROOT_PATH="$(TOOLCHAIN_PREFIX)${HOST}" \
		-DQT_HOST_PATH=/usr \
		-DQT_BUILD_SHARED_LIBS=OFF \
		-DBUILD_SHARED_LIBS=OFF \
		-DQT_BUILD_EXAMPLES=OFF \
		-DQT_BUILD_TESTS=OFF \
		-DFEATURE_testlib=OFF \
		-DFEATURE_dbus=OFF \
		-DFEATURE_sql=OFF \
		-DFEATURE_testlib=OFF \
		-DFEATURE_xml=OFF \
		-Wno-author)
	cmake --build $(QT6_DIR)/build --parallel 4
	cmake --install $(QT6_DIR)/build
	touch $@


### spdlog ###

SPDLOG_DIR = $(BUILD_DIR)/spdlog-$(SPDLOG_VERSION)
SPDLOG_DIR_FLAG = $(SPDLOG_DIR)/.flag
SPDLOG_ARCHIVE = $(ARCHIVE_DIR)/v$(SPDLOG_VERSION).tar.gz

$(SPDLOG_ARCHIVE): $(PREFIX_FLAG)
	$(WGET) https://github.com/gabime/spdlog/archive/refs/tags/$(shell basename $@)

$(SPDLOG_DIR_FLAG): $(SPDLOG_ARCHIVE)
	$(TAR) -xzf $<
	touch $@

.spdlog: $(SPDLOG_DIR_FLAG)
	(cd $(SPDLOG_DIR) \
		&& $(CMAKE) -DSPDLOG_BUILD_EXAMPLE=OFF \
		&& make install)
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
	(cd $(XERCES_DIR) \
		&& $(CONFIGURE) --disable-network \
		&& make install)
	touch $@

