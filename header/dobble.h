#ifndef DOBBLE_H
#define DOBBLE_H

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
