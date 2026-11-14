---
title: Download Eliot for Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Ubuntu-based distributions

For Ubuntu-based distributions (such as [Ubuntu](http://www.ubuntu.com/) or [Linux Mint](http://linuxmint.com/)), precompiled packages are available on a [PPA](https://en.wikipedia.org/wiki/Personal_Package_Archive). There is a lot of documentation online about how to activate a PPA, but usually the following is enough:

- `sudo add-apt-repository ppa:ipkiss-0/ppa-eliot`
- `sudo apt update`
- `sudo apt install eliot`

This method should work starting from Ubuntu 11.10 (oneiric).{{< br >}}

**Note:** after a new release of Ubuntu it may take a few days before the packages are available on the PPA.

## Other Debian-based distributions

Other Debian-based distributions can probably also use the same packages, but the installation is less practical.

- [Download](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) the "eliot" and "libarabica" packages (`.deb` files) for your architecture (i386 or amd64)
- Install them (changing the file names accordingly): `sudo dpkg -i libarabica-202004-focal1_amd64.deb eliot_2.2-focal1_amd64.deb`

## Source code

If you want, you can download the [source code](/en/dl-sources/) of Eliot.

### Dependencies for the current development version

For the default Qt6 graphical build, the dependencies in the [main CMakeLists.txt](https://github.com/ipkiss42/eliot/blob/main/CMakeLists.txt) correspond to these development packages:

```sh
sudo apt install build-essential cmake pkg-config gettext \
  libboost-dev libtomlplusplus-dev libpugixml-dev libspdlog-dev qt6-base-dev
```

The current source requires CMake 4.3 or newer and a C++23-capable compiler; check the versions provided by your distribution. Qt6 Core, Gui, Widgets, Network, and PrintSupport are included in `qt6-base-dev`. The Intl and iconv headers are provided by the system C library development files.

Optional terminal support uses `libreadline-dev` (line editing) and `libncurses-dev` (wide-character curses interface).

These are build dependencies for the current source. The historical Eliot 2.2 `.deb` packages above have their own runtime dependencies.
