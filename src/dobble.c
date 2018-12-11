#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "dobble-config.h"
#include "dobble.h"
#include "graphics.h"

/// Etat du compte à rebous (lancé/non lancé)
static bool timerRunning = false;

void initIcon(Icon* icon, int iconId, double radius, double angle, double rotation, double scale,
              int center){
	icon->iconId = iconId;
	icon->radius = radius;
	icon->angle = angle;
	icon->rotation = rotation;
	icon->scale = scale;
	icon->center = center;
}

void initCard(Card* card, int nbIcons, int incons[]){
	card->nbIcons = nbIcons;
	card->icones = malloc(sizeof(Icon)*nbIcon);
}

void initDeck(Deck* deck, int nbCards, int nbIcons){
	card->nbIcons = nbIcons;
	card->nbCards = nbCards;
	deck->cards = malloc(sizeof(Card)*nbCard);
}


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
	Card card1;
	Card card2;
	int i=rand()%(deck->nbCards);
	card1=deckGlobal->cards[i];
	do {
		int i=rand()%(deck->nbCards);
		card2=deckGlobal->cards[i];
	} while(card1=card2);

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
	int cx, cy;

	// Dessin du fond de carte de la carte supérieure
	drawCardShape(currentCard, 5, 252, 252, 252, 155, 119, 170);

	// Affichage des icônes de la carte du haut (régulièrement en cercle)
	int currentIcon = 0;
	for (angle = 0.; angle < 360.; angle += 360. / ((deckGlobal->nbIcons)-1).)
	{
		rotation = sin(angle) * angle + 120.;

		drawIcon(currentCard, card1.icones[currentIcon], radius, angle, rotation, scale, &cx, &cy);
		currentIcon++;
		// (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
	}
	rotation = 120.;
	drawIcon(currentCard, card1.icones[currentIcon], 0., angle, rotation, scale, &cx, &cy);

	// Dessin de la carte inférieure
	currentCard = LowerCard;

	// Dessin du fond de carte
	drawCardShape(currentCard, 5, 252, 252, 252, 155, 119, 170);

	// Affichage des icônes de la carte du bas (régulièrement en cercle)
	currentIcon=0;
	for (angle = 0.; angle < 360.; angle += 360. / ((deckGlobal->nbIcons)-1).)
	{
		rotation = sin(angle) * angle + 70.;

		drawIcon(currentCard, card2.icones[currentIcon], radius, angle, rotation, scale, NULL, NULL);
		currentIcon++;
	}
	rotation = 70.;
	drawIcon(currentCard, card2.icones[currentIcon], 0., angle, rotation, scale, NULL, NULL);

	// Met au premier plan le résultat des opérations de dessin
	showWindow();
}


int main(int argc, char **argv)
{
	srand(time(NULL));
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
