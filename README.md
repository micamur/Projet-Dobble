# 愛\&雪 Dobble

Nous avons réalisé ce jeu dans le cadre du cours d'Algorithmique et Programmation Impérative (API) de Polytech Grenoble, filière INFO3 (année 2018-2019).

## Règles du jeu

Au départ, **sélectionnez un pack d'icônes** en cliquant dessus, puis **sélectionnez le nombre d'icônes** par carte que vous souhaitez, la fenêtre de jeu s'ouvrira après que vous ayez choisi.

Vous démarrez alors avec **30 secondes** de jeu, votre but est d'avoir le plus haut score possible. Pour gagner des points, cliquez sur l'icône de la carte du haut qui se trouve également sur la carte du bas.

- Si vous cliquez sur le bon icône vous gagnez **1 point** et **3 secondes** et passez au couple de cartes suivant
- Si vous cliquez ailleurs sur la carte du haut vous perdez **3 secondes** et passez au couple de cartes suivant
- Si vous cliquez ailleurs rien ne se passe

## Installation et compilation

Installation des outils nécessaires :
```bash
# DEBIAN/UBUNTU : Installation des outils nécessaires
$ sudo apt install clang cmake imagemagick libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

# CENTOS/FEDORA : Installation des outils nécessaires
$ sudo yum install clang cmake ImageMagick SDL2-devel SDL2_image-devel SDL2_ttf-devel

# CYGWIN : Paquets nécessaires à installer avec cyg-get ou Chocolatey
# Notez qu’un serveur X11 doit aussi être installé
# et en cours d’exécution pour faire fonctionner le projet (paquet xinit).
clang cmake ImageMagick libSDL2-devel libSDL2_image-devel libSDL2_ttf-devel
```

Pour compiler et lancer le jeu, lancez les commandes suivantes dans votre terminal :

```bash
# Vous devez être placé dans le dossier du projet
$ ls
cmake  CMakeLists.txt  data  header  README.md  src
# Choix du compilateur pour le projet
$ export CC=/usr/bin/clang
# Création du dossier
$ mkdir build
# On se place dans le dossier pour la compilation
$ cd build/
# Génération des fichiers Makefile
$ cmake ..
# Compilation du projet
$ make
# Exécution du projet
$ ./dobble
```

## Sources des packs d'icônes utilisés

- Cœur : https://emojipedia.org/
- Flocon : https://www.flaticon.com/packs/snowflakes
- Food : https://www.flaticon.com/packs/gastronomy-set
