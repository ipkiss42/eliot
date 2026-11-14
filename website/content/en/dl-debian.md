---
title: Download Eliot for Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Ubuntu-based distributions

For Ubuntu-based distributions (such as [Ubuntu](http://www.ubuntu.com/) or [Linux Mint](http://linuxmint.com/)), pre-built packages are available on a [PPA](https://en.wikipedia.org/wiki/Personal_Package_Archive). There is a lot of documentation online about how to activate a PPA, but usually the following is enough:

- `sudo add-apt-repository ppa:ipkiss-0/ppa-eliot`
- `sudo apt update`
- `sudo apt install eliot`

This method should work starting from Ubuntu 11.10 (oneiric).{{< br >}}

**Note:** after a new release of Ubuntu it may take a few days before the packages are available on the PPA.

## Other Debian-based distributions

Other Debian-based distributions can probably also use the same packages, but the installation is less practical.

- Install the dependencies: `sudo apt install libqtcore5 libqtgui5 libexpat1 libconfig++9`
- [Download](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) the "eliot" and "libarabica" packages (.deb files) for your architecture (i386 or amd64)
- Install them (changing the file names accordingly): `sudo dpkg -i libarabica-202004-focal1_amd64.deb eliot_2.2-focal1_amd64.deb`

## Source code

If you want, you can download the [source code](/en/dl-sources/) of Eliot.
