# Système de gestion de fichiers avec tags
___

Projet de L3 dans le cadre du cours sur les systèmes d'exploitation.

Création d'une couche par-dessus le système de gestion de fichiers permettant de gérer les fichiers grâce à des tags.Un tag est une chaîne de caractères alphanumériques qui permet de donner un renseignement sur un fichier.

## Installation
Le système s'installe en exécutant le script `install.sh`.

## Fonctionnalités implémentées

**Créer** un ou plusieurs tags, en spécifiant éventuellement leur père avec l’option -f :
```
tag create [-f <tag>] tagname [tagname] …
```

**Supprimer** un tag (et ses enfants) de la liste des tags (et donc de tous les fichiers):
```
tag delete <tag>
```

**Afficher** la liste des tags et leur hiérarchie :
```
tag print
```

**Associer** un ou plusieurs tags à un fichier :
```
tag link <file> <tag> [tag] ...
```

**Supprimer** un ou plusieurs tags d’un fichier :
```
tag unlink <file> <tag> [tag] ...
```

**Supprimer** tous les tags d’un fichier :
```
tag unlink <file> --all
```

**Afficher** les tags associés à un fichier :
```
tag print <file>
```

**Rechercher** les fichiers correspondant à une combinaison de tags
```
tag search tag [[-not] tag] ...
```

**Remettre à zéro** le système en supprimant tous les tags de tous les fichiers et en supprimant la liste de tags créés jusqu’alors.
```
tag reset
```

## Désinstaller le système

Pour désinstaller le système, exécuter le script `uninstall.sh`.

## Auteurs :

- CASTELBOU Louis
- CHEKROUN Hiba-Asmaa
- MIRET Blanche
