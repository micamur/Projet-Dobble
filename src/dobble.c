#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "dobble-config.h"
#include "dobble.h"
#include "graphics.h"

/// Etat du compte à rebous (lancé/non lancé)
static bool timerRunning = false;


void onMouseMove(int x, int y)
{
	printf("dobble: Position de la souris: (%3d %3d)\r", x, y);
	fflush(stdout);
}


void onMouseClick()
{
	printf("\ndobble: Clic de la souris.\n");

	if (timerRunning)
	{
		printf("\ndobble: Arrêt du compte à rebours.\n");
		stopTimer();
		timerRunning = false;
	}
	else
	{
		printf("\ndobble: Démarrage du compte à rebours.\n");
		startTimer();
		timerRunning = true;
	}
}

void onTimerTick()
{
	printf("\ndobble: Tic du compte à rebours\n");
}


void renderScene()
{
	char title[100];

	// Efface le contenu de la fenêtre
	clearWindow();
	// Crée le titre qui sera affiché. Utile pour afficher le score.
	sprintf(title, "RICM3-Dobble    Score %d", 100);
	drawText(title, WIN_WIDTH / 2, 0, Center, Top);

	CardPosition currentCard = UpperCard;
	double radius = CARD_RADIUS * 0.6;
	double angle;
	double rotation;
	double scale = 1.;
	int icon = 0;
	int cx, cy;

	// Dessin du fond de carte de la carte supérieure
	drawCardShape(currentCard, 5, 252, 252, 252, 155, 119, 170);

	// Affichage des icônes de la carte du haut (régulièrement en cercle)
	for (angle = 0.; angle < 360.; angle += 360. / 7.)
	{
		rotation = sin(angle) * angle + 120.;

		drawIcon(currentCard, icon, radius, angle, rotation, scale, &cx, &cy);

		// (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
	}
	rotation = 120.;
	drawIcon(currentCard, 0, 0., angle, rotation, scale, &cx, &cy);

	// Dessin de la carte inférieure
	currentCard = LowerCard;

	// Dessin du fond de carte
	drawCardShape(currentCard, 5, 252, 252, 252, 155, 119, 170);

	// Affichage des icônes de la carte du bas (régulièrement en cercle)
	for (angle = 0.; angle < 360.; angle += 360. / 7.)
	{
		rotation = sin(angle) * angle + 70.;

		drawIcon(currentCard, 0, radius, angle, rotation, scale, NULL, NULL);
	}
	rotation = 70.;
	drawIcon(currentCard, 0, 0., angle, rotation, scale, NULL, NULL);

	// Met au premier plan le résultat des opérations de dessin
	showWindow();
}


int main(int argc, char **argv)
{

	if (!initializeGraphics())
	{
		printf("dobble: Echec de l'initialisation de la librairie graphique.\n");
		return 1;
	}

	if (loadIconMatrix(DATA_DIRECTORY "/Matrice8x10_Icones90x90.png") != 1)
	{
		printf("dobble: Echec du chargement des icônes.\n");
		return -1;
	}


	mainLoop();

	freeGraphics();


	return 0;
}
