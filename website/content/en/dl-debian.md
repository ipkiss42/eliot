---
type: installation
title: Install Eliot on Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Ubuntu-based distributions

The [Eliot PPA](https://launchpad.net/~ipkiss-0/+archive/ubuntu/ppa-eliot) provides packages for older Ubuntu releases. **Check that your Ubuntu release is listed** on the PPA page before adding it.

If packages are available for your release, run these commands in a terminal:

```sh
sudo add-apt-repository ppa:ipkiss-0/ppa-eliot
sudo apt update
sudo apt install eliot
```

If your release is not listed, see the [source-build option](#source-code) below or [ask for installation help](/en/help/#eliot-dev).

## Other Debian-based distributions

For manual installation, check the [package details](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) for a build matching your distribution release and processor architecture. The PPA contains Ubuntu packages; availability there does not establish compatibility with Debian.

Download the `eliot` and `libarabica` `.deb` files for the matching build. For example, the historical Ubuntu 20.04 (Focal), amd64 packages use this command from the folder containing the downloaded files:

```sh
sudo dpkg -i libarabica_202004-1_amd64.deb eliot_2.2-focal1_amd64.deb
```

Use the filenames of your downloaded packages when installing a different build.

## Source code

If no suitable package is available, see the [source code and build instructions](/en/dl-sources/).

### Dependencies for the current development version

For the default Qt6 graphical build, the dependencies in the [main CMakeLists.txt](https://github.com/ipkiss42/eliot/blob/main/CMakeLists.txt) correspond to these development packages:

```sh
sudo apt install build-essential cmake pkg-config gettext \
  libboost-dev libtomlplusplus-dev libpugixml-dev libspdlog-dev qt6-base-dev
```

The current source requires CMake 4.3 or newer and a C++23-capable compiler; check the versions provided by your distribution. Qt6 Core, Gui, Widgets, Network, and PrintSupport are included in `qt6-base-dev`. The Intl and iconv headers are provided by the system C library development files.

Optional terminal support uses `libreadline-dev` (line editing) and `libncurses-dev` (wide-character curses interface).

These are build dependencies for the current source. The historical Eliot 2.2 `.deb` packages above have their own runtime dependencies.

{{< installation-next >}}
