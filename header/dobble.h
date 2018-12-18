#ifndef DOBBLE_H
#define DOBBLE_H

typedef enum {
        FILE_ABSENT,
        INCORRECT_FORMAT
} Error;

typedef struct {
        int iconId;       // Numéro de l'icône.
        double radius;    // Distance entre le centre de la carte et le centre de dessin de l'icône.
        double angle;     // Angle entre l'horizontale et la position de dessin de l'icône.
        double rotation;  // Angle de rotation de l'icône par rapport à son centre.
        double scale;     // Facteur d'échelle pour le dessin de l'icône.
        int centerX;      // Position x du centre de l'icône.
        int centerY;      // Position y du centre de l'icône.
} Icon;

typedef struct {
        int nbIcons;
        Icon* icons;
} Card;

typedef struct {
        int nbIcons;
        int nbCards;
        Card* cards;
} Deck;

#include "graphics.h"

/**
 * Affiche à l'utilisateur une erreur donnée et quitte le programme
 *
 * @param error Le code d'erreur
 */
void printError(Error error);

/**
 * Initialise un deck vide
 *
 * @param nbCards Le nombre de cartes du deck
 * @param nbIcons Le nombre d'icônes par carte
 */
void initDeck(int nbCards, int nbIcons);

/**
 * Initialise une carte vide
 *
 * @param card    Le pointeur cers la carte
 * @param nbIcons Le nombre d'icônes par carte
 * @param icons   Le tableau des identifiants des icônes de la carte
 */
void initCard(Card* card, int nbIcons, int icons[]);

/**
 * Lit un fichier contenant les icônes des cartes du jeu
 *
 * @param fileName Le nom du fichier
 */
void readCardFile(char const *fileName);

/**
 * Initialise aléatoirement une icône donnée (radius, rotation, scale)
 *
 * @param icon  L'icône courante
 * @param angle Son angle (dépend de son ordre dans la liste d'icônes de sa carte)
 */
void initIcon(Icon *icon, double angle);

/**
 * Initialise aléatoirement les icônes d'une carte donnée
 *
 * @param currentCard La carte courante
 */
void initCardIcons(Card currentCard);

/**
 * Fonction appelée lors d'un mouvement du curseur de la souris sur la fenêtre.
 * L'origine des coordonnées est le coin supérieur gauche de la fenêtre.
 *
 * @param x Abscisse du curseur de la souris
 * @param y Ordonnée du curseur de la souris
 */
void onMouseMove(int x, int y);

/**
 * Fonction appelée lorsqu'un bouton de la souris est enfoncé.
 */
void onMouseClick(int mouseX, int mouseY);

/**
 * Fonction appelée chaque seconde par le compte à rebours lorsque celui-ci est
 * activé.
 */
void onTimerTick();

/**
 * Sélectionne deux cartes aléatoires différentes des deux précédentes
 */
void changeCards();

/**
 * Fonction qui mélange de manière aléatoire l'ordre des icônes d'une carte
 *
 * @param elems   Le tableau d'éléments à mélanger
 * @param nbElems Le nombre d'éléments du tableau
 */
void shuffle(Icon *elems, int nbElems);

/**
 * Fonction qui dessine une carte
 *
 * @param currentCardPosition La position de la carte (haut ou bas)
 * @param currentCard         La carte à dessiner
 */
void drawCard(CardPosition currentCardPosition, Card currentCard);

/**
 * renderScene calcule ce qui va être affiché ensuite à l'écran. Toutes
 * les opérations de dessin sont effectuées en arrière-plan, puis l'appel à
 * showWindow() affiche le résultat de ces opérations en une seule étape.
 *
 * Cette méthode permet d'éviter le "flickering" (clignotement des éléments
 * dessinés à l'écran plusieurs fois par seconde) qui résulterait d'un dessin
 * direct à l'écran.
 */
void renderScene();

void afficheMenuFin();

void afficheStat();

void afficheBouton();

#endif /*DOBBLE_H*/
