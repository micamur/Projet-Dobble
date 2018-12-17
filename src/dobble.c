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

Deck deckGlobal;
Card card1;
Card card2;

void printError(Error error) {
	switch (error) {
		case FILE_ABSENT:
			fprintf(stderr, "Failed to open the file\n");
		case INCORRECT_FORMAT:
			fprintf(stderr, "Incorrect file format\n");
	}
	exit(error);
}

void readCardFile(char* fileName) {
	FILE* data = fopen(fileName, "r");

	if (data == NULL)
		printError(FILE_ABSENT);

	int nbCards, nbIcons;

	fscanf(data, "%d %d", &nbCards, &nbIcons);

	// if (fscanf(data, "%d %d", &nbCards, &nbIcons) != 2 || nbCards == 0 || nbIcons == 0) {
	// if (nbCards == 0 || nbIcons == 0) {
		// printf("%d %d\n", nbCards, nbIcons);
		// printError(INCORRECT_FORMAT);
	// }
	initDeck(nbCards, nbIcons);

	int icons[nbIcons], iconId;

	for (int i = 0; i < nbCards; i++) {
		fscanf(data, "\n");

		for (int j = 0; j < nbIcons - 1; j++) {
			if (fscanf(data, "%d ", &iconId) != 1)
				printError(INCORRECT_FORMAT);
			icons[j] = iconId;
		}

		if (fscanf(data, "%d ", &iconId) != 1)
			printError(INCORRECT_FORMAT);
		icons[nbIcons - 1] = iconId;

		initCard(&deckGlobal.cards[i], nbIcons, icons);
	}

	fclose(data);
}

void initIcon(Icon* icon, int iconId, double radius, double angle, double rotation, double scale,
	int centerX, int centerY) {
	icon->iconId = iconId;
	icon->radius = radius;
	icon->angle = angle;
	icon->rotation = rotation;
	icon->scale = scale;
	icon->centerX = centerX;
	icon->centerY = centerY;
}

void initCard(Card* card, int nbIcons, int icons[]) {
	card->nbIcons = nbIcons;
	card->icons = malloc(sizeof(Icon)*nbIcons);
	for(int i=0; i<nbIcons; i++){
		card->icons[i].iconId = icons[i];
	}
}

void initDeck(int nbCards, int nbIcons) {
	deckGlobal.nbIcons = nbIcons;
	deckGlobal.nbCards = nbCards;
	deckGlobal.cards = malloc(sizeof(Card) * nbCards);
}

void onMouseMove(int x, int y)
{
	printf("dobble: Position de la souris: (%3d %3d)\r", x, y);
	fflush(stdout);
}


void onMouseClick(int mouseX, int mouseY)
{

	printf("\ndobble: Clic de la souris.\n");
	if(!timerRunning){
		printf("\ndobble: Démarrage du compte à rebours.\n");
		startTimer();
		timerRunning = true;
	}
	int clickedIcon;
	for(int i =0;  i<card1.nbIcons; i++){
		for(int j=0; j<card2.nbIcons; j++){
				if(card1.icons[i].iconId == card2.icons[j].iconId){
					clickedIcon = card1.icons[i].iconId;
				}
		}
	}

	// int mouseX = event.motion.x;
	// int mouseY = event.motion.y;
	int centerY;
	int centerX;
	int scale;
	int distance;
	for(int i = 0; i<deckGlobal.nbIcons; i++){
		centerY = card1.icons[i].centerY;
		centerX = card1.icons[i].centerX;
		scale = card1.icons[i].scale;
		distance = sqrt((mouseX - centerX)*(mouseX - centerX) + (mouseY - centerY)*(mouseY - centerY));
		if(distance <= scale/2){
			//le joueur a cliquer sur un icon
			if(card1.icons[i].iconId==clickedIcon){
				//le joueur a cliquer sur le bon icon


			} 

		}
	}
	//Si le joueur n'a cliquer sur aucun icone

	// if (timerRunning)
	// {
	// 	printf("\ndobble: Arrêt du compte à rebours.\n");
	// 	stopTimer();
	// 	timerRunning = false;
	// }
	// else
	// {
	// 	printf("\ndobble: Démarrage du compte à rebours.\n");
	// 	startTimer();
	// 	timerRunning = true;
	//}


}

void onTimerTick()
{
	printf("\ndobble: Tic du compte à rebours\n");
}


void renderScene()
{
	int i,j;
	do{
		 i=rand()%(deckGlobal.nbCards);
	}while(deckGlobal.cards[i].icons==card1.icons || deckGlobal.cards[i].icons==card2.icons);
	do {
		j=rand()%(deckGlobal.nbCards);
	} while(i==j || deckGlobal.cards[j].icons==card1.icons || deckGlobal.cards[i].icons==card2.icons);
	card1=deckGlobal.cards[i];
	card2=deckGlobal.cards[j];

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
	for (angle = 0.; angle < 360.; angle += 360. / (float)((deckGlobal.nbIcons)-1)){
		rotation = sin(angle) * angle + 120.;

		drawIcon(currentCard, card1.icons[currentIcon].iconId, radius, angle, rotation, scale, &cx, &cy);
		currentIcon++;
		// (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
	}
	rotation = 120.;
	drawIcon(currentCard, card1.icons[currentIcon].iconId, 0., angle, rotation, scale, &cx, &cy);

	// Dessin de la carte inférieure
	currentCard = LowerCard;

	// Dessin du fond de carte
	drawCardShape(currentCard, 5, 252, 252, 252, 155, 119, 170);

	// Affichage des icônes de la carte du bas (régulièrement en cercle)
	currentIcon=0;
	for (angle = 0.; angle < 360.; angle += 360. / (float)((deckGlobal.nbIcons)-1)){
		rotation = sin(angle) * angle + 70.;

		drawIcon(currentCard, card2.icons[currentIcon].iconId, radius, angle, rotation, scale, NULL, NULL);
		currentIcon++;
	}
	rotation = 70.;
	drawIcon(currentCard, card2.icons[currentIcon].iconId, 0., angle, rotation, scale, NULL, NULL);

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

	int tab[0];
	initCard(&card1, 0, tab);//initialisation de card1 vide
	initCard(&card1, 0, tab);//initialisation de card2 vide
	readCardFile("../data/pg22.txt");

	mainLoop();

	freeGraphics();

	return 0;
}
