---
type: installation
title: Build from source
aliases:
- /dl-sources.html
- /dl-others/
- /dl-others.html
---

Build Eliot from source if no package is listed for your operating system or you want a custom build. Choose the released source archive or the development version below.

## Eliot 2.2 {#eliot-21}

Download the released source archive: [eliot-2.2.tar.gz](https://dl.sv.nongnu.org/releases/eliot/releases/2.2/eliot-2.2.tar.gz).

Extract the archive and follow the `INSTALL` file inside it. This file describes the dependencies and build commands for that release.

## Development version

The development code is available on [GitHub](https://github.com/ipkiss42/eliot). To download it with Git:

```sh
git clone https://github.com/ipkiss42/eliot.git
cd eliot
```

Read the `INSTALL` file in this checkout before building. Release and development versions have different dependencies; the [Debian/Ubuntu page](/en/dl-debian/#dependencies-for-the-current-development-version) lists the current development build requirements for those systems.

## Older versions

Older source archives are available in the [Savannah release archive](http://dl.sv.nongnu.org/releases/eliot/releases/).

## Other packages

If you know of a precompiled package for another system, [tell us on the eliot-dev email list](/en/help/#eliot-dev) so we can add a link.

{{< installation-next build=true >}}
