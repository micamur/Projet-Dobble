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

void initDeck(int nbCards, int nbIcons) {
  deckGlobal.nbIcons = nbIcons;
  deckGlobal.nbCards = nbCards;
  deckGlobal.cards = malloc(sizeof(Card) * nbCards);
}

void initCard(Card *card, int nbIcons, int icons[]) {
  card->nbIcons = nbIcons;
  card->icons = malloc(sizeof(Icon) * nbIcons);
  for (int i = 0; i < nbIcons; i++) {
    card->icons[i].iconId = icons[i];
  }
}

void readCardFile(char *fileName) {
  FILE *data = fopen(fileName, "r");

  if (data == NULL)
    printError(FILE_ABSENT);

  int nbCards, nbIcons;

  fscanf(data, "%d %d", &nbCards, &nbIcons);

  // if (fscanf(data, "%d %d", &nbCards, &nbIcons) != 2 || nbCards == 0 ||
  // nbIcons == 0) { if (nbCards == 0 || nbIcons == 0) { printf("%d %d\n",
  // nbCards, nbIcons); printError(INCORRECT_FORMAT);
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

void initIcon(Icon *icon, double angle) {
  icon->angle = angle;
  icon->rotation = rand() % 360;          // random between 0 and 360
  icon->scale = (rand() % 7) * 0.1 + 0.6; // random between 0.6 and 1.3
  icon->radius =
      CARD_RADIUS * (0.5 + (rand() % 3) * 0.1); // random between 0.5 and 0.8
}

void initCardIcons(Card currentCard) {
  int currentIcon = 0;
  int angleOffset = rand() % 360;
  for (int angle = 0; angle < 360; angle += 360 / ((deckGlobal.nbIcons) - 1)) {
    initIcon(&currentCard.icons[currentIcon], (angle + angleOffset) % 360);
    currentIcon++;
  }

  // Placement d'un icône au centre
  initIcon(&currentCard.icons[currentIcon], 0.);
  currentCard.icons[currentIcon].radius = 0;
}

void onMouseMove(int x, int y) {
  printf("dobble: Position de la souris: (%3d %3d)\r", x, y);
  fflush(stdout);
}

void onMouseClick(int mouseX, int mouseY) {

  printf("\ndobble: Clic de la souris.\n");
  if (!timerRunning) {
    printf("\ndobble: Démarrage du compte à rebours.\n");
    startTimer();
    timerRunning = true;
  }
  int clickedIcon;
  for (int i = 0; i < card1.nbIcons; i++) {
    for (int j = 0; j < card2.nbIcons; j++) {
      if (card1.icons[i].iconId == card2.icons[j].iconId) {
        clickedIcon = card1.icons[i].iconId;
      }
    }
  }

  int centerY;
  int centerX;
  int scale;
  int distance;
  for (int i = 0; i < deckGlobal.nbIcons; i++) {
    centerY = card1.icons[i].centerY;
    centerX = card1.icons[i].centerX;
    scale = card1.icons[i].scale;
    distance = sqrt((mouseX - centerX) * (mouseX - centerX) +
                    (mouseY - centerY) * (mouseY - centerY));
    if (distance <= scale / 2) {
      // le joueur a cliquer sur un icon
      if (card1.icons[i].iconId == clickedIcon) {
        // le joueur a cliquer sur le bon icon
      }
    }
  }
  changeCards();
  renderScene();
  // Si le joueur n'a cliquer sur aucun icone

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

void onTimerTick() { printf("\ndobble: Tic du compte à rebours\n"); }

void changeCards() {
  int i, j;
  do {
    i = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[i].icons == card1.icons ||
           deckGlobal.cards[i].icons == card2.icons);
  do {
    j = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[j].icons == card1.icons ||
           deckGlobal.cards[i].icons == card2.icons || i == j);
  card1 = deckGlobal.cards[i];
  card2 = deckGlobal.cards[j];

  initCardIcons(card1);
  initCardIcons(card2);
}

void shuffle(Icon *elems, int nbElems) {
  for (int i = nbElems - 1; i > 0; i--) {
    int j = rand() % i;
    Icon tmp = elems[i];
    elems[i] = elems[j];
    elems[j] = tmp;
  }
}

void drawCard(CardPosition currentCardPosition, Card currentCard) {
  int cx, cy;

  // Dessin du fond de carte de la carte courante
  drawCardShape(currentCardPosition, 5, 252, 252, 252, 155, 119, 170);

  // Shuffle the icons
  shuffle(currentCard.icons, deckGlobal.nbIcons);

  // Affichage des icônes de la carte du courante (régulièrement en cercle)
  for (int currentIcon = 0; currentIcon < deckGlobal.nbIcons; currentIcon++)
    drawIcon(currentCardPosition, currentCard.icons[currentIcon], &cx, &cy);
  // (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
}

void renderScene() {
  char title[100];

  // Efface le contenu de la fenêtre
  clearWindow();
  // Crée le titre qui sera affiché. Utile pour afficher le score.
  sprintf(title, "RICM3-Dobble    Score %d", 100);
  drawText(title, WIN_WIDTH / 2, 0, Center, Top);

  // Dessin de la carte supérieure et de la carte inférieure
  drawCard(UpperCard, card1);
  drawCard(LowerCard, card2);

  // Met au premier plan le résultat des opérations de dessin
  showWindow();
}

int main(int argc, char **argv) {
  srand(time(NULL));

  if (!initializeGraphics()) {
    printf("dobble: Echec de l'initialisation de la librairie graphique.\n");
    return 1;
  }

  if (loadIconMatrix(DATA_DIRECTORY "/Matrice8x10_Icones90x90.png") != 1) {
    printf("dobble: Echec du chargement des icônes.\n");
    return -1;
  }

  int tab[0];
  initCard(&card1, 0, tab); // initialisation de card1 vide
  initCard(&card1, 0, tab); // initialisation de card2 vide
  readCardFile("../data/pg23.txt");

  changeCards();
  mainLoop();

  freeGraphics();

  return 0;
}
