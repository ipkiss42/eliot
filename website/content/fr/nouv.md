---
title: Archives des nouveautés anciennes
aliases:
- /nouv.html
---

Ces annonces sont conservées à titre historique. Pour les versions publiées, consultez [Eliot sur GitHub](https://github.com/ipkiss42/eliot/releases).


## 07 avril 2013 : Eliot 2.1

Cette version apporte beaucoup de nouvelles fonctionnalités.

Parmi les principaux changements :

- Nouveau mode "topping" :
  - le but est de trouver le top le plus rapidement possible
  - les mots peuvent être essayés librement
  - interface ergonomique, on peut taper les mots très rapidement
  - de nombreux; indices peuvent être utilisés pour aider le joueur
  - les pénalités sont configurables
- Nouveau système de rapports (en [Python](https://fr.wikipedia.org/wiki/Python_%28langage%29)), facilement extensible. Deux rapports existent pour l'instant (cf. [README](https://github.com/ipkiss42/eliot/blob/main/extras/reports/README.txt), en anglais) :
  - stats.py : génère une page HTML similaire à la fenêtre des statistiques
  - sigles.py : génère un fichier utilisable par le programme SIGLES (utilisé dans certains tournois)
- Nouvelle fonctionnalité "partie maîtresse" : permet de rejouer une partie avec les mêmes tirages et les mêmes mots que la partie de référence (et ce dans le même mode de jeu, ou dans un mode différent)
- Nouvelle option pour détecter si de nouvelles versions d'Eliot sont disponibles
- Mode arbitrage:
  - nouvelle option pour gérer automatiquement les solos
  - nouveau filtre de recherche, pour filtrer les résultats par score
- Statistiques:
  - ajout de nouvelles informations
  - possibilité d'inverser la table
  - possibilité d'imprimer la table et de prévisualiser l'impression
- Améliorations de l'ergonomie :
  - nouveau chevalet "visuel" sous le plateau de jeu principal
  - les lettres peuvent être réordonnées par drag & drop dans le chevalet visuel
  - prévisualisation automatique (sur le plateau) du mot joué
  - le minuteur peut être utilisé comme un chronomètre (avec ctrl+clic)
  - gestion des joueurs facilitée dans la fenêtre "Nouvelle partie"
  - nouvelle entrée de menu "Aperçu avant impression"
- Divers :
  - le programme "listdic" utilise getopt
  - sauvegardes : beaucoup plus d'informations sont stockées
  - nombreuses corrections de bugs...

## 14 octobre 2012 : Eliot 2.0

Cette release majeure apporte beaucoup de nouvelles fonctionnalités.

Parmi les principaux changements :

- Nouveau mode d'arbitrage :
  - coup maître choisi par l'arbitre, ou bien automatiquement
  - affectation des coups aux joueurs extrêmement pratique (très peu de clics ou de touches à taper, raccourcis intelligents)
  - possibilité de continuer à jouer même si le tour est incomplet
  - possibilité d'ajouter un joueur pendant la partie, ou de changer son numéro de table
  - possibilité d'affecter le même coup à plusieurs joueurs en un seul clic
  - possibilité de donner aux joueurs des solos, avertissements et pénalités
  - messages de confirmation (désactivables) pour éviter des erreurs d'arbitrage classiques
- Nouvelle fenêtre de statistiques, pour voir en un clin d'œil toutes les informations sur la partie
- Nouvelle présentation des lettres du sac, plus visuelle et intuitive
- Nouveau "minuteur", pour jouer avec des contraintes de temps
- Joueurs favoris : plus besoin de créer les mêmes joueurs à chaque fois
- Sauvegarde automatique : la partie en cours est automatiquement sauvegardée (utile en cas de crash)
- Améliorations de l'ergonomie générale :
  - barre d'état plus complète (numéro du tour, lettres restantes)
  - gestion du focus plus intuitive
  - moins de place perdue
- Format de sauvegarde plus complet
- Traduction du programme en espagnol
- Nombreuses corrections de bugs

Remarque : les sauvegardes créées avec une version antérieure d'Eliot ne sont pas compatibles avec cette nouvelle version.

## 4 janvier 2012 : Eliot 1.14

Nouveautés :

- Nouvelle variante de jeu : 7 sur 8
- Nouvelle préférence pour définir un niveau d'IA par défaut
- Correction d'un bug dans l'assistant de création de dictionnaire

## 21 août 2011 : Eliot 1.13

Nouveautés :

- Affichage de la définition d'un mot dans un navigateur web
- Affichage des points d'un mot avant de le jouer
- Possibilité d'exporter la liste des mots du dictionnaire dans un fichier
- Mode entraînement : possibilité de cacher les mots et/ou les positions des résultats de recherche
- Nouvelles traductions : serbe et indonésien
- Plusieurs corrections de bugs

## 22 mai 2010 : Eliot 1.12

Nouveautés :

- Nouvel assistant de création de dictionnaire, permettant de créer facilement son propre dictionnaire à partir d'une liste de mots
- Traduction du logiciel en Tchèque
- Quelques corrections de bugs

## 08 mai 2010 : ODS indisponible

Suite à une plainte des Éditions Larousse, la liste de mots de l'ODS n'est plus disponible sur le site d'Eliot (depuis le 21/04/2010).{{< br >}} Un dictionnaire de remplacement est proposé dans la section de téléchargements.

## 17 février 2010 : Eliot 1.11b

Cette version apporte quelques corrections pour fonctionner sous Mac OS X. Il n'y a aucun impact pour les versions Linux et Windows.

## 30 janvier 2010 : Eliot 1.11a

Nouveautés :

- Correction d'un bug (sous Windows uniquement) lors de la sauvegarde de parties

## 24 janvier 2010 : Eliot 1.11

Nouveautés :

- Possibilité de sauvegarder/charger une partie, dans tous les modes de jeu (au format XML).{{< br >}} Note : les anciens fichiers de sauvegarde peuvent toujours être chargés avec cette version, mais à partir de la prochaine version seul le nouveau format sera supporté.
- Correction de plusieurs problèmes d'encoding

## 26 septembre 2009 : Eliot 1.10

Nouveautés :

- Support des lettres composées de plusieurs caractères dans le dictionnaire (utile pour l'Espagnol et le Catalan en particulier)
- Traduction de l'interface en Catalan

## 21 février 2009 : Eliot 1.9

Améliorations :

- Les parties joker fonctionnent à nouveau (la version 1.8 avait un bug)
- Il est possible de rentrer manuellement un mot en mode entraînement, sans faire de recherche
- Les coordonnées du mot à jouer peuvent être entrées en cliquant sur la grille (clic gauche pour sélectionner, clic droit pour effacer)

## 25 janvier 2009 : Eliot 1.8

Plusieurs améliorations dans cette version :

- Il est possible de naviguer dans l'historique de la partie, et de reprendre depuis n'importe quel coup
- La recherche est plus rapide, même lorsque des jokers sont présents dans le tirage
- Les points des jetons sont affichés sur le plateau de jeu
- Une nouvelle variante est disponible : les "parties détonantes"
- Il y a également quelques améliorations dans l'interface

## 07 décembre 2008 : Nouveau site web

Le site d'Eliot a été complètement refait, grâce au travail de Xavier Rousselot.

## 18 octobre 2008 : Eliot 1.7a

Cette version corrige un bug assez gênant introduit dans la version précédente : la recherche peut passer à côté de certaines solutions lorsque le tirage contient un joker. Merci à Christian Pierre pour avoir détecté le problème.

## 27 septembre 2008 : Eliot 1.7

Cette nouvelle version contient 2 grandes nouveautés :

- L'interface a été entièrement refaite, elle est un peu plus intuitive. L'ancienne interface est encore disponible, mais elle va sûrement disparaître dans la prochaine version d'Eliot. Si quelque chose vous manque avec la nouvelle interface, n'hésitez pas à le dire !
- Il est désormais possible de choisir entre 3 modes de jeu :
  - Mode entraînement : il s'agit du mode présent dans les anciennes versions d'Eliot
  - Mode duplicate : pour plusieurs joueurs (humains ou contrôlés par l'ordinateur), tous les joueurs ayant le même tirage
  - Mode partie libre : c'est le mode "familial", où les joueurs jouent à tour de rôle

Il est également possible de jouer des parties joker.

Note : la sauvegarde de parties n'est possible qu'en mode entraînement pour l'instant.

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

Une première version du [manuel](https://www.nongnu.org/eliot/fr/intro/) est aussi disponible.

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
