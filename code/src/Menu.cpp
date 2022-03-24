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


// Menu system
// Created 4/3/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "game.h"
#include "Menu.h"
#include "System.h"
#include "Renderer.h"
#include "gui.h"

void    Menu_MainOptions(CGuiLayout *pcLayout);



texture_t *psLogo = NULL;

///////////////////
// Initialize the menu system
// Returns:
// True - Start game
// False - Quit game
int Menu_Initialize(bool bShowLogo)
{
    if(psOptions->nDeveloper)
        return true;

    psLogo = Cache_LoadTexture("data/menu/logo.png");
    // 17/06/05
	if(psLogo)
		Tex_Upload(psLogo);	//This was here originally but without "if-else"
	else
	{
		MessageBox(NULL,"Fatal Error: Could not find Menu's Texture","Menu Error",MB_ICONEXCLAMATION|MB_OK);
		writeLog("Fatal Error: Could not find Menu's Texture");
	}
	//

    // Logo
    if(bShowLogo)
        Menu_Logo();

    // Main menu
    int nResult = Menu_Main();

    switch(nResult) {
        case MNU_NEWGAME:
            // Enter name
            Menu_EnterName();
			Game_NewGame();
            return true;

        case MNU_LOADGAME:
            return true;

        case MNU_QUIT:
            return false;
    }

    // Odd.. Shouldn't get here
    return false;
}



///////////////////
// Do the front logo
void Menu_Logo(void)
{
    texture_t *psAuxTitle = Cache_LoadTexture("data/menu/auxtitle.png");
    Tex_Upload(psAuxTitle);
    texture_t *psSR3Title = Cache_LoadTexture("data/menu/sr3title.png");
    Tex_Upload(psSR3Title);

    // TODO: Check if the textures loaded

    State_Disable(GL_LIGHTING);
    State_Disable(GL_FOG);
    State_Enable(GL_TEXTURE_2D);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int hw = 800/2-40;
    int hh = 600/2;

    int stage = 0;
    float alpha1 = 0;
    float alpha2 = 0;
    float count = 0;


    while(1) {
        System_ProcessInput();

        SetupFrame();

        if( System_GetKeyboard()->KeyUp[SDLK_ESCAPE] && alpha2 > 0.5f )
            break;

        if( stage == 0 ) {
            alpha1 += tMainSR3.fDeltaTime/2;
            alpha2 += tMainSR3.fDeltaTime/2;
            if(alpha1 >= 1) {
                alpha1 = 1;
                alpha2 = 1;
                count += tMainSR3.fDeltaTime;
                if( count > 3 ) {
                    stage = 1;
                    count = 0;
                }
            }
        }

        if( stage == 1 ) {
            alpha1 -= tMainSR3.fDeltaTime/2 * 0.7f;
            alpha2 -= tMainSR3.fDeltaTime/2;

            if(alpha2 <= 0) {
                alpha1 = 0.3f;
                alpha2 = 0;
                break;
            }
        }

        Render_Begin2D();

        hw = 800/2-45;
        hh = 600/2;

        glColor4f(1,1,1,alpha1);


        Tex_Bind(psLogo);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0);  glVertex2f(hw-psLogo->iWidth/2, hh-psLogo->iHeight/2);
            glTexCoord2f(1,0);  glVertex2f(hw+psLogo->iWidth/2, hh-psLogo->iHeight/2);
            glTexCoord2f(1,-1);  glVertex2f(hw+psLogo->iWidth/2, hh+psLogo->iHeight/2);
            glTexCoord2f(0,-1);  glVertex2f(hw-psLogo->iWidth/2, hh+psLogo->iHeight/2);
        glEnd();

        hw = 800/2;
        hh = 600/2-150;

        glColor4f(1,1,1,alpha2);

        Tex_Bind(psSR3Title);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0);  glVertex2f(hw-psSR3Title->iWidth/2, hh-psSR3Title->iHeight/2);
            glTexCoord2f(1,0);  glVertex2f(hw+psSR3Title->iWidth/2, hh-psSR3Title->iHeight/2);
            glTexCoord2f(1,-1);  glVertex2f(hw+psSR3Title->iWidth/2, hh+psSR3Title->iHeight/2);
            glTexCoord2f(0,-1);  glVertex2f(hw-psSR3Title->iWidth/2, hh+psSR3Title->iHeight/2);
        glEnd();

        hw = 800/2;
        hh = 600/2-220;

        glColor4f(1,1,1,alpha2);

        Tex_Bind(psAuxTitle);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0);  glVertex2f(hw-psAuxTitle->iWidth/2, hh-psAuxTitle->iHeight/2);
            glTexCoord2f(1,0);  glVertex2f(hw+psAuxTitle->iWidth/2, hh-psAuxTitle->iHeight/2);
            glTexCoord2f(1,-1);  glVertex2f(hw+psAuxTitle->iWidth/2, hh+psAuxTitle->iHeight/2);
            glTexCoord2f(0,-1);  glVertex2f(hw-psAuxTitle->iWidth/2, hh+psAuxTitle->iHeight/2);
        glEnd();


        Render_End2D();

        SDL_GL_SwapBuffers();
    }
}


///////////////////
// The main menu
// Returns menu mode
int Menu_Main(void)
{
    CModel      *pcSRText = Cache_LoadModel("data/menu/srtext");
    CModel      *pcSRText3 = Cache_LoadModel("data/menu/srtext_3");
    texture_t   *psTitles[8];
    CCamera     cCam;
    int         i;
    float       lpos[4] = {500,500,100,1};
    mouse_t     *psMouse = System_GetMouse();
    texture_t   *psGlow = NULL;


    // Load the titles
    psTitles[0] = Cache_LoadTexture("data/menu/newgame.png");
    psTitles[1] = Cache_LoadTexture("data/menu/loadgame.png");
    psTitles[2] = Cache_LoadTexture("data/menu/options.png");
    psTitles[3] = Cache_LoadTexture("data/menu/quit.png");
    psTitles[4] = Cache_LoadTexture("data/menu/newgame_g.png");
    psTitles[5] = Cache_LoadTexture("data/menu/loadgame_g.png");
    psTitles[6] = Cache_LoadTexture("data/menu/options_g.png");
    psTitles[7] = Cache_LoadTexture("data/menu/quit_g.png");
    for(i=0; i<8; i++)
        Tex_Upload(psTitles[i]);

    psGlow = Tex_Create(256,256);
    //psGlow->iMipmaps = false;
    Tex_Upload(psGlow);

    State_Disable(GL_LIGHTING);
    State_Disable(GL_FOG);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
    
    cCam.Setup( CVec(0,-450,0), CVec(0,0,0) );
    
    float phase = 0;

    while(1) {
        System_ProcessInput();

        //
        // Render to texture
        //
        /*glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();

	    // Setup the viewport (shouldn't need to do this every frame)
        int w = 256;
        int h = 256;
	    gluPerspective(45.0f,1.333f,1,2048.0f);
	    glViewport(0,0,256,192);


	    // Setup the modelview matrix
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
        glClearColor(0,0,0,0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	    

	    // Swap to the SEA(?) coordinate system
	    glTranslatef(0.0,0.0,0.0);
	    glRotatef (-90,  1, 0, 0);
	    glRotatef (90,  0, 0, 1);

        cCam.Setup( CVec(0,-450,0), CVec(0,0,0) );
        cCam.Show();

        State_Disable(GL_BLEND);
        glColor4f(1,1,1,1);
        // The 'Street Rod' text

        
        glPushMatrix();
        float s = 0.2;
        glTranslatef(0,0,130);
        //glScalef(s,s,s);
        glRotatef(15,1,0,0);
        glRotatef(sin(phase * (180/PI))*2,0,0,1);
        glColor4f(1,1,1,1);
        State_Disable(GL_LIGHTING);
        State_Disable(GL_TEXTURE_2D);
        pcSRText->renderRaw();
        glPopMatrix();

        State_Enable(GL_TEXTURE_2D);
        Tex_Bind(psGlow);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
							0,
							64,
							0, 0, 256, 192);


        //glViewport(0,0,800,600);
        Render_Begin2D();
        for(int blah=0;blah<6;blah++) {
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	    
        //State_Disable(GL_CULL_FACE);
        glColor4f(1,1,1,0.65f);
        float wt = 256;
        float ht = 192;
        float ht2 = 256;
        float st = 0.01f;
        DrawPicEx(psGlow,-wt*st,-ht2*st-64,wt+wt*st,ht+ht*st);

        glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
							0,
							64,
							0, 0, 256, 192);

        DrawPicEx(psGlow,0,0,256,192);
        }
        Render_End2D();*/




        cCam.Setup( CVec(0,-450,0), CVec(0,0,0) );

        // Setup the rendering
        glLightfv(GL_LIGHT0, GL_POSITION, lpos);
        SetupFrame();
        cCam.Show();
                
        Render_Begin2D();

        glColor4f(1,1,1,0.3f);
        DrawPic( psLogo, 355-psLogo->iWidth/2, 300+psLogo->iHeight/2);


        // Titles
        for(i=0; i<4; i++) {

            bool onButton = false;
            if(psMouse->X > 20 && psMouse->X < 276) {
                if(600-psMouse->Y > 300 - (i*75)-32 && 600-psMouse->Y < 300 - (i*75)) {

                    if(psMouse->Up & SDL_BUTTON(1)) {
                        // Do the menu command
                        switch(i) {
                            // New game
                            case 0:
                                Render_End2D();
                                return MNU_NEWGAME;
                                break;

                            // Load game
                            case 1:
                                Render_End2D();

                                if(Menu_LoadGame())
                                    return MNU_LOADGAME;

                                Render_Begin2D();
                                break;

                            // Options
                            case 2:                                
                                Render_End2D();
                                
                                // Options
                                Menu_Options();

                                Render_Begin2D();
                                break;


                            // Quit
                            case 3:
                                Render_End2D();
                                return MNU_QUIT;
                                break;
                        }
                    }

                    onButton = true;
                    float a = 0.5+0.5*sin( tMainSR3.fCurTime / 0.3f );
                    if(a>1) a=1;
                    if(a<0) a=0;

                    glColor4f(1,1,1,a);
                    DrawPic( psTitles[i+4], 20, 300 - (i*75));
                }
            }
            
            //if(!onButton) {
                glColor4f(1,1,1,1);
                DrawPic( psTitles[i], 20, 300 - (i*75));
            //}
        }

        
        // Versopn number
        Font_SetSize(10);
        Font_Draw(650,10, CVec(1,1,1), "Version %.2fa",VERSION);

        Render_End2D();                
        
        phase += tMainSR3.fDeltaTime/150;
        if(phase > 360)
            phase -= 360;

        //SDL_GL_SwapBuffers();
        //continue;


        State_Enable(GL_BLEND);
        State_Disable(GL_LIGHTING);
        glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
        
        // The 'Street Rod' text
        glPushMatrix();
        glTranslatef(0,0,130);
        glRotatef(15,1,0,0);
        glRotatef(sin(phase * (180/PI))*2,0,0,1);



        // Model
        State_Disable(GL_BLEND);
        pcSRText->render();
        glPopMatrix();

        // The '3' text
        glPushMatrix();
        //glScalef(2,2,2);
        glTranslatef(0,0,70);
        glRotatef(10,1,0,0);
        pcSRText3->render();
        glPopMatrix();






        

        
        Render_Begin2D();

        // Tris drawn
        Font_SetAlpha(1);
        Font_SetSize(14);
        //Font_Draw(617, 563, CVec(0,0,0), "Tris: %d", tMainSR3.TriCount ); 
        //Font_Draw(615, 565, CVec(1,1,1), "Tris: %d", tMainSR3.TriCount );

        State_Disable(GL_BLEND);
        State_Disable(GL_CULL_FACE);
        State_Enable(GL_TEXTURE_2D);
        glColor4f(1,1,1,1);

        gui_DrawMouse();
        Render_End2D();
        
        SDL_GL_SwapBuffers();
    }

    // Shouldn't get here
    return MNU_NEWGAME;
}



// Option widgets
enum {
    opt_done,
    opt_fullscreen,
    opt_16bpp,
    opt_32bpp,

    opt_soundon,
    opt_soundvol,
    opt_musicvol,

    opt_sensitivity,
    opt_invertx,
    opt_inverty
};

int     nWaitControl = -1;


///////////////////
// Options
void Menu_Options(void)
{
    mouse_t     *psMouse = System_GetMouse();
    keyboard_t  *psKeyb = System_GetKeyboard();
    CGuiLayout  cOptions, cDone;
    texture_t   *psTitles = NULL;
    bool        bProcess = true;
    texture_t   *psArrows[2];
    int         nPageNum = 0;
    
    nWaitControl = -1;

    // Load textures
    psArrows[0] = Cache_LoadTexture("data/gui/arrow.png");
    psArrows[1] = Cache_LoadTexture("data/gui/arrow2.png");
    psTitles = Cache_LoadTexture("data/gui/book_titles.png");
    Tex_Upload(psTitles);
    Tex_Upload(psArrows[0]);
    Tex_Upload(psArrows[1]);

    cDone.Initialize();
    cDone.Add(new CButton("Done",0),                            opt_done,       600, 522, 125,20);

    cOptions.Initialize();
    cOptions.Add(new CCheckbox(psOptions->nFullscreen),         opt_fullscreen, 350, 100, 32, 32);
    cOptions.Add(new CCheckbox(psOptions->nColourDepth==16),    opt_16bpp,      350, 130, 32, 32);
    cOptions.Add(new CCheckbox(psOptions->nColourDepth==32),    opt_32bpp,      500, 130, 32, 32);
    cOptions.Add(new CCheckbox(psOptions->nSoundOn),            opt_soundon,    350, 200, 32, 32);
    cOptions.Add(new CSlider(100,psOptions->nSoundVolume),      opt_soundvol,   350, 235, 150,20);
    cOptions.Add(new CSlider(100,psOptions->nMusicVolume),      opt_musicvol,   350, 270, 150,20);
    cOptions.Add(new CSlider(100,psOptions->fRotateSensitivity*10),opt_sensitivity,350, 355, 150,20);
    cOptions.Add(new CCheckbox(psOptions->nInverseX),           opt_invertx,    350, 385, 32, 32);
    cOptions.Add(new CCheckbox(psOptions->nInverseY),           opt_inverty,    350, 415, 32, 32);


    Game_InitBook();

    while(bProcess) {
        System_ProcessInput();
        SetupFrame();

        Render_Begin2D();

        Game_DrawBook();
        
        // Book Title
        glColor4f(1,1,1,1);
        State_Disable(GL_BLEND);
        BlitTexture(psTitles, 0,90, 300,20, 256,30, 1,1);
        
        // Arrows
        State_Disable(GL_CULL_FACE);
        Font_SetAlpha(1);
        Font_SetSize(14);

        // LEFT arrow
        glColor4f(1,1,1,1);
        if( MouseinRect(300,510, 364,542) && nWaitControl == -1 ) {
            if( psMouse->Up )
                nPageNum = MAX(nPageNum-1,0);
            glColor4f(0,1,0,1);
        }
        DrawPic( psArrows[0], 300, 90 );


        // RIGHT Arrow
        glColor4f(1,1,1,1);
        if( MouseinRect(470,510, 534,542) && nWaitControl == -1 ) {
            if( psMouse->Up )
                nPageNum = MIN(nPageNum+1,1);
            glColor4f(0,1,0,1);
        }
        DrawPic( psArrows[1], 470, 90 );

        Font_SetSize(12);
        Font_DrawCentered(415, 67, CVec(0,0,0), "Page %d / 2",nPageNum+1);

        // Restore the old states
        State_Enable(GL_CULL_FACE);

        // Main options
        if(nPageNum == 0 ) {
            Menu_MainOptions(&cOptions);
        } else {
            Menu_ControlOptions();
        }

        
        cDone.Draw();
        gui_event_t *ev = cDone.Process();
        if(ev) {
            switch(ev->iControlID) {
                case opt_done:
                    if(ev->iEventMsg == BUT_CLICKED) {
                        
                        //
                        // Get the options
                        //

                        // Video
                        psOptions->nFullscreen = ((CCheckbox *)cOptions.getWidget(opt_fullscreen))->getCheck();
                        if(((CCheckbox *)cOptions.getWidget(opt_16bpp))->getCheck())
                            psOptions->nColourDepth = psOptions->nZDepth = 16;
                        else
                            psOptions->nColourDepth = psOptions->nZDepth = 32;

                        // Sound
                        psOptions->nSoundOn = ((CCheckbox *)cOptions.getWidget(opt_soundon))->getCheck();
                        psOptions->nSoundVolume = ((CSlider *)cOptions.getWidget(opt_soundvol))->getValue();
                        psOptions->nMusicVolume = ((CSlider *)cOptions.getWidget(opt_musicvol))->getValue();

                        // Update the sound lib
                        if(psOptions->nSoundOn) {
                            BASS_Start();
	                        BASS_SetVolume(psOptions->nSoundVolume);
                        } else
                            BASS_Stop();

                        // Input
                        psOptions->fRotateSensitivity = (float)((CSlider *)cOptions.getWidget(opt_sensitivity))->getValue() / 10.0f;
                        psOptions->nInverseX = ((CCheckbox *)cOptions.getWidget(opt_invertx))->getCheck();
                        psOptions->nInverseY = ((CCheckbox *)cOptions.getWidget(opt_inverty))->getCheck();

                        // Leave
                        bProcess = false;
                    }
                    break;
            }
        }

        // Mouse
        gui_DrawMouse();
        Render_End2D();

        SDL_GL_SwapBuffers();
    }
}


///////////////////
// Draw & Process the main options
void Menu_MainOptions(CGuiLayout *pcLayout)
{
    Font_SetAlpha(1);
    Font_SetSize(16);
    
    // Video
    Font_DrawShadow(125,500, CVec(0.3f,0.3f,1), CVec(0.6,0.6,0.6), "Video");
    Font_Draw(150,480, CVec(0,0,0), "Fullscreen");
    Font_Draw(250,450, CVec(0,0,0), "16bit");
    Font_Draw(400,450, CVec(0,0,0), "32bit");
    
    // Sound
    Font_DrawShadow(125,400, CVec(0.3f,0.3f,1), CVec(0.6,0.6,0.6), "Sound");
    Font_Draw(150,380, CVec(0,0,0), "Enabled");
    Font_Draw(150,350, CVec(0,0,0), "Volume");
    Font_Draw(150,315, CVec(0,0,0), "Music");
    
    // Mouse
    Font_DrawShadow(125,260, CVec(0.3f,0.3f,1), CVec(0.6,0.6,0.6), "Mouse");
    Font_Draw(150,230, CVec(0,0,0), "Sensitivity");
    Font_Draw(150,195, CVec(0,0,0), "InvertX");
    Font_Draw(150,165, CVec(0,0,0), "InvertY");
    
    

    // Toggle the colour depth checkboxes
    pcLayout->Draw();
    gui_event_t *ev = pcLayout->Process();
    if(ev) {
        switch(ev->iControlID) {
            case opt_16bpp:
                if(ev->iEventMsg == CHK_CHANGED) {
                    bool b16 = ((CCheckbox *)pcLayout->getWidget(opt_16bpp))->getCheck();
                    ((CCheckbox *)pcLayout->getWidget(opt_32bpp))->setCheck(!b16);
                }
                break;
            
            case opt_32bpp:
                if(ev->iEventMsg == CHK_CHANGED) {
                    bool b32 = ((CCheckbox *)pcLayout->getWidget(opt_32bpp))->getCheck();
                    ((CCheckbox *)pcLayout->getWidget(opt_16bpp))->setCheck(!b32);
                }
                break;
        }
    }
}


///////////////////
// Control Options
void Menu_ControlOptions(void)
{
    mouse_t     *ms = System_GetMouse();
    keyboard_t  *kb = System_GetKeyboard();
    int         y = 480;
    int         i;
    char *controls[] = {"Accelerate", "Brake", "Steer Left", "Steer Right", 
                        "Up Gear", "Down Gear", "Change Camera"};
    
    State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);
    Font_SetAlpha(1);
    Font_SetSize(18);
    Font_DrawShadow(125,500, CVec(0.3f,0.3f,1), CVec(0.6,0.6,0.6), "Controls");
    for(i=0; i<7; i++, y-=45) {
        if(MouseinRect(150,600-y,700,600-(y-30)) && nWaitControl == -1) {
            DrawRect(150,y,700,y-30, CVec(0,0,0), CVec(0.7f,0.7f,0.7f), 1,true,0);

            if(ms->Up & SDL_BUTTON(1)) {
                // Change the control
                nWaitControl = i;
                return;
            }
        }

        Font_DrawShadow(150,y-23, CVec(0,0,0), CVec(0.7f,0.7f,0.7f), controls[i]);
        Font_DrawShadow(400,y-23, CVec(0,0,0), CVec(0.7f,0.7f,0.7f), ":   %s", psOptions->szControls[i]);
    }


    // Waiting for a control
    if(nWaitControl == -1)
        return;

    CInput  inp;
    char    szControl[128];

    // Draw a dialog box
    Font_SetSize(16);
    DrawRect(200,390, 600,220, CVec(1,1,1), CVec(0,0,0), 0.75f, true, 0);
    DrawRect(200,390, 600,220, CVec(1,1,1), CVec(0,0,0), 1, false, 1);
    Font_DrawCentered(402,348, CVec(0,0,0), "Waiting for input for:");
    Font_DrawCentered(400,350, CVec(1,1,1), "Waiting for input for:");
    Font_SetSize(18);
    Font_DrawCentered(402,288, CVec(0,0,0), "\"%s\"",controls[nWaitControl]);
    Font_DrawCentered(400,290, CVec(1,1,1), "\"%s\"",controls[nWaitControl]);
    Font_SetAlpha(1);


    // Escaped?
    if(kb->keys[SDLK_ESCAPE]) {
        nWaitControl = -1;
        
        // Flush the controls
        System_InitializeInput();
        return;
    }

    if(inp.Wait(szControl)) {
        sr_strncpy(psOptions->szControls[nWaitControl], szControl, 31);
        nWaitControl = -1;
        
        // Flush the controls
        System_InitializeInput();
    }
}


///////////////////
// Menu: Load game
// Returns true if we loaded the game
bool Menu_LoadGame(void)
{
    InitializeLoadGame();
    System_InitializeInput();

    Game_InitBook();

    while(1) {
        System_ProcessInput();
        SetupFrame();

        Render_Begin2D();

        int nResult = ProcessLoadGame();
        if(nResult == 1)
            return true;
        if(nResult == -1)
            return false;
        
        gui_DrawMouse();
        Render_End2D();

        SDL_GL_SwapBuffers();
    }
    
    return false;
}


///////////////////
// Enter in the name
void Menu_EnterName(void)
{
    texture_t   *psLicense = NULL;
    CGuiLayout  cName;
    gui_event_t *psEvent = NULL;

    psLicense = Cache_LoadTexture("data/menu/license.png");
    Tex_Upload(psLicense);

    

    System_InitializeInput();

    cName.Initialize();
    cName.Add( new CTextbox(),          0, 410,318, 200, 35);
    cName.Add( new CButton("Done",0),   1, 575,410, 128, 32);


    while(1) {
        System_ProcessInput();
        SetupFrame();

        psEvent = cName.Process();
        if(psEvent) {
            if(psEvent->iControlID == 1 && psEvent->iEventMsg == BUT_CLICKED) {
                // Done!
                cMainGuy->setName( ((CTextbox *)cName.getWidget(0))->getText() );
                break;
            }
        }



        Render_Begin2D();

        // Draw the license stuff
        DrawPicEx(psLicense, 80, 460, 720, 140);
        cName.Draw();

        Font_SetSize(16);
        Font_SetAlpha(1);
        
        gui_DrawMouse();
        Render_End2D();

        SDL_GL_SwapBuffers();
    }



}