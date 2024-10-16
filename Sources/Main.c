/** @file Main.c
 * Program initialization and main loop.
 * @author Adrien RICCIARDI
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Viewport.h>

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Display the program help to the terminal.
 * @param String_Program_Name The program name to display.
 */
static void MainDisplayProgramUsage(char *String_Program_Name)
{
	printf("Image Viewer (C) 2017 Adrien RICCIARDI.\n"
		"\n"
		"Usage : %s Image_File | --help\n"
		"Image_File is the file to open.\n"
		"--help displays this message.\n"
		"\n"
		"Control keys :\n"
		"  - Mouse wheel : zoom in/zoom out\n"
		"  - 'f' key : toggle image flipping (first press leads to horizontal flipping, second press vertical flipping, third press both horizontal and vertical flipping, fourth press disables flipping)\n"
		"  - 'q' key : exit program\n", String_Program_Name);
}

/** Automatically called on program exit, gracefully uninitialize SDL. */
static void MainExit(void)
{
	IMG_Quit();
	SDL_Quit();
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	SDL_Event Event;
	SDL_Surface *Pointer_Surface_Image;
	unsigned int Frame_Starting_Time = 0, Elapsed_Time;
	
	// Check arguments
	if (argc != 2)
	{
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	// Is help requested ?
	if (strcmp(argv[1], "--help") == 0)
	{
		MainDisplayProgramUsage(argv[0]);
		return EXIT_SUCCESS;
	}
	
	// Initialize SDL before everything else, so other SDL libraries can be safely initialized
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Error : failed to initialize SDL (%s).\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	atexit(MainExit);
	
	// Try to initialize the SDL image library
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) != (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF))
	{
		printf("Error : failed to initialize SDL image library (%s).\n", IMG_GetError());
		return EXIT_FAILURE;
	}
	
	// Try to load the image before creating the viewport
	Pointer_Surface_Image = IMG_Load(argv[1]);
	if (Pointer_Surface_Image == NULL)
	{
		printf("Error : failed to load image file '%s' (%s).\n", argv[1], IMG_GetError());
		return EXIT_FAILURE;
	}
	
	// TODO when image is loaded, update window title with image name
	
	// Initialize modules (no need to display an error message if a module initialization fails because the module already did)
	if (ViewportInitialize(Pointer_Surface_Image) != 0) return EXIT_FAILURE; // TODO set initial viewport size and window decorations according to parameters saved on previous program exit ?
	SDL_FreeSurface(Pointer_Surface_Image);
	
	// Process incoming SDL events
	while (1)
	{
		// Keep the time corresponding to the frame rendering beginning
		Frame_Starting_Time = SDL_GetTicks();
		
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
				case SDL_QUIT:
					return EXIT_SUCCESS;
				
				case SDL_KEYDOWN:
					// Quit program
					if (Event.key.keysym.sym == SDLK_q) return EXIT_SUCCESS;
					break;
			
				// Unhandled event, do nothing
				default:
					break;
			}
		}
		
		ViewportDrawImage();
		
		// Wait enough time to get a 60Hz refresh rate
		Elapsed_Time = SDL_GetTicks() - Frame_Starting_Time;
		if (Elapsed_Time < 16) SDL_Delay(16 - Elapsed_Time);
	}
}
