---
title: Télécharger Eliot pour Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Distributions basées sur Ubuntu

Pour les distributions basées sur Ubuntu (telles que [Ubuntu](http://www.ubuntu.com/) ou [Linux Mint](http://linuxmint.com/)), des paquets précompilés sont disponibles sur un [PPA](http://doc.ubuntu-fr.org/ppa). On peut trouver beaucoup d'information en ligne sur la manière d'installer un PPA, mais cela se résume généralement à :

- `sudo add-apt-repository ppa:ipkiss-0/ppa-eliot`
- `sudo apt update`
- `sudo apt install eliot`

Cette méthode devrait fonctionner à partir d'Ubuntu 11.10 (oneiric).

**Note :** après une nouvelle release d'Ubuntu, cela peut prendre quelques jours avant que les paquets ne soient disponibles sur le PPA.

## Autres distributions basées sur Debian

Les autres distributions basées sur Debian peuvent probablement utiliser les mêmes paquets, mais l'installation est moins pratique.

- Installer les dépendances : `sudo apt install libqtcore5 libqtgui5 libexpat1 libconfig++9`
- [Télécharger](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) les paquets "eliot" et "libarabica" (fichiers .deb) pour la bonne architecture (i386 ou amd64)
- Les installer (en adaptant les noms de fichiers) : `sudo dpkg -i libarabica-202004-focal1_amd64.deb eliot_2.2-focal1_amd64.deb`

## Code source

Si vous le souhaitez, vous pouvez également télécharger le [code source](/fr/dl-sources/).
