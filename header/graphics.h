#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef enum { UpperCard,
	       LowerCard } CardPosition;

typedef enum { Top,
	       Middle,
	       Bottom } VAlign;

typedef enum { Left,
	       Center,
	       Right } HAlign;

/****************** METHODES A IMPLEMENTER ******************/
/*********** Voir graphics.c pour l'implémentation **********/

/**
 * Retourne la position d'un icône donné en paramètre dans la matrice d'icônes utilisée par le programme.
 *
 * @param iconId  Numéro de l'icône dont retourner les coordonnées
 * @param posX    Pointeur pour retourner la coordonnée X du coin supérieur gauche de l'icône (en pixels)
 * @param posY    Pointeur pour retourner la coordonnée Y du coin supérieur gauche de l'icône (en pixels)
 */
void getIconLocationInMatrix(int iconId, int *posX, int *posY);

/****************** METHODES UTILITAIRES ******************/

/**
 * Retourne les coordonnées du centre de la carte fournie en paramètre.
 *
 * @param card         Carte dont retourner le centre.
 * @param cardCenterX  Pointeur vers la variable recevant la coordonnée X du centre
 * @param cardCenterY  Pointeur vers la variable recevant la coordonnée Y du centre
 */
void getCardCenter(CardPosition card, int *cardCenterX, int *cardCenterY);

/**
 * Planifie l'appel de la procédure donnée en paramètre.
 *
 * @param method  Méthode dont la signature est void méthode(void*) (pas de valeur de retour, mais un paramètre de type void*).
 * @param param   Paramètre à passer à la méthode lors de son appel.
 * @param delay   Temps (en millisecondes) avant l'appel de la méthode.
 */
void callLater(void (*method)(void *), void *param, Uint32 delay);

/****************** METHODES DE GESTION DU TIMER ******************/

/**
 * Démarre le compte à rebours : après l'appel à cette fonction, la fonction
 * onTimer sera appelée régulièrement par la SDL.
 */
void startTimer();

/**
 * Arrête le compte à rebours : après l'appel à cette fonction, la fonction
 * onTimer ne sera plus appelée régulièrement par la SDL.
 */
void stopTimer();

/****************** METHODES DE CHARGEMENT ******************/

/**
 * loadIconMatrix lit une image depuis le disque pour l'utiliser avec la SDL.
 * Cette image sera utilisée comme matrice d'icônes pour le dessin avec la
 * méthode drawIcon.
 *
 * @param  fileName Chemin d'accès au fichier d'image
 * @return          1 si l'image a été chargée correctement, 0 sinon
 */
int loadIconMatrix(const char *fileName);

/****************** METHODES DE DESSIN ******************/

/**
 * Efface le contenu de la fenêtre avant une nouvelle passe de dessin.
 */
void clearWindow();

/**
 * Affiche le résultat des dernières opérations de dessin (depuis le dernier
 * showWindow) au premier plan.
 */
void showWindow();

/**
 * Notifie la boucle d'évènements que le rendu de la fenêtre n'est plus valide
 * et doit donc être effectué dès que possible.
 */
void requestRedraw();

/**
 * Affiche le texte donné en paramètre à la position indiquée. Le texte
 * sera aligné par rapport à la position indiquée en fonction des paramètres
 * d'alignement.
 *
 * @param  message Texte à utiliser pour le titre du jeu
 * @param  x       Coordonnée x du point de dessin du texte
 * @param  y       Coordonnée y du point de dessin du texte
 * @param  hAlign  Alignement horizontal
 * @param  vAlign  Alignement vertical
 * @return         1 si le dessin a réussi, 0 sinon.
 */
int drawText(const char *message, int x, int y, HAlign hAlign, VAlign vAlign);

/**
 * Remplit un disque d'une couleur donnée.
 *
 * @param x0     Coordonnée X du centre du cercle (en pixels)
 * @param y0     Coordonnée Y du centre du cercle (en pixels)
 * @param radius Rayon du cercle (en pixels)
 * @param fr     Valeur R de la couleur de remplissage
 * @param fg     Valeur G de la couleur de remplissage
 * @param fb     Valeur B de la couleur de remplissage
 * @param fa     Valeur A de la couleur de remplissage
 */
void fillCircle(int x0, int y0, int radius, Uint8 fr, Uint8 fg, Uint8 fb, Uint8 fa);

/**
 * Dessine un cercle d'une couleur donnée.
 *
 * @param x0     Coordonnée X du centre du cercle (en pixels)
 * @param y0     Coordonnée Y du centre du cercle (en pixels)
 * @param radius Rayon du cercle (en pixels)
 * @param fr     Valeur R de la couleur de dessin
 * @param fg     Valeur G de la couleur de dessin
 * @param fb     Valeur B de la couleur de dessin
 * @param fa     Valeur A de la couleur de dessin
 */
void drawCircle(int x0, int y0, int radius, Uint8 fr, Uint8 fg, Uint8 fb, Uint8 fa);

/**
 * Affiche le fond et le contour d'une carte de jeu.
 *
 * @param  card     Carte à dessiner
 * @param  w        Epaisseur du contour
 * @param  bgr      Valeur R de la couleur d'arrière-plan
 * @param  bgg      Valeur G de la couleur d'arrière-plan
 * @param  bgb      Valeur B de la couleur d'arrière-plan
 * @param  fgr      Valeur R de la couleur de contour
 * @param  fgg      Valeur G de la couleur de contour
 * @param  fgb      Valeur B de la couleur de contour
 */
void drawCardShape(CardPosition card, int w, Uint8 bgr, Uint8 bgg, Uint8 bgb, Uint8 fgr, Uint8 fgg, Uint8 fgb);

/**
 * drawIcon dessine un icône dans la carte spécifiée. L'emplacement de
 * l'icône est donnée en coordonnées polaires par rapport au centre de la carte.
 *
 * @param card      Indique si l'icône doit être dessiné dans la carte du
 *                  haut ou la carte du bas.
 * @param iconId    Numéro de l'icône à dessiner. Ce numéro est converti en
 *                  position dans la matrice d'image par la méthode
 *                  PositionIconeDansImage.
 * @param radius    Distance entre le centre de la carte et le centre de
 *                  dessin de l'icône.
 * @param angle     Angle (sens horaire) entre l'horizontale et la position
 *                  de dessin de l'icône. 0 correspond à la droite du
 *                  centre de la carte, 90 à la verticale en dessous du
 *                  centre de la carte.
 * @param rotation  Angle de rotation (en degrés) de l'icône par rapport à
 *                  son centre.
 * @param scale     Facteur d'échelle pour le dessin de l'icône. 1.0
 *                  correspond à la taille originale (ICON_SIZE) des icônes.
 *                  2.0 doublerait la taille du dessin.
 * @param centerX   Si différent de NULL, pointeur vers la variable qui
 *                  recevra le centre calculé du dessin de l'icône (X).
 * @param centerY   Si différent de NULL, pointeur vers la variable qui
 *                  recevra le centre calculé du dessin de l'icône (Y).
 */
void drawIcon(CardPosition card, int iconId, double radius, double angle, double rotation, double scale,
              int *centerX, int *centerY);

/****************** METHODES DE GESTION DU CYCLE DE VIE ******************/

/**
 * Initialise les bibliothèques SDL, SDL_image et SDL_ttf, et crée la fenêtre,
 * les textures et les polices utilisées par le programme.
 *
 * Cette méthode doit être appelée avant les autres appels graphiques
 * (chargement d'images, dessin, timers, etc.), et la fin du programme doit
 * appeler freeGraphics() pour libérer les ressources correspondantes.
 *
 * @return 1 si la SDL a été initialisée correctement, 0 sinon.
 */
int initializeGraphics();

/**
 * Attend qu'un évènement utilisateur soit émis, puis traite l'évènement.
 *
 * Le traitement des évènements de la souris est délégué aux méthodes
 * onMouseMove, onMouseClick et renderScene.
 */
void mainLoop();

/**
 * Libère les ressources graphiques utilisées par la bibliothèque SDL.
 */
void freeGraphics();

#endif /*GRAPHICS_H*/
