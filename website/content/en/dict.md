---
title: Dictionaries
type: dictionaries
aliases:
- /dict.html
---

{{< dictionary-index >}}

## Bambara {#bm}

The dictionary below is based on the Bambara-French dictionary by Charles Bailleul. The letter distribution has been calculated using the [Corpus bambara (FR)](http://cormand.tge-adonis.fr/).

- [Bamanakan v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/bambara-bamanakan-v1.dawg)

{{< space >}}

## Catalan {#ca}

The DISC (Diccionari Informatitzat de l'Scrabble en Català) is an alternative dictionary to the official Catalan dictionary (DOSC).{{< br >}} For more details, see the [dedicated site (in Catalan)](http://diccionari.totescrable.cat/).

- [DISC 2.4.10](https://github.com/ipkiss42/eliot/releases/download/dictionaries/catalan-disc-v2.4.10.dawg)

{{< space >}}

## Czech {#cs}

- [czech-unofficial-v1.dawg](https://github.com/ipkiss42/eliot/releases/download/dictionaries/czech-unofficial-v1.dawg)

The word list of this dictionary has been obtained from the following sources:

- [Unofficial list](http://www.gtoal.com/wordgames/details/czech/czech.txt)
- [List of words from 2 to 5 letters](http://scrabble.hrejsi.cz/pravidla/blex.htm)

**Note:** the letters follow the distribution indicated on [this page](http://hrejsi.cz/scrabble/pravidla.htm). Thus the few words containing a W or a Z must necessarily be played using the jokers.

## English {#en}

The dictionaries below contain official word lists used for playing Scrabble in English:

- [CSW 2012](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-csw-2012.dawg)
- [TWL 2006](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-twl-2006.dawg)

{{< space >}}

## French {#fr}

- [french-unofficial-v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/french-unofficial-v1.dawg)

The word list of this dictionary contains additions and omissions, compared to the Officiel Du Scrabble (ODS 6) word list, which contains 386264 words). It has been obtained by a combination of the following sources:

- [Français-Gutemberg list](http://www.pallier.org/ressources/dicofr/dicofr.html)
- [Moby list for French](http://www.gutenberg.org/etext/3206)
- [List of words between 2 and 4 letters](http://www.motsduscrabble.com/)

**Note:** for legal reasons, the word list of the Officiel Du Scrabble (ODS) is not provided anymore on this site. It is however possible to find other word lists on the Internet (including the ODS one...), and to use them to [build your own dictionary](#own).

{{< space >}}

## Italian {#it}

The dictionary below contains the official word list for playing Scrabble in Italian. Enclitic forms are also accepted in live games, but there is at the moment no list containing them all.

- [Zingarelli 2011](https://github.com/ipkiss42/eliot/releases/download/dictionaries/italian-zingarelli-2011.dawg)

**Note:** words containing the letters J, K, W, X or Y can only be played using the jokers.

{{< space >}}

## Malagasy {#mg}

You can play in Malagasy with the below dictionary:

- [RAK](https://github.com/ipkiss42/eliot/releases/download/dictionaries/malagasy-rakibolana-v1.dawg)

{{< space >}}

## Norwegian {#no}

The dictionary below is based on the official word list, available [here](http://www2.scrabbleforbundet.no/?attachment_id=1620):

- [NSF](https://github.com/ipkiss42/eliot/releases/download/dictionaries/norwegian-nsf-2010.dawg)

{{< space >}}

## Polish {#po}

The dictionary below is based on the SJP (Słownik Języka Polskiego), available [here](http://www.sjp.pl/slownik/growy/):

- [SJP](https://github.com/ipkiss42/eliot/releases/download/dictionaries/polish-sjp-2012.dawg)

{{< space >}}

## Romanian {#ro}

The dictionary below is based on the official word list, available [here](http://www.scrabrom.3x.ro/LOC.html):

- [LOC 5](https://github.com/ipkiss42/eliot/releases/download/dictionaries/romanian-loc-5.dawg)

{{< space >}}

## Serbian {#rs}

There is no official word list for playing Scrabble in Serbian. Here is an unofficial dictionary:

- [Serbian](https://github.com/ipkiss42/eliot/releases/download/dictionaries/serbian-unofficial-v1.dawg)

{{< space >}}

## Spanish {#es}

The dictionary below uses the official 2017 word list of the Federación Internacional de Léxico en Español ([FILE](https://www.filexico.com/)):

- [FILE 2017](https://github.com/ipkiss42/eliot/releases/download/dictionaries/spanish-file-2017.dawg)

{{< space >}}

## Create your own dictionary {#own}

Since Eliot 1.12, a wizard allows creating very easily your own dictionary from any list of words. The wizard is available in the *Settings* menu: *Create a new dictionary...*. Then you just need to follow the instructions...

**Note:** languages using double letters for a single tile (like the spanish *LL* or the catalan *QU*) must use the old method indicated below..

For those who prefer to use a command-line tool, another (more complex) method consists in using the **compdic** program, provided with Eliot (and pre-compiled for Windows).

Here is the procedure to follow:

- Save the word list to a file (for example *words.txt*), using UTF-8 encoding
- Create a letters definition file (for example *letters.txt*), also encoded in UTF-8. Each line of the file corresponds to a letter, and contains the following fields (separated with one or more spaces):
  - Letter (for the joker, use the interrogation mark)
  - Number of points
  - Frequency (number of occurrences)
  - 1 if the letter is a vowel, 0 otherwise
  - 1 if the letter is a consonant, 0 otherwise
  
  An example of letters file, suitable for French Scrabble, is available [here](/download/letters.txt)
- Run the **compdic** program, using the following command-line:{{< br >}} `compdic -d "Nom du dictionnaire" -i words.txt -l letters.txt -o my-dic.dawg`{{< br >}} This will generate a file called *my-dic.dawg*, usable with Eliot.
