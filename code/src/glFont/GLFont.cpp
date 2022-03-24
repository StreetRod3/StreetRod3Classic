/*
   Copyright (C) 2000 Nate Miller nathanm@uci.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/

#include "defs.h"
#include "3D.h"
#include "GLFont.h"
#include "Utility.h"

using namespace platformUtility;

namespace platformGL { 

char GLFont::buffer[maxLen + 1] = {'\0'};
static const GLubyte white[4] =   {255, 255, 255, 255};
static const GLubyte gray[4]  =   {128, 128, 128, 255};

/*
=============
Setup
=============
   Sets up the coordinate system that will be used when drawing the characters.
*/
void GLFont::fontCoord_t::Setup(GLint l, GLint r, GLint b, GLint t)
{
   right = l < r ? 1 : -1;
   up    = b < t ? 1 : -1;

   dim[0] = l;
   dim[1] = r;
   dim[2] = b;
   dim[3] = t;
}

/*
=============
GLFont
=============
   The amazing GLFont constructor.
*/
GLFont::GLFont()
{
   tTexture = NULL;
   size = 12;
   tIncX = tIncY = 0.0f;
   modeMan = projMan = coordCustom = 0;
   Reset();
}

/*
=============
Reset
=============
   All variables that are not held over a GLFont::Draw() call are reset here.  
*/
void GLFont::Reset(void)
{
   bold = 0;      
   italic = 0;
   gradient = 0;
   shadow = 0;
   region = 0;
   regionDims[0] = regionDims[1] = regionDims[2] = regionDims[3] = 0;
   ColorCopy(colorForGrnd, white, 4);
   ColorCopy(colorGrad, gray, 4);
   ColorCopy(colorShadow, gray, 4);
}

/*
=============
Load
=============
   Try and load the font image and then build the font map.  Returns 0 if it
fails.  'row' and 'col' are optional variables.  
*/
bool GLFont::Load(char *name, GLint row, GLint col)
{
   if (!name || row < 0 || col < 0)
      return 0;

   tTexture = Cache_LoadTexture(name);

   if(tTexture == NULL)
	   return 0;

   tTexture->iMipmaps = false;
   Tex_Upload(tTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

 
   if ((row && col) && (row * col) == 256)
      MakeFontMap(row, col);
   else
      MakeFontMap(fontBlockRow, fontBlockCol);

   return 1;
}

/*
=============
Draw
=============
   Draws a string.  We setup the GL states and projection if needed and then 
render the string.  After the string has been drawn we undo all of the state
and projection changes if needed.  If letting the font code handle the
projection (0, 0) will be the lower left hand corner of the window and (w, h)
will be the upper right hand corner of the window.  
*/
void GLFont::Draw(GLint xpos, GLint ypos, char *format, ...)
{
   va_list	msg;

   va_start(msg, format);
#ifdef _WIN32
   //_vsntprintf(buffer, maxLen, format, msg);	
   vsprintf(buffer, format, msg); // not safe
#else
   // linux
   vsnprintf(buffer, maxLen, format, msg);	
#endif
   va_end (msg);
   
   if (!modeMan)
      glState.GetStates();

   if (!projMan)
   {
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();


      if (!coordCustom)
      {
         GLint vPort[4];
         glGetIntegerv(GL_VIEWPORT, vPort);
         /*
            06/16/01
            Thanks to Reid Simmons(reids+@cs.cmu.edu) for this bug fix.
         */
         //glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
         glOrtho(vPort[0], vPort[0] + vPort[2], vPort[1], vPort[1] + vPort[3], -1, 1);
      }
      else
         glOrtho(coord.dim[0], coord.dim[1], coord.dim[2], coord.dim[3],
          -1, 1);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
   }

   Tex_Bind(tTexture);
   WalkString(xpos, ypos, buffer);

   if (!projMan)
   {
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();   
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
   }
   
   if (!modeMan)
      glState.SetStates();
   Reset();
}

/*
=============
WalkString
=============
   Walks the passed string and either renders or changes some font state based
on what it encounters.  Makes sure that failed formatters don't break anything.
*/
void GLFont::WalkString(GLint xpos, GLint ypos, char *string)
{
   GLint px = region ? regionDims[0] : xpos;
   GLint py = region ? regionDims[1] - (coord.up * size) : ypos;

   for (char *where = string; *where; where++)
   {
      if (region)
      {
         // ugly mess
         if ((coord.right > 0 && (px + size) > (regionDims[0] + regionDims[2]))
         || (coord.right < 0 && (px - size) < (regionDims[0] - regionDims[2])))
         {
            py -= (size * coord.up);
            px = regionDims[0];
         }

         if (coord.up < 0 && py > (regionDims[1] + regionDims[3]) || 
           coord.up > 0 && py < (regionDims[1] - regionDims[3]))
            break;
      }

      if (*where == '\t')
         px += (size * fontTabSize);
      else if (*where == '\n')
      {
         py -= (coord.up * size);
         px = region ? regionDims[0] : xpos;
      }
      else if (*where == '\\')
      {
         GLubyte clr[4];
         GLint format = FormatCheck(&where, clr);
         
         if (!format)
         {
            ProcessChar(px, py, *where);
            px += (coord.right * size);
         }
         else if (format == formatBoldOn)
            bold = 1;
         else if (format == formatBoldOff)
            bold = 0;
         else if (format == formatItalicOn)
            italic = 1;
         else if (format == formatItalicOff)
            italic = 0;
         else if (format == formatColor || format == formatColorA)
            ColorCopy(colorForGrnd, clr, 4);
      }
      else
      {
         ProcessChar(px, py, *where);
         px += (coord.right * size);
      }
   }
}

/*
=============
ProcessChar
=============
   This function decides how many times a given character needs to be rendered.
If there is no shadow or bold the character is only rendered once.  
*/
void GLFont::ProcessChar(GLint x, GLint y, char c)
{
   if (shadow)
   {
      if (!bold)
         DrawChar(x + (fontShadowShift * coord.right),
          y + (fontShadowShift * coord.up), c);
      else
         DrawChar(x + ((fontShadowShift + fontBoldShift) * coord.right),
          y + (fontShadowShift * coord.up), c);
   }

   GLboolean s = shadow;
   
   shadow = 0;

   DrawChar(x, y, c);

   if (bold)
      DrawChar(x + (fontBoldShift * coord.right), y, c);
   
   shadow = s ? 1 : 0;
}

/*
=============
DrawChar
=============
   Draws a character.  Uses the integer value of the character to index in to
the font map.  Italic drawing is handled here as well, italic just offsets the 
top of the font character.  
*/
void GLFont::DrawChar(GLint x, GLint y, char c)
{
   GLint ci = c;
   GLint ital = italic ? fontItalicShift : 0;

   if (!gradient && !shadow)
      glColor4ubv(colorForGrnd);
   else if (!gradient && shadow)
      glColor4ubv(colorShadow);
   else if (gradient && !shadow)
      glColor4ubv(colorGrad);

   glBegin (GL_TRIANGLE_FAN);
      glTexCoord2f(tPoints[ci][0], tPoints[ci][1]);
      glVertex2i(x, y);

      glTexCoord2f(tPoints[ci][0] + tIncX, tPoints[ci][1]);
      glVertex2i(x + (size * coord.right), y);

      if (!shadow)
         glColor4ubv(colorForGrnd);
      else 
         glColor4ubv(colorShadow);

      glTexCoord2f(tPoints[ci][0] + tIncX, tPoints[ci][1] + tIncY);
      glVertex2i(x + ((size + ital) * coord.right), y + (size * coord.up));

      glTexCoord2f(tPoints[ci][0], tPoints[ci][1] + tIncY);
      glVertex2i(x + (ital * coord.right), y + (size * coord.up));
   glEnd ();
}

/*
=============
FormatCheck
=============
   This is called when we hit a \ in the string.  It checks for \b+, \b-, \i+, 
\i-, \c(int int int) and \a(int int int int).  It is very picky about things so
if there is anything wrong with the passed string it will return 0, an error.  
If everything is parsed without problem it will return an enum from font.h that
represents what was just read in.  
*/
GLint GLFont::FormatCheck(char **where, GLubyte *clr)
{
   if (**where != '\\')
      return 0;

   GLint res = 0;
   char *start = *where;
   char curr;

   if (!*++(*where))
   {
      *where = start;
      return 0;
   }

   curr = **where;

   if (curr == 'b' || curr == 'i')
   {
      GLboolean b = curr == 'b';
      res = IsToggle(where);

      if (res == toggleOn)
         return b ? formatBoldOn : formatItalicOn;
      else if (res == toggleOff)
         return b ? formatBoldOff : formatItalicOff;
   }
   else if ((curr == 'c' || curr == 'a') &&
    ParseColor(clr, where, curr == 'c' ? 3 : 4))
      return curr == 'c' ? formatColor : formatColorA;

   *where = start;

   return 0;
}

/*
=============
IsToggle
=============
   Determins if we have a toggle character, + or -.  Returns an enum from
font.h if all is well and 0 otherwise.
*/
GLint GLFont::IsToggle(char **where)
{  
   if (!*++(*where))
      return 0;
   if (**where == '+')
      return toggleOn;
   else if (**where == '-')
      return toggleOff;
   return 0;
}

/*
=============
ParseColor
=============
   Parses a color out of the string.  It is very picky, one problem and
everything is considered wrong.  Scans (int int int) and (int int int int).
*/
GLboolean GLFont::ParseColor(GLubyte *clr, char **where, GLint num)
{
   GLubyte temp[4] = {0, 0, 0, 255};

   if (!*++(*where) || *(*where)++ != '(')
      return 0;
   
   if (!**where)
      return 0;

   for (int i = 0; i < num - 1; i++)
   {   
      temp[i] = Utility::Clamp(Utility::LineReadInt(where), 0, 255);
      
      if (!**where || *(*where)++ != ' ')
         return 0;
   }

   temp[num - 1] = Utility::Clamp(Utility::LineReadInt(where), 0, 255);
   
   if (**where && **where == ')')
   {
      ColorCopy(clr, temp, 4);
      return 1;
   }
   
   return 0;
}

/*
=============
MakeFontMap
=============
   Makes the font map that is used when rendering characters.
*/
void GLFont::MakeFontMap(GLint row, GLint col)
{
   GLint i = 0;
   GLfloat x, y;

   tIncX = (GLfloat)pow(col, -1);
   tIncY = (GLfloat)pow(row, -1);

   for (y = 1 - tIncY; y >= 0; y -= tIncY)
   {
      for (x = 0; x <= 1 - tIncX; x += tIncX, i++)
      {
         tPoints[i][0] = x;
         tPoints[i][1] = y;
      }
   }
}

/*
=============
GetStates
=============
   Gets the states we change from OpenGL so we can restore them.
*/
void GLFont::fontGLRenderState_t::GetStates(void)
{
   textureOn = glIsEnabled(GL_TEXTURE_2D);
   
   if (!textureOn)
      glEnable(GL_TEXTURE_2D);

   depthOn = glIsEnabled(GL_DEPTH_TEST);

   if (depthOn)
      glDisable(GL_DEPTH_TEST);

   lightingOn = glIsEnabled(GL_LIGHTING);        

   if (lightingOn) 
      glDisable(GL_LIGHTING);

   glGetIntegerv(GL_MATRIX_MODE, &matrixMode); 

   blendOn= glIsEnabled(GL_BLEND);        
   glGetIntegerv(GL_BLEND_SRC, &blendSrc);
   glGetIntegerv(GL_BLEND_DST, &blendDst);

   if (!blendOn)
      glEnable(GL_BLEND);

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*
=============
SetStates
=============
   Restores the states we changed.
*/
void GLFont::fontGLRenderState_t::SetStates(void)
{
   if (lightingOn)
      glEnable(GL_LIGHTING);

   if (!blendOn)
   {
      glDisable(GL_BLEND);
      glBlendFunc(blendSrc, blendDst);
   }
   else
      glBlendFunc(blendSrc, blendDst);

   if (depthOn)
      glEnable(GL_DEPTH_TEST);

   if (!textureOn)
      glDisable(GL_TEXTURE_2D);

   //glMatrixMode(matrixMode);
}

}