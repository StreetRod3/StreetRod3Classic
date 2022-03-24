/////////////////////////////////////////
//
//             Street Rod 3
//
// Copyright (C) Auxiliary Software 2002-2004
//
//
/////////////////////////////////////////
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


// System Initialization for Street Rod 3
// Created 7/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "3d.h"
#include "cache.h"
#include "StateManager.h"
#include "game.h"
#include "font.h"
#include "Options.h"
#include <SDL/SDL_SysWM.h>


mouse_t		tMouse;
keyboard_t	tKeyboard;
sr3_t		tMainSR3;
float		oldtime;
SDL_Event   sdlEvent;

bool        bDisableSound = false;
bool        bMultiTexture = false;

// OpenGL Extensions
int         GL_TEXTURE0, GL_TEXTURE1;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;



///////////////////
// Main entry point
int main(int argc, char *argv[])
{
	writeLog("Street Rod 3 begin system initialization...\n");

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)) {
		writeLog("Error: SDL Failed to initialize!\n");
		writeLog("  SDL Returns:  %s\n",SDL_GetError());
		System_Shutdown();
		return -1;
	}

   	Key_Initialize();

    // Check the parameters
    ParseArguments(argc, argv);

    // Load the options
    if( !LoadOptions() ) {
        System_Shutdown();
        return -1;
    }
    

	tMainSR3.f1 = tMainSR3.f2 = 0.0f;


	// Initialize the video
	if(!System_InitializeOpenGL()) {
		System_Shutdown();
		return -1;
	}

	// Initialize the cache
	if(!Cache_Initialize()) {
		System_Shutdown();
		return -1;
	}

    // Initialize the texture manager
    Tex_Initialize();

	// Seed the random number generator
	srand((unsigned int)time(NULL));

    System_InitializeSound();

	LoadSounds();

	Font_Load("data/gui/font2.png");

	// Initialize the manager's

	// Initialize the input
	System_InitializeInput();

	// Settle the timing a bit
	System_ProcessInput();
	System_ProcessInput();
	

	// Run the main game
	Game_Start();





	System_Shutdown();

	return 0;
}


///////////////////
// Initialize the sound
void System_InitializeSound(void)
{    
    if(!bDisableSound && psOptions->nSoundOn) {
	    // Initialize BASS
	    if(!BASS_Init(-1,44100,0,GetForegroundWindow())) {		
            writeLog("Warning: Could not initialize sound\n");
	    }
	    BASS_Start();
	    BASS_SetVolume(psOptions->nSoundVolume);
    }
}



///////////////////
// Parse the arguments
void ParseArguments(int argc, char *argv[])
{
    // Parameters passed to street rod 3 overwrite the loaded options
    char *a;
    for(int i=1; i<argc; i++) {
        a = argv[i];

        // -nosound
        // Turns off the sound
        if( stricmp(a, "-nosound") == 0 ) {
            bDisableSound = true;
        }
    }    
}



///////////////////
// Initialize the input
void System_InitializeInput(void)
{
	System_ProcessInput();
	
	for(int k = 0;k<SDLK_LAST;k++) {
		tKeyboard.KeyUp[k] = false;
		tKeyboard.KeyDown[k] = false;
	}
	
	tMouse.Button = 0;
	tMouse.Up = 0;
    tMouse.Down = 0;
}


///////////////////
// Process the input
void System_ProcessInput(void)
{

	while(SDL_PollEvent(&sdlEvent));

	// Mouse
	tMouse.Button = SDL_GetMouseState(&tMouse.X,&tMouse.Y);
	tMouse.Up = 0;
	tMouse.vPos = CVec((float)tMouse.X, (float)tMouse.Y, 0);
	
	if(!(tMouse.Button & SDL_BUTTON(1)) && tMouse.Down & SDL_BUTTON(1))
		tMouse.Up |= SDL_BUTTON(1);

	if(!(tMouse.Button & SDL_BUTTON(3)) && tMouse.Down & SDL_BUTTON(3))
		tMouse.Up |= SDL_BUTTON(3);
		
	tMouse.Down = tMouse.Button;

	

	// Keyboard
	tKeyboard.keys = SDL_GetKeyState(NULL);

	// Update the key up's
	for(int k=0;k<SDLK_LAST;k++) {
		tKeyboard.KeyUp[k] = false;
		
		if(!tKeyboard.keys[k] && tKeyboard.KeyDown[k])
			tKeyboard.KeyUp[k] = true;
		tKeyboard.KeyDown[k] = tKeyboard.keys[k];
	}


	// Process the time
	tMainSR3.fCurTime = (float)SDL_GetTicks() * 0.001f;
	tMainSR3.fDeltaTime = tMainSR3.fCurTime - oldtime;
	oldtime = tMainSR3.fCurTime;


    // Screenshot?
    if( tKeyboard.KeyUp[SDLK_F12] )
        System_TakeScreenshot();
    
    tMainSR3.TriCount = 0;

}


///////////////////
// Initialize OpenGL
int System_InitializeOpenGL(void)
{
	int rgb_size[4];
	int flags = SDL_OPENGL;

    int bpp = psOptions->nColourDepth;
	int fullscreen = psOptions->nFullscreen;
	int zdepth = psOptions->nZDepth;

	switch (bpp) {
	    case 8:
			rgb_size[0] = 2;
			rgb_size[1] = 3;
			rgb_size[2] = 3;
			break;
	    case 15:
	    case 16:
			rgb_size[0] = 5;
			rgb_size[1] = 5;
			rgb_size[2] = 5;
		break;
        
		default:
			rgb_size[0] = 8;
			rgb_size[1] = 8;
			rgb_size[2] = 8;
		break;
	}
	
	rgb_size[0] = 0;
	rgb_size[1] = 0;
	rgb_size[2] = 0;
	rgb_size[3] = 0;	

	writeLog("Initializing OpenGL\n");
	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, rgb_size[0] );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, rgb_size[1] );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, rgb_size[2] );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, rgb_size[3] );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, zdepth );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	if(fullscreen)
		flags |= SDL_FULLSCREEN;

	SDL_Surface *scr = SDL_SetVideoMode( 800, 600, bpp, flags );
	if(scr == NULL) {
		writeLog("Error: Failed to initialize the video\n");
		writeLog("  SDL Returns:  %s\n",SDL_GetError());
		return false;
	}

	SDL_ShowCursor(SDL_DISABLE);
    SDL_EnableUNICODE(1);
	SDL_WM_SetCaption("Street Rod 3","Street Rod 3");
	
	writeLog("   Screen BPP : %d\n", scr->format->BitsPerPixel);
	writeLog("   Vendor     : %s\n", glGetString( GL_VENDOR ) );
	writeLog("   Renderer   : %s\n", glGetString( GL_RENDERER ) );
	writeLog("   Version    : %s\n", glGetString( GL_VERSION ) );
	//writeLog("   Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
	writeLog("\n");


    if( strstr((char *)glGetString( GL_EXTENSIONS ), "GL_ARB_multitexture") != 0 && psOptions->nMultiTextureEnabled ) {
        writeLog("   Using 'GL_ARB_multitexture' extension\n");
        
        glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
        glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	    glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
    
        GL_TEXTURE0 = GL_TEXTURE0_ARB;
	    GL_TEXTURE1 = GL_TEXTURE1_ARB;
        bMultiTexture = true;
    } else {
        writeLog("   Warning: 'GL_ARB_multitexture' extension not found! Lightmaps will not be used\n");

        glActiveTextureARB = NULL;
        glMultiTexCoord2fARB = NULL;
        glClientActiveTextureARB = NULL;
        bMultiTexture = false;
    }

    

    writeLog("================================================\n");

	return true;
}


///////////////////
// Shutdown the system
void System_Shutdown(void)
{
	writeLog("\nShutting down the system...\n");
	Cache_Shutdown();
    ShutdownOptions();

	BASS_Free();
	closeLog();
	SDL_Quit();
}






///////////////////
// Get the keyboard
keyboard_t *System_GetKeyboard(void)
{
	return &tKeyboard;
}


///////////////////
// Get the mouse
mouse_t *System_GetMouse(void)
{
	return &tMouse;
}


///////////////////
// Get the event
SDL_Event *System_GetEvent(void)
{
    return &sdlEvent;
}


///////////////////
// Multitexture enabled?
bool System_MultiTextureEnabled(void)
{
    return bMultiTexture;
}


int		Frames = 0;
float	OldFPSTime = 0;
int		Fps = 0;

///////////////////
// Get the frames per second count
int GetFPS(void)
{
	Frames++;

	if(((float)SDL_GetTicks() * 0.001f) - OldFPSTime >= 1.0f) {
		OldFPSTime = (float)SDL_GetTicks() * 0.001f;
		Fps = Frames;
		Frames = 0;
	}

	return Fps;
}


///////////////////
// Take a screenshot
void System_TakeScreenshot(void)
{
    byte		*buffer;
	char		picname[80];
	int			i, c, temp;
	FILE		*f;

    
    // Find a filename not used
	strcpy(picname,"screen000.tga");

	for(i=0; i<1000; i++) { 
		picname[6] = i/100 + '0'; 
        picname[7] = i/10 + '0'; 
		picname[8] = i%10 + '0';
		f = fopen (picname, "rb");
		if( !f )
			break;	// file doesn't exist
		fclose (f);
	} 

    // No free files
	if(i==1000)
		return;

    int ResW = 800;
    int ResH = 600;

    // Save the TGA
    buffer = new byte[ ResW*ResH*3 + 18 ];
	memset (buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = ResW & 255;
	buffer[13] = ResW >> 8;
	buffer[14] = ResH & 255;
	buffer[15] = ResH >> 8;
	buffer[16] = 24;	// pixel size
    
	glReadPixels(0, 0, ResW, ResH, GL_RGB, GL_UNSIGNED_BYTE, buffer+18 ); 

	// swap rgb to bgr
	c = 18+ResW*ResH*3;
	for(i=18; i<c ;i+=3) {
		temp = buffer[i];
		buffer[i] = buffer[i+2];
		buffer[i+2] = temp;
	}

	f = fopen(picname, "wb");
	fwrite(buffer, 1, c, f);
	fclose (f);

	delete[] buffer;
}