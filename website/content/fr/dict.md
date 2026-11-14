---
title: Dictionnaires
type: dictionaries
aliases:
- /dict.html
---

Un dictionnaire pour Eliot est un fichier `.dawg` compilé à partir d’une liste de mots. Les sections ci-dessous décrivent les listes de mots utilisées par les dictionnaires disponibles.

{{< dictionary-index >}}

## Anglais {#en}

Les dictionnaires ci-dessous contiennent les listes de mots de référence utilisées pour le Scrabble anglophone :

- [CSW 2012](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-csw-2012.dawg)
- [TWL 2006](https://github.com/ipkiss42/eliot/releases/download/dictionaries/english-twl-2006.dawg)

{{< space >}}

## Bambara {#bm}

Le dictionnaire ci-dessous est basé sur le Dictionnaire bambara-français de Charles Bailleul. La distribution des lettres a été calculée sur la base du [Corpus bambara](http://cormand.tge-adonis.fr/).

- [Bamanakan v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/bambara-bamanakan-v1.dawg)

{{< space >}}

## Catalan {#ca}

Le DISC (Diccionari Informatitzat de l'Scrabble en Català) est une alternative libre au dictionnaire officiel en catalan (DOSC).{{< br >}} Pour plus de détails, consulter le [site dédié (en catalan)](http://diccionari.totescrable.cat/).

- [DISC 2.4.10](https://github.com/ipkiss42/eliot/releases/download/dictionaries/catalan-disc-v2.4.10.dawg)

{{< space >}}

## Espagnol {#es}

Le dictionnaire ci-dessous contient la liste officielle 2017 de la Federación Internacional de Léxico en Español ([FILE](https://www.filexico.com/)) :

- [FILE 2017](https://github.com/ipkiss42/eliot/releases/download/dictionaries/spanish-file-2017.dawg)

{{< space >}}

## Français {#fr}

- [french-unofficial-v1](https://github.com/ipkiss42/eliot/releases/download/dictionaries/french-unofficial-v1.dawg)

La liste de mots de ce dictionnaire contient des ajouts et omissions par rapport à l'Officiel du Scrabble (ODS 6), qui contient 386264 mots. Elle a été obtenue en combinant les sources suivantes :

- [Liste Français-Gutenberg](http://www.pallier.org/ressources/dicofr/dicofr.html)
- [Liste Moby pour le français](http://www.gutenberg.org/etext/3206)
- [Liste de mots de 2 à 4 lettres](http://www.motsduscrabble.com/)

**Note :** pour des raisons légales, la liste de mots de l'Officiel du Scrabble (ODS) n'est plus fournie sur ce site. Il est toutefois possible de trouver d'autres listes de mots sur Internet (y compris celle de l'ODS...) et de les utiliser pour [construire son propre dictionnaire](#own).

{{< space >}}

## Italien {#it}

Le dictionnaire ci-dessous contient la liste de mots officielle utilisée pour le Scrabble en italien. Les formes avec enclitique sont également acceptées en partie, mais il n'existe actuellement aucune liste avec toutes les formes valides.

- [Zingarelli 2011](https://github.com/ipkiss42/eliot/releases/download/dictionaries/italian-zingarelli-2011.dawg)

**Note :** les mots contenant les lettres J, K, W, X ou Y ne peuvent être joués qu'en utilisant les jokers.

{{< space >}}

## Malgache {#mg}

Le dictionnaire ci-dessous permet de jouer au Scrabble en malgache :

- [RAK](https://github.com/ipkiss42/eliot/releases/download/dictionaries/malagasy-rakibolana-v1.dawg)

{{< space >}}

## Norvégien {#no}

Le dictionnaire ci-dessous est basé sur la liste officielle, disponible [ici](http://www2.scrabbleforbundet.no/?attachment_id=1620) :

- [NSF](https://github.com/ipkiss42/eliot/releases/download/dictionaries/norwegian-nsf-2010.dawg)

{{< space >}}

## Polonais {#po}

Le dictionnaire ci-dessous est basé sur le SJP (Słownik Języka Polskiego) disponible [ici](http://www.sjp.pl/slownik/growy/) :

- [SJP](https://github.com/ipkiss42/eliot/releases/download/dictionaries/polish-sjp-2012.dawg)

{{< space >}}

## Roumain {#ro}

Le dictionnaire ci-dessous est basé sur la liste officielle, disponible [ici](http://www.scrabrom.3x.ro/LOC.html) :

- [LOC 5](https://github.com/ipkiss42/eliot/releases/download/dictionaries/romanian-loc-5.dawg)

{{< space >}}

## Serbe {#rs}

Il n'existe pas de liste de mots officielle pour jouer au Scrabble en serbe. Voici un dictionnaire non officiel :

- [Serbe](https://github.com/ipkiss42/eliot/releases/download/dictionaries/serbian-unofficial-v1.dawg)

{{< space >}}

## Tchèque {#cs}

- [czech-unofficial-v1.dawg](https://github.com/ipkiss42/eliot/releases/download/dictionaries/czech-unofficial-v1.dawg)

La liste de mots de ce dictionnaire a été obtenue en combinant les sources suivantes :

- [Liste non officielle](http://www.gtoal.com/wordgames/details/czech/czech.txt)
- [Liste des mots de 2 à 5 lettres](http://scrabble.hrejsi.cz/pravidla/blex.htm)

**Note :** les lettres respectent la distribution indiquée sur [cette page](http://hrejsi.cz/scrabble/pravidla.htm). Ainsi, les quelques mots contenant un W ou un Z doivent nécessairement être joués en utilisant les jokers.

## Créer son propre dictionnaire {#own}

Depuis la version 1.12 d'Eliot, un assistant permet de créer très facilement son propre dictionnaire à partir d'une liste de mots quelconque. L'assistant est disponible dans le menu *Paramètres* : *Créer un nouveau dictionnaire...*. Il suffit ensuite de se laisser guider...

**Note :** les langues utilisant 2 lettres pour un même jeton (comme le *LL* espagnol ou le *QU* catalan) doivent utiliser l'ancienne méthode indiquée ci-dessous.

Pour ceux qui préfèrent utiliser la ligne de commande, une autre méthode (plus complexe) consiste à utiliser le programme `compdic` fourni avec Eliot (et précompilé pour Windows).

Voici la procédure à suivre :

- Enregistrer la liste de mots dans un fichier (par exemple `mots.txt`), en utilisant l'encodage UTF-8
- Créer un fichier de définition des lettres (par exemple `lettres.txt`), également encodé en UTF-8. Chaque ligne du fichier correspond à une lettre, et contient les champs suivants (séparés par un ou plusieurs espaces) :
  - Lettre (pour le joker, utiliser un point d'interrogation)
  - Nombre de points
  - Fréquence (nombre d'occurrences)
  - 1 si la lettre est une voyelle, 0 sinon
  - 1 si la lettre est une consonne, 0 sinon
  
  Un exemple de fichier de lettres correspondant au Scrabble en français est disponible [ici](/download/letters.txt)
- Exécuter le programme `compdic` en utilisant la ligne de commande suivante :{{< br >}} `compdic -d "Nom du dictionnaire" -i mots.txt -l lettres.txt -o mon-dico.dawg`{{< br >}} Cela génère le fichier `mon-dico.dawg`, utilisable avec Eliot.
