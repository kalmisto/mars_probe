#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <assert.h>
#define WIDHT	640
#define HEIGHT	480
#define GRAV	4

struct alus {
	int x;	/* aluksen paikka screenillä */
	int y;
};

struct alus probe;
double time_scale;
static SDL_Surface *screen;
static SDL_Surface *luotain_surface;
static SDL_Surface *background;

static void grav() 
{
	probe.y = probe.y++;
}

static void draw_background()
{
	// draw the background image
  	SDL_Rect src, dest;
     	src.x = 0;
        src.y = 0;
        src.w = background->w;
        src.h = background->h;
        dest = src;
        SDL_BlitSurface(background, &src, screen, &dest);
}

static void draw_probe()
{
	SDL_Rect src, dest;

	src.x = 0;
      	src.y = 0;
      	src.w = luotain_surface->w;
      	src.h = luotain_surface->h;

      	
      /* The probe's position specifies its centre. We subtract half of 
	 its width and height to get its upper left corner */
	dest.x = probe.x - luotain_surface->w / 2;
      	dest.y = probe.y - luotain_surface->h / 2;
      	dest.w = luotain_surface->w;
      	dest.h = luotain_surface->h;
      	SDL_BlitSurface(luotain_surface, &src, screen, &dest);
}

static void PlayGame()
{
	Uint8 *keystate; 
	int quit = 0;
    	int prev_ticks = 0, cur_ticks = 0; /* for keeping track of timing */

    	/* framerate counter variables */
    	int start_time, end_time;
    	int frames_drawn = 0;
	
    	prev_ticks = SDL_GetTicks();
 
	while (quit==0) {
		/* Determine how many milliseconds have passed since
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
		if (keystate[SDLK_LEFT]) probe.x = probe.x - 1;
		if (keystate[SDLK_RIGHT]) probe.x = probe.x + 1;
		
		/* Forward and back arrow keys activate thrusters. */
		if (keystate[SDLK_UP]) probe.y = probe.y - 3;
		if (keystate[SDLK_DOWN]) probe.y = probe.y + 3;
	
		grav();
		draw_background();
		draw_probe(); 
	        SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
}

int main()
{
	probe.x = 320;
	probe.y = 100;

	/* Initialise the SDL and error check */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
    		{
      		printf("Unable to initialise SDL: %s\n", SDL_GetError());
      		exit(EXIT_FAILURE);
      		// PFJ - Changed from return 1;
    	}

	/* Ensure SDL_Quit is called on termination */
	atexit(SDL_Quit);

	/* Attempt to set a 640 x 480 hicolor (16-bit) video mode */
	screen = SDL_SetVideoMode(640, 480, 16, 0);
	if (screen == NULL)
		{
      		printf("Unable to set video mode: %s\n", SDL_GetError());
      		exit(EXIT_FAILURE);
      		// PFJ - Changed from return 1;
    	}

  	/* Load the bitmaps */
  	background = SDL_LoadBMP("background2.bmp");
  	if (background == NULL)
    		{
      		printf("Unable to load background image\n");
      		exit(EXIT_FAILURE);
      		// Both lines changed to give a corrected exit and descriptive error
    	}

  	luotain_surface = SDL_LoadBMP("alus.bmp");
  	if (luotain_surface == NULL)
    		{
      		printf("Unable to load the penguin\n");
      		exit(EXIT_FAILURE);
      		// Both lines changed as above
    	}

	PlayGame();
	SDL_FreeSurface(background);
  	SDL_FreeSurface(luotain_surface);

  	return 0;
}
 



