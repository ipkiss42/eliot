---
title: Installer Eliot sous Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Distributions basées sur Ubuntu

Pour les distributions basées sur Ubuntu (telles que [Ubuntu](http://www.ubuntu.com/) ou [Linux Mint](http://linuxmint.com/)), des paquets précompilés sont disponibles sur un [PPA](http://doc.ubuntu-fr.org/ppa). On peut trouver beaucoup d'informations en ligne sur la manière d'installer un PPA, mais cela se résume généralement à :

- `sudo add-apt-repository ppa:ipkiss-0/ppa-eliot`
- `sudo apt update`
- `sudo apt install eliot`

Cette méthode devrait fonctionner à partir d'Ubuntu 11.10 (oneiric).

**Note :** après une nouvelle version d'Ubuntu, cela peut prendre quelques jours avant que les paquets ne soient disponibles sur le PPA.

## Autres distributions basées sur Debian

Les autres distributions basées sur Debian peuvent probablement utiliser les mêmes paquets, mais l'installation est moins pratique.

- [Télécharger](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) les paquets "eliot" et "libarabica" (fichiers `.deb`) pour la bonne architecture (i386 ou amd64)
- Les installer (en adaptant les noms de fichiers) : `sudo dpkg -i libarabica-202004-focal1_amd64.deb eliot_2.2-focal1_amd64.deb`

## Code source

Si vous le souhaitez, vous pouvez également télécharger le [code source](/fr/dl-sources/).

### Dépendances de la version de développement actuelle

Pour la compilation par défaut avec l’interface graphique Qt6, les dépendances du [CMakeLists.txt principal](https://github.com/ipkiss42/eliot/blob/main/CMakeLists.txt) correspondent aux paquets de développement suivants :

```sh
sudo apt install build-essential cmake pkg-config gettext \
  libboost-dev libtomlplusplus-dev libpugixml-dev libspdlog-dev qt6-base-dev
```

Le code actuel nécessite CMake 4.3 ou une version ultérieure et un compilateur compatible C++23 ; vérifiez les versions fournies par votre distribution. Qt6 Core, Gui, Widgets, Network et PrintSupport sont inclus dans `qt6-base-dev`. Les en-têtes Intl et iconv sont fournis par les fichiers de développement de la bibliothèque C du système.

Les fonctions optionnelles en terminal utilisent `libreadline-dev` (édition de ligne) et `libncurses-dev` (interface curses avec caractères larges).

Il s’agit des dépendances de compilation du code actuel. Les anciens paquets `.deb` d’Eliot 2.2 ci-dessus ont leurs propres dépendances d’exécution.
