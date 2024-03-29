#include <sys/stat.h>

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define WIDHT	1024
#define HEIGHT	640
#define GRAV	1
#define POHJA	607
#define PUOLIVALI WIDHT/2
#define TAUSTAN_LEVEYS 5680 /* = viimeisen rotaatiospritekuvan aloituskohta blittaukselle(koska blitataan surfacen vasemman yläkulman koordinaattien mukaan) */
#define round(p) ((p)>=0?(long)((p)+0.5):(long)((p)-0.5))

struct alus {
	long x;	/* aluksen paikka screenillä */
	long y;
	double dy; /* nopeusvektori */
	double dx;
};

struct	alus probe;
double	time_scale;
double	matka_x, matka_y;
double	kulma;
static	SDL_Surface *screen;
static	SDL_Surface *luotain_surface;
static	SDL_Surface *background;
int	sprite_offset;


static void
grav()
{
	if (probe.y < POHJA) {
		probe.dy = probe.dy + 0.2;
	} else {
		probe.dy = 0;
	}
}

static void
draw_background()
{
	SDL_Rect src, dest;
	src.x = 0;
        src.y = 0;
        src.w = background->w;
        src.h = background->h;
        dest = src;
        SDL_BlitSurface(background, &src, screen, &dest);
}

static void
draw_probe()
{
	SDL_Rect src, dest;

	matka_y = probe.dy * time_scale;
	probe.y = probe.y + round(matka_y);
	matka_x = probe.dx * time_scale;
	probe.x = probe.x + round(matka_x);

	src.x = 0 + sprite_offset;
	src.y = 0;
	src.w = 80;
	src.h = luotain_surface->h;


      /* Alusta liikutetaan muuttamalla aluksen keskipisteen sijaintia. Aluksen piirtäminen
	 sen sijaan määräytyy aluksen kuvan vasemman yläkulman mukaan */
	dest.x = probe.x - 80 / 2;
	dest.y = probe.y - luotain_surface->h / 2;
	dest.w = 80;
	dest.h = luotain_surface->h;
	SDL_BlitSurface(luotain_surface, &src, screen, &dest);
}

static void
throttle()
{
	probe.dy = probe.dy - (0.5 * sin(2 * M_PI * kulma / 360));
	probe.dx = probe.dx - (0.5 * cos(2 * M_PI * kulma / 360));
}


static void
PlayGame()
{
	Uint8 *keystate;
	int quit = 0;
	int prev_ticks = 0, cur_ticks = 0; /* muuttujat ajastuksen ylläpitoon */

	/* muuttujat frameratea varten  */
	int start_time, end_time;
	int frames_drawn = 0;

	prev_ticks = SDL_GetTicks();

	while (quit==0) {
		/* Determine how many milliseco5nds have passed since
		   the last frame, and update our motion scaling. */

		prev_ticks = cur_ticks;
		cur_ticks = SDL_GetTicks();
		time_scale = (double)(cur_ticks-prev_ticks)/30.0;

		/* Update SDL's internal input state information. */
		/* SDL_PumpEvents(); */
		SDL_PumpEvents();

		/* Grab a snapshot of the keyboard. */
		keystate = SDL_GetKeyState(NULL);

		/* Respond to input. */
		if (keystate[SDLK_q] || keystate[SDLK_ESCAPE]) quit = 1;
		if (keystate[SDLK_RIGHT]) {
			kulma = kulma + 5;
			if (sprite_offset >= TAUSTAN_LEVEYS) {
				sprite_offset = 0;
			} else {
				sprite_offset = sprite_offset + 80;
			}
		}
		if (keystate[SDLK_LEFT]) {
			kulma = kulma - 5;
			if (sprite_offset <= 0) {
				sprite_offset = TAUSTAN_LEVEYS;
			} else {
				sprite_offset = sprite_offset - 80;
			}
		}
		grav();
		/* Forward and back arrow keys activate thrusters. */
		if (keystate[SDLK_UP]) throttle();
		draw_background();
		if (probe.y >= POHJA) {
			probe.dx = 0;
			probe.y = POHJA;
		}
		if (probe.dy < 0 && probe.y >= POHJA) {
			probe.y--;
		}
		draw_probe();
	        SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
}

int
main(void)
{
	probe.x = PUOLIVALI;
	probe.y = POHJA;
	probe.dx = 0;
	probe.dy = 3;
	kulma = 90; /* Raketti on alussa pystyssä. Jos kulma = 0 niin raketin kärki näyttää suoraan vasemmalle */
	sprite_offset = 0; /* aloitetaan luotain_surfacen piirtö rotaatiosprite.bmp:n ensimmäisestä "framesta". */

	/* Initialise the SDL and error check */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
		printf("Unable to initialise SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		// PFJ - Changed from return 1;
	}

	/* Ensure SDL_Quit is called on termination */
	atexit(SDL_Quit);

	/* Attempt to set a WIDHT x HEIGHT hicolor (16-bit) video mode */
	screen = SDL_SetVideoMode(WIDHT, HEIGHT, 16, 0);
	if (screen == NULL)
		{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		// PFJ - Changed from return 1;
	}

	/* Load the bitmaps */
	background = SDL_LoadBMP("bg1024.bmp");
	if (background == NULL)
		{
		printf("Unable to load background image\n");
		exit(EXIT_FAILURE);
		// Both lines changed to give a corrected exit and descriptive error
	}

	luotain_surface = SDL_LoadBMP("rotaatiosprite.bmp");
	if (luotain_surface == NULL)
		{
		printf("Unable to load the rotaatiosprite.bmp\n");
		exit(EXIT_FAILURE);
		// Both lines changed as above
	}

	PlayGame();
	SDL_FreeSurface(background);
	SDL_FreeSurface(luotain_surface);

	return 0;
}




