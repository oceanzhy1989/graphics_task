#include "utils.h"

void storeLine(const Vertex *v1, const Vertex *v2)
{
	double dy=v2->pos.a[1]-v1->pos.a[1];
	double dx=v2->pos.a[0]-v1->pos.a[0];
	double m=dy/dx;
	double t=-0.0001,dt;
	int x1,y1,x2,y2;

	const Vertex *sz_v1;
	const Vertex *sz_v2;

	if(fabs(m)<1)
	{
		if(dx<0)
		{
			x1=round(v2->pos.a[0]);
			x2=round(v1->pos.a[0]);
			y1=round(v2->pos.a[1]);
			y2=round(v1->pos.a[1]);

			sz_v1=v2;
			sz_v2=v1;
		}
		else
		{
			x1=round(v1->pos.a[0]);
			x2=round(v2->pos.a[0]);
			y1=round(v1->pos.a[1]);
			y2=round(v2->pos.a[1]);

			sz_v1=v1;
			sz_v2=v2;
		}

		double y=y1;
		
		dt=1/fabs(dx);
		for(int i=x1;i<x2;i++)
		{
			t+=dt;
			y+=m;

			double color[3];
			for(int k=0;k<3;k++)
			{
				color[k]=(1.0001-t)*(sz_v1->color[k])+t*(sz_v2->color[k]);
			}

			RGBQUAD RGB4;
			RGB4.rgbRed=255*color[0];
			RGB4.rgbGreen=255*color[1];
			RGB4.rgbBlue=255*color[2];
			RGB4.rgbReserved=0;
			graphicsAPI::getInstance()->storePixel(i,(int)y,RGB4);

		}

	}
	else
	{
		m=1/m;

		if(dy<0)
		{
			x1=round(v2->pos.a[0]);
			x2=round(v1->pos.a[0]);
			y1=round(v2->pos.a[1]);
			y2=round(v1->pos.a[1]);

			sz_v1=v2;
			sz_v2=v1;
		}
		else
		{
			x1=round(v1->pos.a[0]);
			x2=round(v2->pos.a[0]);
			y1=round(v1->pos.a[1]);
			y2=round(v2->pos.a[1]);

			sz_v1=v1;
			sz_v2=v2;
		}

		double x=x1;
		
		dt=1/fabs(dy);
		for(int i=y1;i<y2;i++)
		{
			t+=dt;
			x+=m;

			double color[3];
			for(int k=0;k<3;k++)
			{
				color[k]=(1.0001-t)*(sz_v1->color[k])+t*(sz_v2->color[k]);
			}

			RGBQUAD RGB4;
			RGB4.rgbRed=255*color[0];
			RGB4.rgbGreen=255*color[1];
			RGB4.rgbBlue=255*color[2];
			RGB4.rgbReserved=0;
			graphicsAPI::getInstance()->storePixel((int)x,i,RGB4);

		}

	}

	//graphicsAPI::getInstance()->draw(hDC,left,top);
}

Vertex operator + (const Vertex &v1, const Vertex &v2)
{
	Vertex tmp;

	tmp.pos=v1.pos+v2.pos;
	tmp.n=v1.n+v2.n;

	tmp.mat.T=v1.mat.T+v2.mat.T;
	tmp.mat.Ka=v1.mat.Ka+v2.mat.Ka;
	tmp.mat.Kd=v1.mat.Kd+v2.mat.Kd;
	tmp.mat.Ks=v1.mat.Ks+v2.mat.Ks;
	tmp.mat.n=v1.mat.n+v2.mat.n;
	tmp.mat.roughn=v1.mat.roughn+v2.mat.roughn;

	for(int i=0;i<3;i++)
	{
		tmp.mat.alpha[i]=v1.mat.alpha[i]+v2.mat.alpha[i];
		tmp.color[i]=v1.color[i]+v2.color[i];
	}

	return tmp;
}
Vertex operator * (double c, const Vertex &v2)
{
	Vertex tmp;

	tmp.pos=c*v2.pos;
	tmp.n=c*v2.n;

	tmp.mat.T=c*v2.mat.T;
	tmp.mat.Ka=c*v2.mat.Ka;
	tmp.mat.Kd=c*v2.mat.Kd;
	tmp.mat.Ks=c*v2.mat.Ks;
	tmp.mat.n=c*v2.mat.n;
	tmp.mat.roughn=c*v2.mat.roughn;

	for(int i=0;i<3;i++)
	{
		tmp.mat.alpha[i]=c*v2.mat.alpha[i];
		tmp.color[i]=c*v2.color[i];
	}

	return tmp;
}
