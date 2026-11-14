---
title: Scarica Eliot per Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Distribuzioni basate su Ubuntu

Per le “distro” basate su Ubuntu (come [Ubuntu](http://www.ubuntu.com/) o [Linux Mint](http://linuxmint.com/)), sono disponibili pacchetti precompilati su un [PPA](https://en.wikipedia.org/wiki/Personal_Package_Archive). Ci sono molte informazioni in rete su come attivare un PPA, ma di solito è sufficiente questa:

- `sudo add-apt-repository ppa:ipkiss-0/ppa-eliot`
- `sudo apt update`
- `sudo apt install eliot`

Questo metodo dovrebbe funzionare a partire da Ubuntu 11.10 (oneiric).{{< br >}}

**Nota:** dopo un nuovo rilascio di Ubuntu potrebbero essere necessari alcuni giorni prima che i pacchetti siano disponibili sul PPA.

## Altre distribuzioni basate su Debian

Altre “distro” basate su Debian possono probabilmente anche utilizzare gli stessi pacchetti, ma l'installazione è meno pratica.

- Installa le dipendenze: `sudo apt install libqtcore5 libqtgui5 libexpat1 libconfig++9`
- [Scarica](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) i pacchetti "eliot" e "libarabica" (.deb files) per la tua architettura (i386 o amd64)
- Installale (cambiando i nomi dei file di conseguenza): `sudo dpkg -i libarabica-202004-focal1_amd64.deb eliot_2.2-focal1_amd64.deb`

## Codice sorgente

Volendo è possibile scaricare il [codice sorgente](/it/dl-sources/) di Eliot.
