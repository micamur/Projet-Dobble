#include <stdbool.h>

/**
 * pour mac os x, et suivant l'installation de la librairie SDL2, vous devrez éventuellement adapter les trois lignes suivantes
 *
**/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "dobble-config.h"
#include "dobble.h"
#include "graphics.h"

/**
 * Représente l'état des méthodes graphiques.
 *
 * Cette structure est initialisée par la méthode initializeGraphics, et libéré par freeGraphics.
 */
static struct GraphicState
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *matrixTexture;
	SDL_Texture *iconTexture;

	Sint32 matrixWidth;
	Sint32 matrixHeight;

	TTF_Font *font;

	bool stopTimerRequested;
	bool timerRunning;

	bool redrawRequested;

	Uint32 userTimerEvent;
	Uint32 userCallLaterEvent;
} g;

typedef struct
{
	void (*method)(void *);
	void *param;
} CallLaterData;

/****************** METHODES A IMPLEMENTER ******************/

void getIconLocationInMatrix(int iconId, int *posX, int *posY)
{
	// TODO: Calculer la position de l'icône iconId dans la matrice d'icônes
	// g.matrixWidth (resp. g.matrixHeight) sont la largeur (resp. hauteur) de la matrice d'icônes, en pixels
	// Les icônes sont de taille ICON_SIZE (défini dans build/dobble-config.h)
	*posX = 0;
	*posY = 0;
}

/****************** METHODES UTILITAIRES ******************/

void getCardCenter(CardPosition card, int *cardCenterX, int *cardCenterY)
{
	if (card == UpperCard)
	{
		(*cardCenterX) = WIN_WIDTH / 2;
		(*cardCenterY) = 4 * FONT_SIZE + CARD_RADIUS;
	}
	else
	{
		(*cardCenterX) = WIN_WIDTH / 2;
		(*cardCenterY) = WIN_HEIGHT - CARD_RADIUS - FONT_SIZE;
	}
}

Uint32 callLaterCallback(Uint32 interval, void *param)
{
	// Envoi d'un évènement à la boucle principale pour appeler la méthode indiquée en paramètre
	SDL_Event evt;
	SDL_zero(evt);
	evt.type = SDL_USEREVENT;
	evt.user.type = g.userCallLaterEvent;
	evt.user.data1 = ((CallLaterData *)param)->method;
	evt.user.data2 = ((CallLaterData *)param)->param;

	// Libération de la mémoire
	free(param);

	SDL_PushEvent(&evt);

	return 0;
}

void callLater(void (*method)(void *), void *param, Uint32 delay)
{
	CallLaterData *data = malloc(sizeof(CallLaterData));
	if (!data)
	{
		printf("SDL: Echec de l'allocation pour l'invocation de méthode planifiée.\n");
		return;
	}

	data->method = method;
	data->param = param;

	SDL_AddTimer(delay, callLaterCallback, data);
}

/****************** METHODES DE GESTION DU TIMER ******************/

/**
 * Procédure appelée par la SDL lorsqu'un timer est activé.
 *
 * @param  interval  Nombre de millisecondes depuis le dernier appel
 * @param  param     Paramètre personnalisé (NULL)
 * @return           0 si le compte à rebours doit être arrêté, != 0 sinon
 */
Uint32 myTimer(Uint32 interval, void *param)
{
	if (g.stopTimerRequested)
	{
		// L'arrêt du compte à rebours a été demandé, retour de la
		// valeur 0 pour arrêter le timer de la SDL
		g.timerRunning = false;
		g.stopTimerRequested = false;
		return 0;
	}
	else
	{
		// Envoi d'un évènement à la boucle principale pour indiquer que le timer a été déclenché
		// La méthode myTimer est appelée sur un thread différent du thread principal, on utilise les évènements
		// de la SDL pour éviter les problèmes de synchronisation
		SDL_Event evt;
		SDL_zero(evt);
		evt.type = SDL_USEREVENT;
		evt.user.type = g.userTimerEvent;
		SDL_PushEvent(&evt);

		// Sinon continuer l'exécution du timer avec le même intervalle
		// de temps
		return interval;
	}
}

void startTimer()
{
	if (g.timerRunning)
	{
		if (g.stopTimerRequested)
			g.stopTimerRequested = false;
		else
			printf("SDL: Impossible de lancer un compte à rebours alors qu'un compte à rebours est déjà en cours d'exécution.\n");
		return;
	}

	g.timerRunning = true;

	// 1000ms = 1s, intervalle par défaut du timer
	SDL_AddTimer(1000, myTimer, NULL);
}

void stopTimer()
{
	// Demander l'arrêt du timer au prochain déclenchement
	g.stopTimerRequested = true;
}

/****************** METHODES DE CHARGEMENT ******************/

int loadIconMatrix(const char *fileName)
{
	SDL_Surface *image = NULL;

	printf("SDL: Chargement de l'image '%s'.\n", fileName);

	// Chargement de l'image avec SDL_Image
	image = IMG_Load(fileName);
	if (image == NULL)
	{
		printf("SDL: Echec du chargement de l'image '%s'.\n", fileName);
		g.matrixWidth = g.matrixHeight = 0;
		return 0;
	}

	// Transformation de la surface (données d'image) en texture (pouvant être dessiné à l'écran)
	g.matrixTexture = SDL_CreateTextureFromSurface(g.renderer, image);
	if (g.matrixTexture == NULL)
	{
		printf("SDL: Echec de la création de texture pour '%s'.\n", fileName);
		g.matrixWidth = g.matrixHeight = 0;
		SDL_FreeSurface(image);
		return 0;
	}

	// Nécessaire pour le dessin d'icônes transparents
	SDL_SetTextureBlendMode(g.matrixTexture, SDL_BLENDMODE_NONE);

	// La surface n'est plus nécessaire (l'image est maintenant stockée dans la texture)
	SDL_FreeSurface(image);

	// Obtention de la taille de la matrice d'icônes (en pixels)
	SDL_QueryTexture(g.matrixTexture, NULL, NULL, &g.matrixWidth, &g.matrixHeight);

	return 1;
}

/****************** METHODES DE DESSIN ******************/

void clearWindow()
{
	SDL_SetRenderDrawColor(g.renderer, 255, 255, 255, 0);
	SDL_RenderClear(g.renderer);
}

void showWindow()
{
	SDL_RenderPresent(g.renderer);
}

void requestRedraw()
{
	g.redrawRequested = true;
}

int drawText(const char *message, int x, int y, HAlign hAlign, VAlign vAlign)
{
	SDL_Surface *text = NULL;
	SDL_Texture *textTexture = NULL;
	int tw, th;

	SDL_Color black = {0, 0, 0},
	          white = {255, 255, 255};

	// Rendu du texte dans une surface
	text = TTF_RenderUTF8_Shaded(g.font, message, black, white);
	tw = text->w;
	th = text->h;

	if (text == NULL)
	{
		printf("SDL: Echec de la création de la surface texte.\n");
		return 0;
	}

	// Tranformation de la surface en texture
	textTexture = SDL_CreateTextureFromSurface(g.renderer, text);
	if (textTexture == NULL)
	{
		printf("SDL: Echec de la création de la texture texte.\n");
		SDL_FreeSurface(text);
		return 0;
	}

	// La surface de texte n'est plus nécessaire (convertie en texture)
	SDL_FreeSurface(text);

	// Position par défaut pour alignement (Left, Top)
	SDL_Rect textPosition = {x, y, tw, th};

	if (hAlign == Center)
	{
		textPosition.x -= tw / 2;
	}
	else if (hAlign == Right)
	{
		textPosition.x -= tw;
	}

	if (vAlign == Middle)
	{
		textPosition.y -= th / 2;
	}
	else if (vAlign == Bottom)
	{
		textPosition.y -= th;
	}

	SDL_RenderCopy(g.renderer, textTexture, NULL, &textPosition);

	// La texture de texte n'est plus nécessaire maintenant
	SDL_DestroyTexture(textTexture);

	return 1;
}

// Remplissage de cercle basé sur des scanlines et le midpoint algorithm.
// Chaque itération remplit 4 lignes de pixels du cercle.
//
// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void fillCircle(int x0, int y0, int radius, Uint8 fr, Uint8 fg, Uint8 fb, Uint8 fa)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	SDL_SetRenderDrawColor(g.renderer, fr, fg, fb, fa);

	while (x >= y)
	{
		SDL_RenderDrawLine(g.renderer, x0 - x, y0 - y, x0 + x, y0 - y);
		SDL_RenderDrawLine(g.renderer, x0 - x, y0 + y, x0 + x, y0 + y);
		SDL_RenderDrawLine(g.renderer, x0 - y, y0 + x, x0 + y, y0 + x);
		SDL_RenderDrawLine(g.renderer, x0 - y, y0 - x, x0 + y, y0 - x);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}

// Dessin de cercle basé sur le midpoint algorithm.
// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void drawCircle(int x0, int y0, int radius, Uint8 fr, Uint8 fg, Uint8 fb, Uint8 fa)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	SDL_SetRenderDrawColor(g.renderer, fr, fg, fb, fa);

	while (x >= y)
	{
		SDL_RenderDrawPoint(g.renderer, x0 + x, y0 + y);
		SDL_RenderDrawPoint(g.renderer, x0 + y, y0 + x);
		SDL_RenderDrawPoint(g.renderer, x0 - y, y0 + x);
		SDL_RenderDrawPoint(g.renderer, x0 - x, y0 + y);
		SDL_RenderDrawPoint(g.renderer, x0 - x, y0 - y);
		SDL_RenderDrawPoint(g.renderer, x0 - y, y0 - x);
		SDL_RenderDrawPoint(g.renderer, x0 + y, y0 - x);
		SDL_RenderDrawPoint(g.renderer, x0 + x, y0 - y);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}

void drawCardShape(CardPosition card, int w, Uint8 bgr, Uint8 bgg, Uint8 bgb, Uint8 fgr, Uint8 fgg, Uint8 fgb)
{
	int cardCenterX, cardCenterY;

	/* Mise à l'échelle de l'épaisseur */
	w = (int)(WIN_SCALE * w);
	if (w <= 0)
		w = 1;

	getCardCenter(card, &cardCenterX, &cardCenterY);

	fillCircle(cardCenterX, cardCenterY, CARD_RADIUS + w / 2, fgr, fgg, fgb, 255);
	fillCircle(cardCenterX, cardCenterY, CARD_RADIUS - w / 2, bgr, bgg, bgb, 255);
}

void drawIcon(CardPosition card, int iconId, double radius, double angle, double rotation, double scale,
              int *centerX, int *centerY)
{
	int cardCenterX, cardCenterY;

	/* Mise à l'échelle des mesures */
	radius *= WIN_SCALE;
	scale *= WIN_SCALE;

	getCardCenter(card, &cardCenterX, &cardCenterY);

	double cx = radius * cos(angle / 360. * (2. * M_PI)) + cardCenterX;
	double cy = radius * sin(angle / 360. * (2. * M_PI)) + cardCenterY;

	if (centerX)
		*centerX = (int)cx;
	if (centerY)
		*centerY = (int)cy;

	// Attention aux conversions entre nombres flottants et entiers
	int destX = cx - scale * (double)(DRAW_ICON_SIZE) / 2.;
	int destY = cy - scale * (double)(DRAW_ICON_SIZE) / 2.;
	int origX, origY;

	// Récupération de la position de l'icône dans la matrice d'icônes
	getIconLocationInMatrix(iconId, &origX, &origY);

	// Zone occupée par l'icône dans la matrice d'icônes
	SDL_Rect srcRect = {origX, origY, ICON_SIZE, ICON_SIZE};
	// Zone occupée par l'icône dans le rendu de la fenêtre du jeu
	SDL_Rect dstRect = {destX, destY, DRAW_ICON_SIZE * scale, DRAW_ICON_SIZE * scale};
	// Zone occupée par l'icône dans la texture temporaire
	SDL_Rect tmpRect = {0, 0, ICON_SIZE, ICON_SIZE};

	// Copie de l'icône dans une texture temporaire
	SDL_SetRenderTarget(g.renderer, g.iconTexture);
	SDL_RenderClear(g.renderer);
	SDL_RenderCopyEx(g.renderer, g.matrixTexture, &srcRect, &tmpRect, 0.0, NULL, SDL_FLIP_NONE);
	SDL_SetRenderTarget(g.renderer, NULL);

	// Dessin de l'icône en texture temporaire vers l'écran
	SDL_RenderCopyEx(g.renderer, g.iconTexture, &tmpRect, &dstRect, rotation, NULL, SDL_FLIP_NONE);
}

/****************** METHODES DE GESTION DU CYCLE DE VIE ******************/

int initializeGraphics()
{
	// Initialisation de la structure de données
	SDL_zero(g);
	g.redrawRequested = true;

	// Initialisation de la SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	// Création de la fenêtre
	g.window = SDL_CreateWindow("Polytech - RICM3 - API - Projet Dobble",
	                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                            WIN_WIDTH, WIN_HEIGHT, 0);

	// Création du renderer (objet de dessin sur la fenêtre)
	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_TARGETTEXTURE);

	// Initialisation de SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL: Echec de l'initialisation de SDL_image: %s\n", IMG_GetError());
		return 0;
	}

	// Initialisation de SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL: Erreur d'initialisation de TTF_Init: %s\n", TTF_GetError());
		return 0;
	}

	// Chargement de la police de caractères
	g.font = TTF_OpenFont(DATA_DIRECTORY "/FONTS/Chalkduster.ttf", FONT_SIZE);
	if (g.font == NULL)
	{
		printf("SDL: Echec du chargement de la police de caractères.\n");
		return 0;
	}

	// Création de la texture temporaire d'icône
	g.iconTexture = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888,
	                                  SDL_TEXTUREACCESS_TARGET,
	                                  ICON_SIZE, ICON_SIZE);

	if (g.iconTexture == NULL)
	{
		printf("SDL: Echec de la création de la texture d'icône temporaire.\n");
		return 0;
	}

	// Choix du mode de mélange pour la texture d'icône
	SDL_SetTextureBlendMode(g.iconTexture, SDL_BLENDMODE_BLEND);

	// Initialisation du générateur de nombres aléatoires
	srand(SDL_GetTicks());

	// Enregistrement de l'évènement de timer
	g.userTimerEvent = SDL_RegisterEvents(1);

	return 1;
}

void mainLoop()
{
	int quit = 0;
	SDL_Event event;

	while (!quit)
	{
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			// Rajouter ici le test "souris sur un pixel non transparent"
			onMouseMove(event.motion.x, event.motion.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			onMouseClick();
			break;
		case SDL_WINDOWEVENT:
			g.redrawRequested = true;
			break;
		case SDL_USEREVENT:
			if (event.user.type == g.userTimerEvent)
			{
				// Appel de la procédure onTimerTick déclarée dans dobble.h
				// et implémentée dans dobble.c
				onTimerTick();
			}
			else if (event.user.type == g.userCallLaterEvent)
			{
				// Appel de la procédure fournie en paramètre de l'évènement
				void (*method)(void *) = event.user.data1;
				void *param = event.user.data2;

				method(param);
			}
			break;
		case SDL_QUIT:
			printf("Merci d'avoir joué!\n");
			quit = 1;
			break;
		}

		if (g.redrawRequested)
		{
			renderScene();
			g.redrawRequested = false;
		}
	}
}

void freeGraphics()
{
	TTF_CloseFont(g.font);
	TTF_Quit();
	IMG_Quit();

	SDL_DestroyTexture(g.matrixTexture);
	SDL_DestroyTexture(g.iconTexture);
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);

	SDL_Quit();
}
