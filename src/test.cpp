#include <iostream>
#include "utils.h"

using namespace std;

void main()
{
	Vector4 nz(0,0,1);
	Vector4 nx(1,0,0);
	Vector4 ss(10,10,10);

	Vector4 xx=rotate(nx,nz,pi/2);
	Vector4 x1=conj(ss);
	x1=inv(ss);

	xx=ss*x1;
	//Vector4 xx=cross(nz,nx);

	Vector4 axis(1,1,1);
	Matrix4 m(Mat::IDENTITY);

	m.rotateAxis(axis,2*pi/3);

	cout<<"xx="<<xx.a[0]<<","<<xx.a[1]<<","<<xx.a[2];

	system("pause");
}