#include "Controller.h"

double vRed[3]={1,0,0};
double vGreen[3]={0,1,0};
double vBlue[3]={0,0,1};
double vWhite[3]={1,1,1};

Controller::Controller()
{
	this->ren=NULL;
	WisDown=false;
	AisDown=false;
	SisDown=false;
	DisDown=false;
	SpaceIsDown=false;
}

Controller::~Controller()
{
	if(ren)
		delete ren;
}

void Controller::init(int winWidth, int winHeight, HDC hdc)
{
	if(!ren)
	{
		ren=new myRenderer(winWidth,winHeight,hdc);

		ren->SetOptions(FILL | DEPTH_TEST);
		Material m1;
		m1=newMaterial(0.02,0.151,0.2,0,1,0.8);
		double newColor[3]={0.8,0.6,0.3};
		ren->ReadSTL("H:/zhy's/projects/CG/graphics_task/build/zhy2.stl",m1,newColor);
		ren->ReadSTL("H:/zhy's/projects/CG/graphics_task/build/traffic_cone.stl",m1,newColor,Matrix4(SCALE,8));
		/*ren->AddVertex(Vector(0,0),m1,vRed,true);
		ren->AddVertex(Vector(200,0),m1,vGreen,true);
		ren->AddVertex(Vector(200,200),m1,vBlue,true);
		ren->AddVertex(Vector(100,100,-100),m1,vWhite,true);

		ren->AddTriangle(0,2,1);
		ren->AddTriangle(0,1,3);
		ren->AddTriangle(0,3,2);
		ren->AddTriangle(1,2,3);*/
		double Solar[3]={10,10,10};
		ren->setAmbient(3,3,3);
		ren->setLightSource(Vector(-1,-1,-1),Solar);
		ren->getCamera()->drawBack(-300);
		ren->getCamera()->pitch(1.5);
		ren->getCamera()->setPosition(Vector(30,30,30));

		ren->calIllumination();
	}
}

void Controller::setDC(HDC hDC)
{
	if(!ren)
		return;

	ren->setHDC(hDC);
}
void Controller::setKeyState(char key, bool state)
{
	switch(key)
	{
		case 'A':
			AisDown=state;
			break;
		case 'S':
			SisDown=state;
			break;
		case 'D':
			DisDown=state;
			break;
		case 'W':
			WisDown=state;
			break;
		case VK_SPACE:
			SpaceIsDown=state;
			break;
	}
}

void Controller::setMouseState(int button, bool state, LPARAM lParam)
{
	switch(button)
	{
	case BUTTON_LEFT:
		break;
	case BUTTON_RIGHT:
		rButtonIsDown=state;
		if(state)
		{
			old_mouseX=LOWORD(lParam);
			old_mouseY=HIWORD(lParam);
		}
		break;
	}
}

void Controller::mouseMove(LPARAM lParam)
{
	if(rButtonIsDown)
	{
		old_mouseX=mouseX;
		old_mouseY=mouseY;

		mouseX=LOWORD(lParam);
		mouseY=HIWORD(lParam);

		int dx=mouseX-old_mouseX;
		int dy=mouseY-old_mouseY;

		ren->getCamera()->pitch(dy/50.0);
		ren->getCamera()->rotateByTarget(dx/50.0);
	}
}

void Controller::display()
{
	if(!ren)
		return;

	Vector pos=ren->getCamera()->getPosition();
	Vector direction=ren->getCamera()->getFacingDirection();
	if(AisDown)
	{
		ren->getCamera()->yaw(-0.05);
	}
	if(DisDown)
	{
		ren->getCamera()->yaw(0.05);
	}
	if(WisDown)
	{
		ren->getCamera()->setPosition(pos+50*direction);
	}
	if(SisDown)
	{
		//ren->getCamera()->setPosition(pos-5*direction);
	}

	ren->Render();
	ren->clearBuffer();
}