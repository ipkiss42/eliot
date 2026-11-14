---
title: 'Older news:'
aliases:
- /nouv.html
---

## April 07, 2013: Eliot 2.1

This release brings many new features.

The main changes include:

- New topping mode:
  - the goal is to find the top move as fast as possible
  - words can be tried freely
  - ergonomic interface, words can be typed quickly
  - various hints can be used to help the player
  - penalties can be configured
- New reports system (in [Python](https://en.wikipedia.org/wiki/Python_%28programming_language%29)), easily extensible. There are two reports at the moment (cf. [README](https://github.com/ipkiss42/eliot/blob/master/extras/reports/README.txt)):
  - stats.py: generates an HTML page similar to the statistics window in Eliot
  - sigles.py: generates a file usable with the SIGLES program (used in French Scrabble tournaments)
- New "master game" feature: allows replaying a game with the same racks and words as in the reference game (be it in the same mode or in another one)
- New option to detect if new versions of Eliot are available
- Arbitration mode:
  - new option to handle solos automatically
  - new search filter, filtering points
- Statistics:
  - added information
  - allow flipping the table
  - allow print preview and printing
- Ergonomy improvements:
  - new "visual" rack under the main board
  - tiles can be reordered using drag & drop in the visual rack
  - automatic preview (on the board) of the word being played
  - the timer can be used as a chronometer (using ctrl+click)
  - easier handling of players in the "New game" dialog
  - new "Print preview" menu entry
- Various:
  - listdic program: use getopt
  - saved games: much more information is stored
  - many bug fixes...

## October 14, 2012: Eliot 2.0

This major release introduces a lot of new features.

Among the main changes:

- New arbitration mode:
  - master move chosen by the arbitrator, or automatically
  - extremely efficient way to assign moves to players (very few keystrokes or mouse clicks, clever shortcuts)
  - possibility to go on playing even if a turn is incomplete
  - possibility to add a player during the game, or to change his/her table number
  - possibility to assign the same word to many players in one click
  - possibility to give solos, warnings and penalties to players
  - confirmation messages, to avoid common arbitration mistakes (can be deactivated)
- New "statistics" view: all the information about the game at a glance
- New presentation of the bag letters, much more visual and intuitive
- New "timer", to play with time constraints
- Favorite players: no need to create the same players every time
- Auto-save: the current game is saved automatically (useful in case of crash)
- More complete save game format
- Global ergonomics improvements:
  - more complete status bar (turn number, remaining letters)
  - more intuitive focus handling
  - less space lost
- New Spanish translation
- Many bug fixes

Note: due to the big internal changes, save games created with a previous version of Eliot are not compatible with this new version.

## January 04, 2012: Eliot 1.14

Changes:

- New game variant: 7 among 8
- New preference to define a default AI level
- Bug fix in the dictionary creation wizard

## August 21, 2011: Eliot 1.13

Changes:

- Display the definition of a word in a web browser
- Display the points of a word before playing it
- Allow exporting the dictionary words list to a file
- Training mode: enable hiding the words and/or positions of search results
- New translations: Serbian and Indonesian
- Several bugs fixes

## May 22, 2010: Eliot 1.12

Changes:

- New dictionary creation wizard, allowing to create easily a custom dictionary from a list of words
- Czech translation of the software
- A few bug fixes

## May 08, 2010: ODS unavailable

Due to a claim from Editions Larousse, the ODS word list is not available anymore on Eliot web site (since 04/21/2010).{{< br >}} An alternative dictionary is present in the downloads section.

## February 17, 2010: Eliot 1.11b

This new version brings some changes to work under Mac OS X. There is no impact on the Linux and Windows versions.

## January 30, 2010: Eliot 1.11a

New features:

- Fixed a bug when saving a game (occurring only on Windows)

## January 24, 2010: Eliot 1.11

New features:

- Support for saving/loading games in any game mode (XML format).{{< br >}} Note: older savegames can still be loaded with this version, but the next version will only support the new format.
- Various encoding fixes

## September 26, 2009: Eliot 1.10

New features:

- Support for tiles made of multiple characters in the dictionary (useful for Spanish and Catalan, in particular)
- Translation of the interface to Catalan

## February 21, 2009: Eliot 1.9

Enhancements:

- Joker games work again (version 1.8 was buggy)
- In training mode, a word can be entered manually, without doing a search
- The coordinates of the word to play can be entered by clicking on the board (left click to set the coordinates, right click to clear them)

## January 25, 2009: Eliot 1.8

This version brings several enhancements:

- It is possible to navigate in the history of the game, and resume from any turn
- The search is much faster, even with jokers in the rack
- Tiles points are displayed on the board
- A new game variant is available: "explosive" games
- There are a few GUI enhancements

## December 7, 2008: New Website

The Eliot website has been completely redesigned, thanks to the work of Xavier Rousselot.

## October 18, 2008: Eliot 1.7a

This version corrects a pretty annoying bug, which was introduced in the previous version: the search could miss some solutions when the rack contained a joker. Thanks to Christian Pierre for detecting the problem.

## September 27, 2008: Eliot 1.7

There are two main novelties in this new version:

- The graphical user interface has been entirely rewritten, and should be more intuitive. The older one is still available, but it will probably disappear in the next version of Eliot. If you think that something is missing in the new interface, please tell us!
- Three different types of game are now available:
  - Training mode: the mode already present in the previous versions of Eliot
  - Duplicate mode: a multiplayer game (with human or computer players), in which all the players have the same rack
  - Free game mode: this is the classical Scrabble game, in which the players play by turn

It is also possible to play joker games.

Note: for now, saving games is only possible in the training mode.

## 13 janvier 2008 : Eliot 1.6

La principale nouveauté avec Eliot 1.6 est la possibilité de créer des dictionnaires pour des langues autres que le français et de jouer avec. Cette version corrige également de nombreux bugs.

## 19 novembre 2007 : ODS5

Après plusieurs vérifications et tests il semble que la version actuelle de l'ODS5 pour Eliot corresponde à la version officielle (378989 mots). Merci à tous ceux qui ont participé à la construction et à la vérification du dictionnaire (et en particulier à Marcel Jolly et Hughes Damry).

## 12 novembre 2007 : ODS5 et Eliot 1.5

{{< plain >}}
Une première version de l'ODS5 est disponible pour Eliot. D'après son auteur, Marcel Jolly, cette version peut encore contenir quelques coquilles mais devrait s'approcher assez fidèlement de la version officielle. Eliot 1.5 est maintenant disponible en test. Nous vous invitons à utiliser cette nouvelle version également.
{{< /plain >}}

## 22 avril 2006 : Eliot avec MacOSX

{{< plain >}}
Grâce au travail de schlum, Eliot fonctionne maintenant sur MaxOSX (ppc et x86). Une version avec un installeur et un binaire fonctionnant sur les deux plateformes devrait être disponible sous peu. Pour plus de renseignements contactez [schlum](mailto:schlum@macbidouille.com). La version disponible est la version CVS du projet. Une capture d'écran est disponible [ici](/img/eliot-osx.jpg).
{{< /plain >}}

## 30 juillet 2005: Eliot version 1.5.2

Quelques corrections pour la dernière version qui était sortie un peu rapidement. Le comptage des points a été corrigé (merci à Nicolas et Olivier). Le chargement/sauvegarde des parties est corrigé ainsi que l'affichage de la grille sous Windows.

## 28 mai 2005: Eliot version 1.5

Une nouvelle version d'Eliot est disponible pour Linux et pour Windows. Les plus grosse modifications ne sont pas visibles et concernent le moteur du jeu. Ces modifications permettrons dans une prochaine version de mettre en place de nouveaux modes de jeux. Pour ce qui est de l'interface vous pourrez découvrir la recherche dans le dictionnaire à l'aide d'expressions rationnelles ainsi qu'une nouvelle interface *ncurses* qui contient déjà les différents modes de jeux (duplicate, partie libre, entrainement, jeu à plusieurs, parties joker, ...).

## 26 mars 2005: déplacement du site

Le site web d'Eliot a été déplace sur le serveur de projet [Savannah](/).

## 5 octobre 2003: correction de l'ODS 4

La version précédente du dictionnaire comportait des mots de plus de 15 lettres et 2 entrées incorrectes (SEPTICOPYOEMIE(S)). Merci à Philippe Chataignon pour sa version corrigée du dictionnaire.

## 21 septembre 2003: ODS 4

L'ODS 4 est disponible pour Eliot. Merci Fabien !!

## 8 mars 2003: Eliot 1.3

Les binaires du 2 mars étaient encore en édition de liens dynamique sur une partie : [mingwm10.dll](https://www.nongnu.org/eliot/download/mingwm10.dll) Pour plus d'informations voir le site [wingw](http://www.mingw.org/). Si vous avez pris ces binaires il vous suffit de rajouter la dll dans le même dossier qu'eliot pour le faire fonctionner. Sinon, les binaires ont été mis à jour et vous pouvez les recharger.

## 2 mars 2003: Eliot 1.3

Les binaires ont été mis à jour sur le site. La version d'hier utilisait wxWindows en édition des liens dynamique (DLL).

## 1 mars 2003: Eliot 1.3

Merci beaucoup à Olivier Teulière pour les nombreuses corrections sur le code d'eliot.

- effacement de résultats de recherche lors du chargement d'une partie
- corrections sur la vérification de la validité des tirages
- le 'Y' et les jokers comptent désormais comme des voyelles et des consonnes

En plus de ces modifications, l'adaptation pour la compilation avec la nouvelle version de wxWindows (2.4.0) a été faite. Cette version doit pouvoir être compilée sous MacOSX sans modification du code source d'eliot. Je n'ai pas de Mac à disposition pour effectuer la compilation, si quelqu'un peut prendre en charge la compilation je serai heureux de proposer la version sur le site.

## 16 janvier 2001: Eliot 1.1.2

Correction d'un bug sur les scores. Les mots en appuis vertical sur un joker comptaient les points de la lettre blanche. Merci à Jean-Luc Grevesse pour avoir levé l'erreur.

## 21 déc 2000: Eliot 1.1.1

Modifications et nettoyage du code C. Pas de changement visible dans le programme sauf sur l'affichage de la liste des résultats.

## 12 déc 2000: Eliot 1.1

Eliot dispose maintenant d'un panneau de recherche ([captures](/img/recherche.jpg))

- recherche des anagrammes avec n et n+1 lettres (comme un 7 plus 1 en fait)
- recherche avec des contraintes (idéale pour les mots croisés)

Une première version du [manuel](https://www.nongnu.org/eliot/en/intro/) est aussi disponible.

les 15 premiers tirages peuvent être vérifiés selon les règles officielles (au moins 2 voyelles et 2 consonnes par tirage)

pas mal de ménage dans le code du programme.

## 10 déc 2000: Rien de spécial aujourd'hui.

## 7 déc 2000: Correction du dictionnaire

Il y avait de nombreuses erreurs dans la version précédente du dictionnaire. La présente version contient les corrections et doit être correcte et complète par rapport à l'ODS3. Merci à Patrick Jenty pour m'avoir signalé le problème.

## 4 déc 2000: Eliot 1.0.1

Des modifications légères : un peu de ménage dans les sources et quelques corrections dans l'interface wxWindows. Les programmes pour construire son propre dictionnaire sont disponibles en version compilée.

## 28 nov 2000: appel à participation

Je n'ai pas de Mac disponible pour faire le portage d'Eliot. Il faut pouvoir disposer de MacOS 8.5 et de CodeWarrior 4.0. wxWindows pour Mac semble avoir un peut de retard par rapport aux versions Unix et Windows®, certaines modifications seront sans doute nécessaires pour compiler en attendant la version 2.2. Quelqu'un est-il intéressé pour faire le portage ?

## 7 nov 2000: Eliot version 1.0

Eliot est (enfin) disponible pour Linux ET Windows®.
