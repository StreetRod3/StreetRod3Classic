/////////////////////////////////////////
//
//             Street Rod 3
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Graphical User Interface class
// Created 12/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "CGUI.h"
#include "Cache.h"
#include "System.h"


///////////////////
// Load the gui
int CGUI::Load(void)
{
	texMouse = Cache_LoadTexture("data/gui/mouse2.png");
	if(texMouse == NULL)
		return false;
    Tex_Upload(texMouse);


	return true;
}


///////////////////
// Shutdown the gui
void CGUI::Shutdown(void)
{

}


///////////////////
// Process the gui
void CGUI::Process(void)
{


}


///////////////////
// Draw the gui
void CGUI::Draw(void)
{


}


///////////////////
// Draw the mouse
void CGUI::DrawMouse(void)
{
    mouse_t *psMouse = System_GetMouse();

    State_Disable(GL_TEXTURE_GEN_S);
    State_Disable(GL_TEXTURE_GEN_T);
    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_LIGHTING);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,1);

    Tex_Bind(texMouse);

    glBegin(GL_QUADS);
    glTexCoord2f(0,1);  glVertex2i(psMouse->X, 600-psMouse->Y-32);
    glTexCoord2f(1,1);  glVertex2i(psMouse->X+32, 600-psMouse->Y-32);
    glTexCoord2f(1,0);  glVertex2i(psMouse->X+32, 600-psMouse->Y);
    glTexCoord2f(0,0);  glVertex2i(psMouse->X, 600-psMouse->Y);
    glEnd();
}