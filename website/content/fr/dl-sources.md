---
type: installation
title: Compiler Eliot à partir des sources
aliases:
- /dl-sources.html
- /dl-others/
- /dl-others.html
---

Pour un système sans paquet précompilé ou pour une compilation personnalisée, compilez Eliot à partir des sources. Choisissez ci-dessous l’archive de la version publiée ou la version de développement.

## Eliot 2.2

Téléchargez l’archive du code source de la version publiée : [eliot-2.2.tar.gz](https://dl.sv.nongnu.org/releases/eliot/releases/2.2/eliot-2.2.tar.gz).

Décompressez l’archive et suivez le fichier `INSTALL` qu’elle contient. Il décrit les dépendances et les commandes de compilation de cette version.

## Version de développement

Le code de développement est disponible sur [GitHub](https://github.com/ipkiss42/eliot). Pour le récupérer avec Git :

```sh
git clone https://github.com/ipkiss42/eliot.git
cd eliot
```

Lisez le fichier `INSTALL` de cette copie avant de compiler. Les versions publiée et de développement ont des dépendances différentes ; la [page Debian/Ubuntu](/fr/dl-debian/#dépendances-de-la-version-de-développement-actuelle) indique les dépendances de compilation du code actuel pour ces systèmes.

## Anciennes versions

Les anciennes archives du code source se trouvent dans les [archives des versions sur Savannah](http://dl.sv.nongnu.org/releases/eliot/releases/).

## Autres paquets

Si vous connaissez un paquet précompilé pour un autre système, [signalez-le à la liste de discussion eliot-dev](/fr/help/#eliot-dev) pour que nous puissions ajouter un lien.

{{< installation-next build=true >}}
