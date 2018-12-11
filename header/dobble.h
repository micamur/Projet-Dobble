#ifndef DOBBLE_H
#define DOBBLE_H

typedef enum {
  FILE_ABSENT;
  INCORRECT_FORMAT;
} Error;

/**
 * Fonction qui
 */
void readCardFile(char* fileName);

typedef struct {
  int iconId; //Numero de l'icon
  double radius; //Distance entre le centre de la carte et le centre de dessin de l'icône.
  double angle; //Angle entre l'horizontale et la position de dessin de l'icône.
  double rotation; //Angle de rotation de l'icône par rapport à son centre.
  double scale; //Facteur d'échelle pour le dessin de l'icône.
  int center; //centre de l'icon

}Icon;

typedef struct {
  int nbIcons;
  Icon* icons;
}Card;

typedef struct {
  int nbIcons;
  int nbCards;
  Card* cards;
}Deck;

void initIcon();

void initCard(Card* card, int nbIcons);

void initDeck(Deck* deck, int nbCards, int nbIcons);

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
void onMouseClick();

/**
 * Fonction appelée chaque seconde par le compte à rebours lorsque celui-ci est
 * activé.
 */
void onTimerTick();

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

#endif /*DOBBLE_H*/
