---
type: installation
title: Installer Eliot sous Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Distributions basées sur Ubuntu

Le [PPA d’Eliot](https://launchpad.net/~ipkiss-0/+archive/ubuntu/ppa-eliot) propose des paquets pour d’anciennes versions d’Ubuntu. **Vérifiez que votre version d’Ubuntu figure dans la liste** du PPA avant de l’ajouter.

Si des paquets sont disponibles pour votre version, exécutez ces commandes dans un terminal :

```sh
sudo add-apt-repository ppa:ipkiss-0/ppa-eliot
sudo apt update
sudo apt install eliot
```

Si votre version ne figure pas dans la liste, consultez la [compilation depuis les sources](#code-source) ci-dessous ou [demandez de l’aide pour l’installation](/fr/help/#eliot-dev).

## Autres distributions basées sur Debian

Pour une installation manuelle, consultez les [détails des paquets](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) et recherchez une compilation adaptée à votre version de distribution et à l’architecture de votre processeur. Le PPA contient des paquets Ubuntu ; leur présence ne garantit pas leur compatibilité avec Debian.

Téléchargez les fichiers `.deb` d’`eliot` et de `libarabica` correspondant à cette compilation. Par exemple, les paquets historiques pour Ubuntu 20.04 (Focal), amd64, s’installent avec cette commande, depuis le dossier où se trouvent les fichiers téléchargés :

```sh
sudo dpkg -i libarabica_202004-1_amd64.deb eliot_2.2-focal1_amd64.deb
```

Pour une autre compilation, utilisez les noms des fichiers que vous avez téléchargés.

## Code source

Si aucun paquet adapté n’est disponible, consultez le [code source et les instructions de compilation](/fr/dl-sources/).

### Dépendances de la version de développement actuelle

Pour la compilation par défaut avec l’interface graphique Qt6, les dépendances du [CMakeLists.txt principal](https://github.com/ipkiss42/eliot/blob/main/CMakeLists.txt) correspondent aux paquets de développement suivants :

```sh
sudo apt install build-essential cmake pkg-config gettext \
  libboost-dev libtomlplusplus-dev libpugixml-dev libspdlog-dev qt6-base-dev
```

Le code actuel nécessite CMake 4.3 ou une version ultérieure et un compilateur compatible C++23 ; vérifiez les versions fournies par votre distribution. Qt6 Core, Gui, Widgets, Network et PrintSupport sont inclus dans `qt6-base-dev`. Les en-têtes Intl et iconv sont fournis par les fichiers de développement de la bibliothèque C du système.

Les fonctions optionnelles en terminal utilisent `libreadline-dev` (édition de ligne) et `libncurses-dev` (interface curses avec caractères larges).

Il s’agit des dépendances de compilation du code actuel. Les anciens paquets `.deb` d’Eliot 2.2 ci-dessus ont leurs propres dépendances d’exécution.

{{< installation-next >}}
