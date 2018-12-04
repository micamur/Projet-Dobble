#!/bin/bash

if hash montage &>/dev/null ; then
	COMMAND=(montage)
elif hash magick &>/dev/null ; then
	COMMAND=(magick montage)
else
	echo "ImageMagick n'est pas installé!" >&2
	echo "Installation :" >&2
	echo "  - Debian/Ubuntu" >&2
	echo "      sudo apt install imagemagick" >&2
	echo "  - CentOS/Fedora" >&2
	echo "      sudo yum install ImageMagick" >&2
	echo "  - Cygwin" >&2
	echo "      ImageMagick" >&2
	exit 1
fi

if [ "$@" = "" ]; then
	echo "Utilisation: ./Assembler90x90.sh OPEN_CLIP_ART/*" >&2
	exit 0
fi

FILENAME=AssemblageIcones.png

"${COMMAND[@]}" -background none "$@" \
		-geometry 90x90 \
		"$FILENAME"
RESULT=$?

if [ "$RESULT" = "0" ]; then
	echo "Images assemblées dans $FILENAME."
	exit 0
else
	echo "Echec de l'assemblage."
	exit $RESULT
fi
