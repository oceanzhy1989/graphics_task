#include <iostream>
#include "camera.h"
#include "graphicsAPI.h"
using namespace std;

void main()
{
	graphicsAPI *api=graphicsAPI::getInstance();

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

	api->init(1000,1000);

	RGBQUAD color={0};
	color.rgbGreen=255;
	for(int i=0;i<500;i++)
		api->storePixel(i,i,color);

	HWND hwnd = GetDesktopWindow();
	//获取桌面窗口DC
	HDC hdc = GetWindowDC(hwnd);

	api->draw(hdc,1000,100);

	system("pause");
}