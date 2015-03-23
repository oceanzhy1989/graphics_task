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
	myRenderer ren(1000,800,hdc,900,20);

	Material m1,m2,m3;

	ren.AddVertex(Vector(0,0),m1,Red);
	ren.AddVertex(Vector(200,0),m1,Green);
	ren.AddVertex(Vector(200,200),m1,Blue);

	ren.AddTriangle(0,1,2);

	ren.getCamera()->drawBack(1500);
	ren.getCamera()->rotateByTarget(1);
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