---
type: installation
title: Compilare Eliot dai sorgenti
aliases:
- /dl-sources.html
- /dl-others/
- /dl-others.html
---

Se non è disponibile un pacchetto per il vostro sistema operativo o volete una compilazione personalizzata, compilate Eliot dai sorgenti. Scegliete qui sotto l’archivio della versione pubblicata oppure la versione di sviluppo.

## Eliot 2.2 {#eliot-21}

Scaricate l’archivio del codice sorgente della versione pubblicata: [eliot-2.2.tar.gz](https://dl.sv.nongnu.org/releases/eliot/releases/2.2/eliot-2.2.tar.gz).

Estraete l’archivio e seguite il file `INSTALL` al suo interno. Descrive le dipendenze e i comandi di compilazione di quella versione.

## Versione di sviluppo

Il codice di sviluppo è disponibile su [GitHub](https://github.com/ipkiss42/eliot). Per scaricarlo con Git:

```sh
git clone https://github.com/ipkiss42/eliot.git
cd eliot
```

Leggete il file `INSTALL` di questa copia prima di compilare. Le versioni pubblicata e di sviluppo hanno dipendenze diverse; la [pagina Debian/Ubuntu](/it/dl-debian/#dipendenze-della-versione-di-sviluppo-attuale) elenca i requisiti di compilazione del codice attuale per questi sistemi.

## Vecchie versioni

I vecchi archivi del codice sorgente sono disponibili nell’[archivio delle versioni su Savannah](http://dl.sv.nongnu.org/releases/eliot/releases/).

## Altri pacchetti

Se conoscete un pacchetto precompilato per un altro sistema, [segnalatelo alla lista di discussione eliot-dev](/it/help/#eliot-dev) così potremo aggiungere un link.

{{< installation-next build=true >}}
