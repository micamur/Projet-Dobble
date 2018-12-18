#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "dobble-config.h"
#include "dobble.h"
#include "graphics.h"

static bool timerRunning = false; // État du compte à rebours (lancé/non lancé)

Card cardUpperGlobal, cardLowerGlobal; // Cartes du haut et du bas
Deck deckGlobal;                       // Deck des cartes du jeu actuel
int timeGlobal, scoreGlobal, nbFalse;  // Temps restant et score du joueur

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
  deckGlobal.cards = (Card *)malloc(sizeof(Card) * nbCards);
}

void initCard(Card *card, int nbIcons, int icons[]) {
  card->icons = (Icon *)malloc(sizeof(Icon) * nbIcons);
  for (int i = 0; i < nbIcons; i++) {
    card->icons[i].iconId = icons[i];
  }
}

void readCardFile(char const *fileName) {
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

double dist(double ax, double ay, double bx, double by) {
  return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
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
  for (int i = 0; i < deckGlobal.nbIcons; i++)
    for (int j = 0; j < deckGlobal.nbIcons; j++)
      if (cardUpperGlobal.icons[i].iconId == cardLowerGlobal.icons[j].iconId)
        identicalIconId = cardUpperGlobal.icons[i].iconId;

  // Vérification de l'icône cliqué
  bool iconClickedIsCorrect = false;

  // Calcul de la distance entre le clic et chacun des icônes
  for (int i = 0; i < deckGlobal.nbIcons; i++) {
    int centerY = cardUpperGlobal.icons[i].centerY;
    int centerX = cardUpperGlobal.icons[i].centerX;
    int scale = cardUpperGlobal.icons[i].scale;
    int distance = dist(mouseX, mouseY, centerX, centerY);
    // Si le joueur a cliqué sur le bon icône il gagne du temps et augmente
    // son score
    if (distance <= scale / 2 &&
        cardUpperGlobal.icons[i].iconId == identicalIconId) {
      scoreGlobal++;
      timeGlobal += 3;
      iconClickedIsCorrect = true;
    }
  }

  // Si le joueur n'a pas cliqué sur le bon icône on perd du temps
  if (!iconClickedIsCorrect) {
    timeGlobal -= 3;
    nbFalse++;
  }

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

  // Sélection d'un indice pour cardUpperGlobal différent de ceux des cartes
  // précédentes
  do {
    i = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[i].icons == cardUpperGlobal.icons ||
           deckGlobal.cards[i].icons == cardLowerGlobal.icons);

  // Sélection d'un indice pour cardLowerGlobal différent de ceux des cartes
  // précédentes et de celui de cardUpperGlobal
  do {
    j = rand() % (deckGlobal.nbCards);
  } while (deckGlobal.cards[j].icons == cardUpperGlobal.icons ||
           deckGlobal.cards[i].icons == cardLowerGlobal.icons || i == j);

  // Mise à jour de cardUpperGlobal et cardLowerGlobal en fonction des nouveaux
  // indices
  cardUpperGlobal = deckGlobal.cards[i];
  initCardIcons(cardUpperGlobal);
  cardLowerGlobal = deckGlobal.cards[j];
  initCardIcons(cardLowerGlobal);
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

  // Dessin du fond de carte de la carte courante (fond clair, bord foncé)
  drawCardShape(currentCardPosition, 5, CARDCOLOR, CARDCOLOR, CARDCOLOR,
                CARDBORDER, CARDBORDER, CARDBORDER);

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
    printf("Score final : %d\nNombre d'erreurs : %d\nMerci d'avoir joué!\n",
           scoreGlobal, nbFalse);
    exit(0);
  }

  char title[100];

  // Efface le contenu de la fenêtre
  clearWindow();

  // Crée le texte qui sera affiché avec le titre, le score et le temps restant
  sprintf(title, "Velphy-Dobble     Score : %d", scoreGlobal);
  drawText(title, WIN_WIDTH / 2, 0.4 * FONT_SIZE, Center, Top);
  sprintf(title, "Temps restant : %ds", timeGlobal);
  drawText(title, WIN_WIDTH / 2, 1.6 * FONT_SIZE, Center, Top);

  // Dessin de la carte supérieure et de la carte inférieure
  drawCard(UpperCard, cardUpperGlobal);
  drawCard(LowerCard, cardLowerGlobal);

  // Met au premier plan le résultat des opérations de dessin
  showWindow();
}

Card getCardFromPosition(CardPosition cardPos) {
  if (cardPos == UpperCard)
    return cardUpperGlobal;
  else
    return cardLowerGlobal;
}

// void initMovingIcons(CardPosition cardPos, Card card,
//                      movingIcon movingIcons[]) {
//   int cx, cy;
//   initCardIcons(card);
//   for (int currentIcon = 0; currentIcon < deckGlobal.nbCards; currentIcon++)
//   {
//     drawIcon(cardPos, card.icons[currentIcon], &cx, &cy);
//     movingIcons[currentIcon].px = cx;
//     movingIcons[currentIcon].py = cy;
//     movingIcons[currentIcon].vx = 0;
//     movingIcons[currentIcon].ax = 0;
//     movingIcons[currentIcon].vy = 0;
//     movingIcons[currentIcon].ay = 0;
//   }
// }

// void updateMovingIcons(CardPosition cardPos, movingIcon movingIcons[]) {
//   for (int i = 0; i < deckGlobal.nbCards; i++) {
//     movingIcons[i].px += movingIcons[i].vx;
//     movingIcons[i].py += movingIcons[i].vy;
//     movingIcons[i].vx += movingIcons[i].ax;
//     movingIcons[i].vy += movingIcons[i].ay;
//     Card card = getCardFromPosition(cardPos);
//     double dx = fabsf(movingIcons[i].px - card.icons[i].centerX);
//     double dy = fabsf(movingIcons[i].py - card.icons[i].centerY);
//   }
// }

int main(int argc, char **argv) {
  srand(time(NULL));

  if (!initializeGraphics()) {
    printf("dobble: Echec de l'initialisation de la librairie graphique.\n");
    return 1;
  }

  // if (loadIconMatrix(DATA_DIRECTORY "/Matrice8x10_Icones90x90.png") != 1) {
  if (loadIconMatrix(DATA_DIRECTORY "/Snowflakes_200_90x90pixels.png") != 1) {
    printf("dobble: Echec du chargement des icônes.\n");
    return -1;
  }

  // Lecture du fichier de cartes
  char const *cardFileName = "../data/pg28.txt";
  readCardFile(cardFileName);

  // Sélection de deux cartes aléatoires
  changeCards();

  // Initialisation du temps et du score
  timeGlobal = 100;
  scoreGlobal = 0;
  nbFalse = 0;

  mainLoop();

  freeGraphics();

  return 0;
}
