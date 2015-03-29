#include "Controller.h"
#include "bmpReader.h"

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

		ren->SetOptions(FILL | DEPTH_TEST | TEXTURE_MAPPING);
		ren->loadNewTexture("data/grass.bmp");

		Material m1;
		m1=newMaterial(0.02,0.151,0.2,0,1,0.8);
		double newColor[3]={0.8,0.6,0.3};

		Vertex texv1,texv2,texv3,texv4;

		double delt=0.049;
		double rectsize=50;
		int t_id=0;
		for(int i=-20;i<20;i++)
		{
			for(int j=-20;j<20;j++)
			{
				texv1.triangle_count=0;
				texv1.tex_id=0;
				texv1.tex_coord.u=0.451+(i%8)*delt;
				texv1.tex_coord.v=0.451+(j%8)*delt;
				texv1.mat=m1;
				texv1.pos=Vector(rectsize*i,rectsize*j,-10);

				texv2.triangle_count=0;
				texv2.tex_id=0;
				texv2.tex_coord.u=0.451+delt+(i%8)*delt;
				texv2.tex_coord.v=0.451+(j%8)*delt;
				texv2.mat=m1;
				texv2.pos=Vector(rectsize*(i+1),rectsize*j,-10);

				texv3.triangle_count=0;
				texv3.tex_id=0;
				texv3.tex_coord.u=0.451+delt+(i%8)*delt;
				texv3.tex_coord.v=0.451+delt+(j%8)*delt;
				texv3.mat=m1;
				texv3.pos=Vector(rectsize*(i+1),rectsize*(j-1),-10);

				texv4.triangle_count=0;
				texv4.tex_id=0;
				texv4.tex_coord.u=0.451+(i%8)*delt;
				texv4.tex_coord.v=0.451+delt+(j%8)*delt;
				texv4.mat=m1;
				texv4.pos=Vector(rectsize*i,rectsize*(j-1),-10);

				ren->AddVertex(texv1);
				ren->AddVertex(texv2);
				ren->AddVertex(texv3);
				ren->AddVertex(texv4);

				ren->AddTriangle(4+4*t_id,6+4*t_id,5+4*t_id);
				ren->AddTriangle(4+4*t_id,7+4*t_id,6+4*t_id);
				t_id++;
			}
		}

		ren->finishAdd();


		ren->ReadSTL("data/zhy2.stl",m1,newColor);
		ren->ReadSTL("data/traffic_cone.stl",m1,newColor,Matrix4(SCALE,8));
		/*ren->AddVertex(Vector(0,0),m1,vRed,true);
		ren->AddVertex(Vector(200,0),m1,vGreen,true);
		ren->AddVertex(Vector(200,200),m1,vBlue,true);
		ren->AddVertex(Vector(100,100,-100),m1,vWhite,true);

		ren->AddTriangle(0,2,1);
		ren->AddTriangle(0,1,3);
		ren->AddTriangle(0,3,2);
		ren->AddTriangle(1,2,3);*/
		double Solar[3]={20,20,20};
		ren->setAmbient(3,3,3);
		ren->setLightSource(Vector(-1,-1,-1),Solar);
		ren->getCamera()->drawBack(500);
		
		ren->getCamera()->setPosition(Vector(30,30,30));
		//ren->getCamera()->pitch(1.5);
		ren->getCamera()->lookAt(Vector(0,2,-1));

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
			mouseX=LOWORD(lParam);
			mouseY=HIWORD(lParam);
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

void Controller::scrollCamera(int dist)
{
	ren->getCamera()->drawBack(dist);
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
		ren->getCamera()->setPosition(pos-50*direction);
	}
	if(SpaceIsDown)
	{
		ren->getCamera()->setPosition(pos+Vector(0,0,30));
	}

	//while(1)
	//{
	ren->Render();
	ren->clearBuffer();
	//}
}