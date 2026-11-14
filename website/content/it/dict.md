---
title: Dizionari
type: dictionaries
aliases:
- /dict.html
---

{{< dictionary-index >}}

## Bambara {#bm}

The dictionary below is based on the Bambara-French dictionary by Charles Bailleul. The letter distribution has been calculated using the [Corpus bambara (FR)](http://cormand.tge-adonis.fr/).

- [Bamanakan v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/bambara-bamanakan-v1.dawg)

{{< space >}}

## Catalano {#ca}

Il DISC (Diccionari Informatitzat de l'Scrabble en Català) è un dizionario alternativo a quello ufficiale catalano (DOSC).{{< br >}} Per ulteriori dettagli, visita il [sito dedicato (in catalano)](http://diccionari.totescrable.cat/).

- [DISC 2.4.10](https://github.com/ipkiss42/eliot/releases/download/dictionaries/catalan-disc-v2.4.10.dawg)

{{< space >}}

## Ceco {#cs}

- [czech-unofficial-v1.dawg](https://github.com/ipkiss42/eliot/releases/download/dictionaries/czech-unofficial-v1.dawg)

Il dizionario è ottenuto dalle seguenti fonti:

- [Unofficial list](http://www.gtoal.com/wordgames/details/czech/czech.txt)
- [Lista di parole da 2 a 5 lettere](http://scrabble.hrejsi.cz/pravidla/blex.htm)

**Nota:** le lettere seguono la distribuzione indicata su [questa pagina](http://hrejsi.cz/scrabble/pravidla.htm). Pertanto le poche parole contenenti una W o Z devono necessariamente essere giocate usando i jolly.

## Francese {#fr}

- [french-unofficial-v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/french-unofficial-v1.dawg)

La lista delle parole di questo dizionario contiene aggiunte ed omissioni, rispetto all'elenco di parole Officiel du Scrabble (ODS 6), che contiene 386.264 parole. È ottenuta da una combinazione delle seguenti fonti:

- [Français-Gutenberg](http://www.pallier.org/ressources/dicofr/dicofr.html)
- [Moby lista Francese](http://www.gutenberg.org/etext/3206)
- [Lista di parole da 2 a 4 lettere](http://www.motsduscrabble.com/)

**Nota:** per motivi legali, la lista di parole Officiel Du Scrabble (ODS) non viene più fornita su questo sito. È comunque possibile trovare altri elenchi di parole su Internet (compreso l'ODS ...), e utilizzarli per [creare il proprio dizionario](#own).

{{< space >}}

## Inglese {#en}

I dizionari qui sotto contengono liste di parole ufficiali per giocare a Scrabble in inglese:

- [CSW 2012](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-csw-2012.dawg)
- [TWL 2006](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-twl-2006.dawg)

{{< space >}}

## Italiano {#it}

Il dizionario che segue è la lista ufficiale di parole per giocare a Scrabble in italiano, nel gioco dal vivo sono ammesse anche le particelle enclitiche, ma attualmente non esiste una lista che le contenga.{{< br >}} Le parole contenenti le lettere J - K - W - X - Y sono contemplate, ma possono essere giocate solo usando i jolly.

- [Zingarelli 2011](https://github.com/ipkiss42/eliot/releases/download/dictionaries/italian-zingarelli-2011.dawg)

{{< space >}}

## Malgascio {#mg}

Puoi giocare in malgascio con questo dizionario:

- [RAK](https://github.com/ipkiss42/eliot/releases/download/dictionaries/malagasy-rakibolana-v1.dawg)

{{< space >}}

## Norvegese {#no}

The dictionary below is based on the official word list, available [here](http://www2.scrabbleforbundet.no/?attachment_id=1620):

- [NSF](https://github.com/ipkiss42/eliot/releases/download/dictionaries/norwegian-nsf-2010.dawg)

{{< space >}}

## Polacco {#po}

The dictionary below is based on the SJP (Słownik Języka Polskiego), available [here](http://www.sjp.pl/slownik/growy/):

- [SJP](https://github.com/ipkiss42/eliot/releases/download/dictionaries/polish-sjp-2012.dawg)

{{< space >}}

## Rumeno {#ro}

Il dizionario qui sotto è basato sulla lista ufficiale, disponibile [qui](http://www.scrabrom.3x.ro/LOC.html):

- [LOC 5](https://github.com/ipkiss42/eliot/releases/download/dictionaries/romanian-loc-5.dawg)

{{< space >}}

## Serbo {#rs}

Non esiste un dizionario ufficiale per giocare a Scrabble in serbo. Questo è un dizionario non ufficiale:

- [Serbo](https://github.com/ipkiss42/eliot/releases/download/dictionaries/serbian-unofficial-v1.dawg)

{{< space >}}

## Spagnolo {#es}

The dictionary below uses the official 2017 word list of the Federación Internacional de Léxico en Español ([FILE](https://www.filexico.com/)):

- [FILE 2017](https://github.com/ipkiss42/eliot/releases/download/dictionaries/spanish-file-2017.dawg)

{{< space >}}

## Crea il tuo dizionario {#own}

A partire dalla versione 1.12 di Eliot, una procedura guidata permette di creare con estrema facilità un proprio dizionario da qualsiasi elenco di parole. La procedura guidata è disponibile nel menu *Impostazioni*: *Crea un nuovo dizionario...*. Poi basta seguire le istruzioni ...

**Nota:** Le lingue che utilizzano lettere doppie per una singola lettera (come lo spagnolo *LL* o il catalano *QU*) devono utilizzare il vecchio metodo sotto indicato.

Per coloro che non hanno ancora la versione 1.12, o che preferiscono utilizzare una riga di comando, un altro (più complesso) metodo consiste nell'utilizzare il programma compdic, fornito con Eliot (e pre-compilato per Windows).

Ecco la procedura da seguire:

- Salva la lista delle parole in un file (ad esempio *parole.txt*), usando la codifica UTF-8
- Crea un file di definizione lettere (per esempio *lettere.txt*), sempre con codifica UTF-8. Ogni riga del file corrisponde ad una lettera, e contiene i seguenti campi (separati da uno o più spazi):

* Lettera (per il jolly, utilizza il punto interrogativo)
* Numero di punti
* Frequenza (numero di occorrenze)
* 1 se la lettera è una vocale, 0 altrimenti
* 1 se la lettera è una consonante, 0 altrimenti

{{< plain >}}
Un esempio di file, adatto per lo Scrabble in francese è disponibile [qui](/download/letters.txt)
{{< /plain >}}

- Esegui il programma **compdic**, utilizzando la seguente riga di comando:{{< br >}} `compdic -d "Nome del dizionario" -i parole.txt -l lettere.txt -o my-dic.dawg`{{< br >}} Questo genererà un file chiamato *my-dic.dawg*, utilizzabile con Eliot.
