---
type: installation
title: Installa Eliot su Debian/Ubuntu
aliases:
- /dl-debian.html
---

## Distribuzioni basate su Ubuntu

Il [PPA di Eliot](https://launchpad.net/~ipkiss-0/+archive/ubuntu/ppa-eliot) offre pacchetti per vecchie versioni di Ubuntu. **Verificate che la vostra versione di Ubuntu sia elencata** nella pagina del PPA prima di aggiungerlo.

Se sono disponibili pacchetti per la vostra versione, eseguite questi comandi in un terminale:

```sh
sudo add-apt-repository ppa:ipkiss-0/ppa-eliot
sudo apt update
sudo apt install eliot
```

Se la vostra versione non è elencata, consultate la [compilazione dai sorgenti](#codice-sorgente) qui sotto oppure [chiedete aiuto per l’installazione](/it/help/#eliot-general).

## Altre distribuzioni basate su Debian

Per l’installazione manuale, consultate i [dettagli dei pacchetti](https://launchpad.net/~ipkiss-0/+archive/ppa-eliot/+packages) e cercate una compilazione adatta alla vostra versione della distribuzione e all’architettura del processore. Il PPA contiene pacchetti Ubuntu; la loro presenza non ne garantisce la compatibilità con Debian.

Scaricate i file `.deb` di `eliot` e `libarabica` per la compilazione scelta. Per esempio, i pacchetti storici per Ubuntu 20.04 (Focal), amd64, si installano con questo comando dalla cartella che contiene i file scaricati:

```sh
sudo dpkg -i libarabica_202004-1_amd64.deb eliot_2.2-focal1_amd64.deb
```

Per una compilazione diversa, usate i nomi dei file che avete scaricato.

## Codice sorgente

Se non è disponibile un pacchetto adatto, consultate il [codice sorgente e le istruzioni di compilazione](/it/dl-sources/).

### Dipendenze della versione di sviluppo attuale

Per la compilazione predefinita con l’interfaccia grafica Qt6, le dipendenze del [CMakeLists.txt principale](https://github.com/ipkiss42/eliot/blob/main/CMakeLists.txt) corrispondono ai seguenti pacchetti di sviluppo:

```sh
sudo apt install build-essential cmake pkg-config gettext \
  libboost-dev libtomlplusplus-dev libpugixml-dev libspdlog-dev qt6-base-dev
```

Il codice attuale richiede CMake 4.3 o successivo e un compilatore compatibile con C++23; verificate le versioni fornite dalla distribuzione. Qt6 Core, Gui, Widgets, Network e PrintSupport sono inclusi in `qt6-base-dev`. Gli header Intl e iconv sono forniti dai file di sviluppo della libreria C di sistema.

Le funzionalità opzionali del terminale usano `libreadline-dev` (modifica della riga) e `libncurses-dev` (interfaccia curses con caratteri estesi).

Queste sono le dipendenze di compilazione del codice attuale. I vecchi pacchetti `.deb` di Eliot 2.2 indicati sopra hanno le proprie dipendenze di esecuzione.

{{< installation-next >}}
