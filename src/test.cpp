#include <iostream>
#include "camera.h"
#include "graphicsAPI.h"
#include "utils.h"
#include "myRenderer.h"
using namespace std;

double Red[3]={1,0,0};
double Green[3]={0,1,0};
double Blue[3]={0,0,1};

void main()
{
	//graphicsAPI *api=graphicsAPI::getInstance();

	//Vector4 nz(0,0,1);
	//Vector4 nx(1,0,0);
	//Vector4 ss(10,10,10);

	//Vector4 xx=rotate(nx,nz,pi/2);
	//Vector4 x1=conj(ss);
	//x1=inv(ss);

	//xx=ss*x1;
	////Vector4 xx=cross(nz,nx);

	//Vector4 axis(1,1,1);
	//Matrix4 m(IDENTITY);

	//m.rotateAxis(axis,2*pi/3);

	//cout<<"xx="<<xx.a[0]<<","<<xx.a[1]<<","<<xx.a[2];

	//api->init(1000,800);

	//RGBQUAD color={0};
	//color.rgbGreen=255;
	//for(int i=0;i<500;i++)
	//	api->storePixel(i,i,color);

	HWND hwnd = GetDesktopWindow();
	//获取桌面窗口DC
	HDC hdc = GetWindowDC(hwnd);

	//api->draw(hdc,1000,100);
	myRenderer ren(600,500,hdc,500,20);
	//ren.SetOptions(DRAW_BOARDER | FILL);
	//ren.SetOptions(DRAW_BOARDER);
	ren.SetOptions(FILL | DEPTH_TEST);
	Material m1,m2,m3;
	m1=newMaterial(0.02,0.151,0.2,0,1,0.8);

	double White[3]={1,1,1};
	ren.AddVertex(Vector(0,0),m1,Red,true);
	ren.AddVertex(Vector(200,0),m1,Green,true);
	ren.AddVertex(Vector(200,200),m1,Blue,true);
	ren.AddVertex(Vector(100,100,-100),m1,White,true);

	ren.AddTriangle(0,1,2);
	ren.AddTriangle(0,1,3);
	ren.AddTriangle(0,3,2);
	ren.AddTriangle(3,1,2);

	ren.finishAdd();

	double Solar[3]={10,10,1};
	//double DayLight[3]={3,3,3};
	ren.setAmbient(3,3,3);
	ren.setLightSource(Vector(-1,-1,-1),Solar);

	ren.getCamera()->drawBack(100);
	ren.getCamera()->rotateByTarget(0.5);
	ren.getCamera()->pitch(-1.5);
	ren.Render();

	
	

	//Vertex v1,v2;

	//v1.pos.a[0]=10;
	//v1.pos.a[1]=10;
	//v2.pos.a[0]=100;
	//v2.pos.a[1]=200;

	//memcpy(v1.color,Green,3*sizeof(double));
	//memcpy(v2.color,Green,3*sizeof(double));


	//storeLine(&v1,&v2);
	//api->draw(hdc,1000,100);

	system("pause");
}