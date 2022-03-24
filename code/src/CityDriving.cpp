/////////////////////////////////////////
//
//             Street Rod 3
//
//      Copyright Auxiliary Software 2000
//
//
/////////////////////////////////////////


// Main city driving
// Created 12/7/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"
#include "City.h"
#include "font.h"


CCity	*cCity = NULL;
CCamera	cCityCamera;
CCar	cCityCar;


///////////////////
// Handle the main driving process
int City_Start(char *sDir)
{
	keyboard_t		*kb = System_GetKeyboard();	

	// Load the city
	if(!City_Load(sDir))
		return false;

	cCityCamera.Setup(CVec(0,40, 20), CVec(0,0,0));
	State_Enable(GL_FOG);
	//State_Disable(GL_LIGHTING);

	// Fog
	glFogf(GL_FOG_START, 300.0f);
	glFogf(GL_FOG_END, 550.0f);


	// Hack: Load the car
	cCityCar.LoadSetup("data/newspaper/mustang.sr3");
	cCityCar.SetupSimulation();

	CVec c;

	c = cCityCamera.getPos();


	System_ProcessInput();
	while(!kb->KeyUp[SDLK_ESCAPE]) {
		System_ProcessInput();

		// Driving
		City_ProcessDriving();		
		
		State_Disable(GL_LIGHTING);


		// Drawing
		SetupFrame();
		cCityCamera.Show();
		cCityCar.DrawDriving();
		cCity->Draw(&cCityCamera);
		
		cCityCar.Camera_FollowCar(&c, tMainSR3.fDeltaTime);
		if(kb->KeyDown[SDLK_c])
			cCityCamera.setPos(c);


		// 2D stuff
		Render_Begin2D();

		// Debug drawing
		Font_SetSize(10);
		Font_Draw(0,570,CVec(1,1,1),"%f",tMainSR3.f1);
		Font_Draw(0,550,CVec(1,1,1),"%f",tMainSR3.f2);

		// Draw the vector direction
		State_Disable(GL_TEXTURE_2D);
		State_Disable(GL_FOG);
		State_Disable(GL_LIGHTING);
		glColor4f(1,1,1,1);

		VectorNormalize(&tMainSR3.v1);
		VectorNormalize(&tMainSR3.v2);
		
		glBegin(GL_LINES);
		glVertex2f(20,500);
		glVertex2f(20 + (tMainSR3.v1.GetX()*20),500 + (tMainSR3.v1.GetY()*20));
		glEnd();

		glBegin(GL_LINES);
		glVertex2f(20,450);
		glVertex2f(20 + (tMainSR3.v2.GetX()*20),450 + (tMainSR3.v2.GetY()*20));
		glEnd();




		Render_End2D();

		SDL_GL_SwapBuffers();
	}

	
	// Hack to exit
	Game_SetLocation(LOC_EXIT);

	// Shutdown the city
	City_Shutdown();


	return true;
}


///////////////////
// Process the driving
void City_ProcessDriving(void)
{
	// Check collisions between the city track and the wheels
	City_CheckCollisions(&cCityCar, cCity);

	cCityCar.SimulateDriving(tMainSR3.fDeltaTime );
	cCityCamera.LookAt( cCityCar.getCarSim()->getSimBody()->getPosition() );
}


///////////////////
// Check collisions
void City_CheckCollisions(CCar *car, CCity *city)
{
	CWheel				*w;
	contact_t			*cont;
	tile_t				*tile;
	CollisionModel3D	*col, *carcol;
	plane_t				plane;

	CVec p, m;
	float v1[3], v2[3];
	float t1[9],t2[9];
	float tilesize = 48;
	float mat[16];
	int i;


	// Create a 3x3 tile grid around the car
	int x = (int) (car->getCarSim()->getSimBody()->getPosition().GetX() / tilesize);
	int y = (int) (car->getCarSim()->getSimBody()->getPosition().GetY() / tilesize);
	int sx = x - 1;
	int sy = y - 1;
	int ex = x + 1;
	int ey = y + 1;

	// Clipping
	sx = MAX(0,sx);
	sy = MAX(0,sy);
	
	// TODO: City size clipping

	// Set contacts to false
	for(int n=0;n<4;n++)
		car->getCarSim()->getWheel(n)->getContact()->Touching = false;

	car->getCarSim()->getContact()->Touching = false;

	carcol = car->getModel()->GetCDModel();

	for(y=sy; y<=ey; y++) {
		tile = city->getTile(sx,y);
		for(x=sx; x<=ex; x++) {

			if(tile==NULL)
				break;
			if(!tile->iUsed) {
				tile++;
				continue;
			}


			// Tile translation
			col = tile->tLib->cModel->GetCDModel();
			CVec offset = CVec((float)x,(float)y,0) * tilesize + CVec(tilesize/2.0f,tilesize/2.0f,0);
			CreateCDMatrix(mat, offset);

			col->setTransform(mat);



			// Go through the 4 wheels
			for(int n=0;n<4;n++) {
				w = car->getCarSim()->getWheel(n);
				cont = w->getContact();

				// Get the contact ray
				w->getContactRay(car->getCarSim()->getSimBody()->GetSimBody(w->getRelPos()), &p, &m);
		
				// Check collision
				VectorNormalize(&m);
		
				for(i=0;i<3;i++) v1[i] = p.GetIndex(i);
				for(i=0;i<3;i++) v2[i] = m.GetIndex(i);

				if(col->rayCollision(v1,v2,true,0,4)) {
					// Setup the contact
					cont->Touching = true;
					col->getCollisionPoint(v1);
					for(i=0;i<3;i++) cont->Pos.SetIndex(i,v1[i]);

					cont->Pos = cont->Pos + offset + CVec(0,0,2);
			
					col->getCollidingTriangles(t1,t2);

					plane = CalculatePlane(CVec(t1[0],t1[1],t1[2]),
										   CVec(t1[3],t1[4],t1[5]),
										   CVec(t1[6],t1[7],t1[8]));

					cont->Norm = plane.vNormal;
				}
			}


			// Check car collisions
			cont = car->getCarSim()->getContact();

			car->getCarSim()->getSimBody()->createDrawMatrix(mat);
			carcol->setTransform(mat);

			if(carcol->collision( col )) {
				cont->Touching = true;

				// Get collision point
				carcol->getCollisionPoint(v1);
				for(i=0;i<3;i++) cont->Pos.SetIndex(i,v1[i]);

				//cont->Pos = cont->Pos + offset;

				// Get collision normal
				carcol->getCollidingTriangles(t1,t2);

				plane = CalculatePlane(CVec(t1[0],t1[1],t1[2]),
									   CVec(t1[3],t1[4],t1[5]),
									   CVec(t1[6],t1[7],t1[8]));

				cont->Norm = plane.vNormal;
			}

		
			tile++;
		}
	}




}


///////////////////
// Load the city
int City_Load(char *sDir)
{
	cCity = new CCity;
	if(cCity == NULL)
		return false;

	if(!cCity->Load(""))
		return false;

	return true;
}


///////////////////
// Shutdown the city
void City_Shutdown(void)
{
	if(cCity) {
		cCity->Shutdown();
		delete cCity;
		cCity = NULL;
	}
}