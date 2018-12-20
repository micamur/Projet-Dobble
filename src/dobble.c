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

Resultat
    resultatClicGlobal; // Vaut INCORRECT à si le joueur a fait une erreur,
                        // CORRECT si il a une bonne réponse et INDEFINI sinon
bool iconPackChosen; // permet de savoir si le pack d'icônes a déjà été choisi

void printError(Error error) {
  switch (error) {
  case FILE_ABSENT:
    fprintf(stderr, "Echec à l'ouverture d'un fichier\n");
    break;
  case INCORRECT_FORMAT:
    fprintf(stderr, "Format de fichier incorrect\n");
    break;
  case ECHEC_ICONES:
    fprintf(stderr, "Echec du chargement des icônes.\n");
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

  // Check is the format is correct while reading each card / line
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
  icon->rotation = rand() % 360; // random between 0 and 359
  icon->radius =
      CARD_RADIUS * (0.5 + (rand() % 3) * 0.1); // random between 0.5 and 0.7
  icon->scale =
      (rand() % 6) * 0.1 + 0.005 * icon->radius; // random between 0.6 and 1.2
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
  currentCard.icons[currentIcon].scale = 1;
}

void onMouseMove(int x, int y) {
  printf("dobble: Position de la souris: (%3d %3d)\r", x, y);
  fflush(stdout);
}

double dist(double ax, double ay, double bx, double by) {
  return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

Resultat onMouseClick(int mouseX, int mouseY) {
  printf("\ndobble: Clic de la souris.\n");

  // Si le timer n'est pas enclanché :
  // Choix du pack d'icônes puis lancement du timer
  if (!timerRunning && !iconPackChosen) {
    EnterBoutonClic(mouseX, mouseY);
    showWindow();
  }

  // Si le timmer n'est pas enclanché et que le menu a été initilisé
  if (!timerRunning && iconPackChosen) {
    printf("\ndobble: Démarrage du compte à rebours.\n");
    startTimer();
    timerRunning = true;
    return INDEFINI;
  }

  // Si le timer est inferieur ou égal à 0 on gère les clics du menu de fin
  // Sinon cas normal du mainloop
  if (timeGlobal <= 0) {
    ExitBoutonClic(mouseX, mouseY);
  } else {
    // Identification de l'icône identique aux deux cartes
    int indexOfIdenticalIconUpper;
    for (int i = 0; i < deckGlobal.nbIcons; i++) {
      for (int j = 0; j < deckGlobal.nbIcons; j++) {
        if (cardUpperGlobal.icons[i].iconId ==
            cardLowerGlobal.icons[j].iconId) {
          indexOfIdenticalIconUpper = i;
        }
      }
    }

    // Variable booléenne permettant de savoir si on a cliqué sur le bon icône
    bool iconClickedIsCorrect = false;

    // Vérification que le joueur n'a pas cliqué hors de la carte
    // Si le clic est hors de la carte son action n'est pas pris en compte
    float distance =
        dist(mouseX, mouseY, (WIN_WIDTH / 2), (4 * FONT_SIZE + CARD_RADIUS));
    if (distance > CARD_RADIUS) {
      return INDEFINI;
    }

    // Calcul de la distance entre le curseur au moment du clic et le bon icône
    int centerY = cardUpperGlobal.icons[indexOfIdenticalIconUpper].centerY;
    int centerX = cardUpperGlobal.icons[indexOfIdenticalIconUpper].centerX;
    float scale = cardUpperGlobal.icons[indexOfIdenticalIconUpper].scale;
    distance = dist(mouseX, mouseY, centerX, centerY);

    // Si le joueur a cliqué sur le bon icône il gagne du temps, on augmente
    // son score et le résultat de son clic est mis à CORRECT
    if (distance <= (scale * WIN_ICON_SIZE) / 2.) {
      iconClickedIsCorrect = true;
      timeGlobal += 3;
      scoreGlobal++;
      resultatClicGlobal = CORRECT;
      changeCards();
      renderScene();
      return CORRECT;
    } else {
      // Si le joueur n'a pas cliqué sur le bon icône il perd du temps et le
      // résultat de son clic est mis à INCORRECT
      timeGlobal -= 3;
      resultatClicGlobal = INCORRECT;
      nbFalse++;
      changeCards();
      renderScene();
      return INCORRECT;
    }
  }
  return INDEFINI;
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

  // Mise à jour de cardUpperGlobal et cardLowerGlobal en fonction des
  // nouveaux indices
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

void drawCard(CardPosition currentCardPosition, Card currentCard,
              int resultatClic) {
  int cx, cy;
  // Dessin du fond de carte de la carte courante (fond clair, bord foncé)
  // Le joueur a fait une erreur
  if (resultatClic == INCORRECT) {
    drawCardShape(currentCardPosition, 5, CARDCOLOR, CARDCOLOR, CARDCOLOR, 220,
                  0, 0);

    // Le joueur a trouvé une bonne réponse
  } else if (resultatClic == CORRECT) {
    drawCardShape(currentCardPosition, 5, CARDCOLOR, CARDCOLOR, CARDCOLOR, 0,
                  200, 0);
    // cas normal
  } else {
    drawCardShape(currentCardPosition, 5, CARDCOLOR, CARDCOLOR, CARDCOLOR,
                  CARDBORDER, CARDBORDER, CARDBORDER);
  }
  // Mélange des icônes
  shuffle(currentCard.icons, deckGlobal.nbIcons);

  // Affichage des icônes de la carte du courante (régulièrement en cercle)
  for (int currentIcon = 0; currentIcon < deckGlobal.nbIcons; currentIcon++) {
    drawIcon(currentCardPosition, currentCard.icons[currentIcon], &cx, &cy);
    currentCard.icons[currentIcon].centerX = cx;
    currentCard.icons[currentIcon].centerY = cy;
  }
  // (cx, cy) est le centre de l'icône placé à l'écran (en pixels)
}

void renderScene() {
  // Affichage des différents menus ou du jeu
  if (timeGlobal <= 0) {
    afficheMenuFin();
  } else if (!iconPackChosen) {
    afficheMenuDebut();
  } else {
    char title[100];

    // Efface le contenu de la fenêtre
    clearWindow();

    // Crée le texte qui sera affiché avec le titre, le score et le temps
    // restant
    sprintf(title, "Ai & Yuki -Dobble     Score : %d", scoreGlobal);
    drawText(title, WIN_WIDTH / 2, 0.4 * FONT_SIZE, Center, Top, TEXTCOLOR,
             TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);
    sprintf(title, "Temps restant : %ds", timeGlobal);
    drawText(title, WIN_WIDTH / 2, 1.6 * FONT_SIZE, Center, Top, TEXTCOLOR,
             TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);

    // Dessin de la carte supérieure et de la carte inférieure
    drawCard(UpperCard, cardUpperGlobal, resultatClicGlobal);
    // on remet erreur à 0 pour que seulement le cercle du
    // haut soit modifié en cas d'erreur ou de bonne réponse
    resultatClicGlobal = INDEFINI;
    drawCard(LowerCard, cardLowerGlobal, resultatClicGlobal);

    // Met au premier plan le résultat des opérations de dessin
    showWindow();
  }
}

void afficheMenuDebut() {
  clearWindow();
  afficheTitreMenuDebut();
  afficheBoutonsDebut();
  showWindow();
}

void afficheMenuFin() {
  clearWindow();
  afficheStats();
  afficheBoutonsFin();
  showWindow();
}

void afficheStats() {
  char title[100];

  sprintf(title, "Ai & Yuki -Dobble     Score : %d", scoreGlobal);
  drawText(title, WIN_WIDTH / 2, 0.4 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);

  sprintf(title, "Nombre d'erreurs : %d", nbFalse);
  drawText(title, WIN_WIDTH / 2, 1.6 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);

  sprintf(title, "Bravo ! Et merci d'avoir joué !");
  drawText(title, WIN_WIDTH / 2, 2.8 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);

  sprintf(title, "Voulez-vous rejouer ?");
  drawText(title, WIN_WIDTH / 2, 4 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);
}

void afficheTitreMenuDebut() {
  char title[100];

  sprintf(title, "Bienvenue sur Ai & Yuki Dobble !");
  drawText(title, WIN_WIDTH / 2, 0.4 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);

  sprintf(title, "À quelle version voulez-vous jouer ?");
  drawText(title, WIN_WIDTH / 2, 1.6 * FONT_SIZE, Center, Top, TEXTCOLOR,
           TEXTCOLOR, TEXTCOLOR, GENERALCOLOR);
}

void afficheBouton(int offsetY, double circleWidth, char text[100], int textR,
                   int textG, int textB) {

  int w = (int)(WIN_SCALE * 5);
  if (w <= 0)
    w = 1;
  fillCircle(WIN_WIDTH / 2, offsetY * FONT_SIZE + CARD_RADIUS,
             (CARD_RADIUS + 5) * circleWidth + w / 2, textR, textG, textB, 255);
  fillCircle(WIN_WIDTH / 2, offsetY * FONT_SIZE + CARD_RADIUS,
             (CARD_RADIUS + 5) * circleWidth - w / 2, 1.1 * GENERALCOLOR,
             1.1 * GENERALCOLOR, 1.1 * GENERALCOLOR, 255);

  char title[100];
  sprintf(title, "%s\n", text);
  drawText(title, WIN_WIDTH / 2, offsetY * FONT_SIZE + CARD_RADIUS, Center,
           Middle, textR, textG, textB, 1.1 * GENERALCOLOR);
}

void afficheBoutonsFin() {
  afficheBouton(4, 1 / 4., "Oui", TEXTCOLOR, TEXTCOLOR, TEXTCOLOR);
  afficheBouton(10, 1 / 4., "Non", TEXTCOLOR, TEXTCOLOR, TEXTCOLOR);
}

void afficheBoutonsDebut() {
  afficheBouton(4, 1 / 3., "Cœur", 200, 90, 180);
  afficheBouton(10, 1 / 3., "Flocon", 90, 190, 190);
  afficheBouton(16, 1 / 3., "Food", 225, 150, 50);
}

void ExitBoutonClic(int mouseX, int mouseY) {
  float rayon = ((CARD_RADIUS + 5) / 4);
  int centerX = WIN_WIDTH / 2;

  // Test si le clic est au niveau du bouton Oui (pour rejouer)
  int centerY = 4 * FONT_SIZE + CARD_RADIUS;
  float distance = dist(mouseX, mouseY, centerX, centerY);
  if (distance <= rayon) {
    // on reprend 2 nouvelles cartes
    changeCards();
    // on réinitialise le temps et on conserve le score
    timeGlobal = 30;
    // on remet le résultat à INDEFINI pour l'inintialiser normalement
    resultatClicGlobal = INDEFINI;
    // on relance la boucle principale
    mainLoop();
    // on conserve le score d'une partie à l'autre
  }

  // Test si le clic est au niveau du bouton Non (pour quitter)
  centerY = 10 * FONT_SIZE + CARD_RADIUS;
  distance = dist(mouseX, mouseY, centerX, centerY);
  if (distance <= rayon) {
    exit(0);
  }
}

void EnterBoutonClic(int mouseX, int mouseY) {
  float rayon = ((CARD_RADIUS + 5) / 3);
  int centerX = WIN_WIDTH / 2;

  // Test si le clic est au niveau du bouton Cœur
  int centerY = 4 * FONT_SIZE + CARD_RADIUS;
  float distance = dist(mouseX, mouseY, centerX, centerY);
  if (distance <= rayon) {
    printf("Cœur");
    if (loadIconMatrix(DATA_DIRECTORY "/Hearts_80_90x90pixels.png") != 1) {
      printError(ECHEC_ICONES);
    }
    iconPackChosen = true;
    return;
  }

  // Test si le clic est au niveau du bouton Flocon
  centerY = 10 * FONT_SIZE + CARD_RADIUS;
  distance = dist(mouseX, mouseY, centerX, centerY);
  if (distance <= rayon) {
    printf("Flocon");
    if (loadIconMatrix(DATA_DIRECTORY "/Snowflakes_200_90x90pixels.png") != 1) {
      printError(ECHEC_ICONES);
    }
    iconPackChosen = true;
    return;
  }

  // Test si le clic est au niveau du bouton Flocon
  centerY = 16 * FONT_SIZE + CARD_RADIUS;
  distance = dist(mouseX, mouseY, centerX, centerY);
  if (distance <= rayon) {
    printf("Food");
    if (loadIconMatrix(DATA_DIRECTORY "/Gastronomy_230_90x90pixels.png") != 1) {
      printError(ECHEC_ICONES);
    }
    iconPackChosen = true;
    return;
  }
  // Si le clic est hors des deux boutons on sort de la fonction sans rien faire
  return;
}

Card getCardFromPosition(CardPosition cardPos) {
  if (cardPos == UpperCard)
    return cardUpperGlobal;
  else
    return cardLowerGlobal;
}

int main(int argc, char **argv) {
  srand(time(NULL));

  if (!initializeGraphics()) {
    printf("dobble: Echec de l'initialisation de la librairie graphique.\n");
    return 1;
  }

  // Lecture du fichier de cartes
  char const *cardFileName = "../data/pg28.txt";
  readCardFile(cardFileName);

  // Sélection de deux cartes aléatoires
  changeCards();

  // Initialisation des variables globales
  iconPackChosen = false;
  timeGlobal = 30;
  scoreGlobal = 0;
  nbFalse = 0;
  resultatClicGlobal = INDEFINI;
  // Initilisé à INDEFINI : on n'a encore donné ni bonne ni mauvaise réponse

  mainLoop();

  freeGraphics();

  return 0;
}
