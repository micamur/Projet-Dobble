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

int timeGlobal;
int scoreGlobal;
Deck deckGlobal;
Card card1;
Card card2;

void printError(Error error) {
  switch (error) {
  case FILE_ABSENT:
    fprintf(stderr, "Failed to open the file\n");
    break;
  case INCORRECT_FORMAT:
    fprintf(stderr, "Incorrect file format\n");
    break;
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
  // Open the card file in read-only mode
  FILE *data = fopen(fileName, "r");

  // Check if the opening was successful
  if (data == NULL)
    printError(FILE_ABSENT);

  // Check if the format is correct while reading the first line
  int nbCards, nbIcons;
  if (fscanf(data, "%d %d", &nbCards, &nbIcons) != 2 || nbCards == 0 ||
      nbIcons == 0) {
    printf("%d %d\n", nbCards, nbIcons);
    printError(INCORRECT_FORMAT);
  }
  initDeck(nbCards, nbIcons);

  // Check is the format es correct while reading each card / line
  int icons[nbIcons], iconId;
  for (int i = 0; i < nbCards; i++) {
    for (int j = 0; j < nbIcons; j++) {
      if (fscanf(data, "%d", &iconId) != 1)
        printError(INCORRECT_FORMAT);
      icons[j] = iconId;
    }
    initCard(&deckGlobal.cards[i], nbIcons, icons);
  }

  fclose(data);
}

void initIcon(Icon *icon, double angle) {
  icon->angle = angle;
  icon->rotation = rand() % 360;          // random between 0 and 359
  icon->scale = (rand() % 7) * 0.1 + 0.6; // random between 0.6 and 1.2
  icon->radius =
      CARD_RADIUS * (0.5 + (rand() % 3) * 0.1); // random between 0.5 and 0.7
}

void initCardIcons(Card currentCard) {
  int currentIcon = 0;
  int angleOffset = rand() % 360; // random between 0 and 359

  // Placement des icônes en cercle (régulièrement)
  for (int angle = 0; angle < 360; angle += 360 / (deckGlobal.nbIcons - 1)) {
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

  // Identification de l'icône identique aux deux cartes
  int identicalIconId;
  for (int i = 0; i < card1.nbIcons; i++)
    for (int j = 0; j < card2.nbIcons; j++)
      if (card1.icons[i].iconId == card2.icons[j].iconId)
        identicalIconId = card1.icons[i].iconId;

  // Vérification de l'icône cliqué
  bool iconClickedIsCorrect = false;

  // Calcul de la distance entre le clic et chacun des icônes
  for (int i = 0; i < deckGlobal.nbIcons; i++) {
    int centerY = card1.icons[i].centerY;
    int centerX = card1.icons[i].centerX;
    int scale = card1.icons[i].scale;
    int distance = sqrt((mouseX - centerX) * (mouseX - centerX) +
                        (mouseY - centerY) * (mouseY - centerY));
    // Si le joueur a cliqué sur le bon icône il gagne du temps et augmente
    // son score
    if (distance <= scale / 2 && card1.icons[i].iconId == identicalIconId) {
      scoreGlobal++;
      timeGlobal += 3;
      iconClickedIsCorrect = true;
    }
  }

  // Si le joueur n'a pas cliqué sur le bon icône on perd du temps
  if (!iconClickedIsCorrect)
    timeGlobal -= 3;

  // Quoi qu'il arive, après avoir cliqué on change de cartes
  changeCards();
  renderScene();
}

void onTimerTick() {
  printf("\ndobble: Tic du compte à rebours\n");
  timeGlobal--;
  renderScene();
}

void changeCards() {
  int i, j;

  // Sélection d'un indice pour card1 différent de ceux des cartes précédentes
  do {
    i = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[i].icons == card1.icons ||
           deckGlobal.cards[i].icons == card2.icons);

  // Sélection d'un indice pour card2 différent de ceux des cartes précédentes
  // et de celui de card1
  do {
    j = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[j].icons == card1.icons ||
           deckGlobal.cards[i].icons == card2.icons || i == j);

  // Mise à jour de card1 et card2 en fonction des nouveaux indices
  card1 = deckGlobal.cards[i];
  initCardIcons(card1);
  card2 = deckGlobal.cards[j];
  initCardIcons(card2);
}

void shuffle(Icon *elems, int nbElems) {
  // On échange des éléments aléatoirement
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

  // Mélange des icônes
  shuffle(currentCard.icons, deckGlobal.nbIcons);

  // Affichage des icônes de la carte du courante (régulièrement en cercle)
  for (int currentIcon = 0; currentIcon < deckGlobal.nbIcons; currentIcon++)
    drawIcon(currentCardPosition, currentCard.icons[currentIcon], &cx, &cy);
  // (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
}

void renderScene() {
  // Condition de fin de jeu
  if (timeGlobal <= 0) {
    printf("Score final : %d\nMerci d'avoir joué!\n", scoreGlobal);
    exit(0);
  }

  char title[100];

  // Efface le contenu de la fenêtre
  clearWindow();

  // Crée le texte qui sera affiché avec le titre, le score et le temps restant
  sprintf(title, "Velphy-Dobble    Score %d", scoreGlobal);
  drawText(title, WIN_WIDTH / 2, 0, Center, Top);
  sprintf(title, "Temps restant %d", timeGlobal);
  drawText(title, WIN_WIDTH / 2, 1.2 * FONT_SIZE, Center, Top);

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

  // Lecture du fichier de cartes
  readCardFile("../data/pg23.txt");

  // Sélection de deux cartes aléatoires
  changeCards();

  // Initialisation du temps et du score
  timeGlobal = 100;
  scoreGlobal = 0;

  mainLoop();

  freeGraphics();

  return 0;
}
