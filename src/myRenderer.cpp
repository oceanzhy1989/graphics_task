#include "myRenderer.h"
#include "graphicsAPI.h"


myRenderer::myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset, int yoffset) : options(DRAW_BOARDER),m_hdc(hDC),xoff(xoffset),yoff(yoffset)
{
	width=winWidth;
	height=winHeight;
	halfwidth=width/2;
	halfheight=height/2;
	m_camera=new Camera(0,winWidth,winHeight,max(winWidth,winHeight));
	m_ModelViewMatrix=m_camera->getModelViewMatrix();
	m_ProjectionMatrix=m_camera->getProjectionMatrix();

	graphicsAPI::getInstance()->init(winWidth,winHeight);

	modelVertice.reserve(100);
	modelTriangles.reserve(100);

	if(options & DEPTH_TEST)
	{
		bufferMap=new list<IllumWithDepth>[width*height];
	}
	else
	{
		bufferMap=NULL;
	}

}

myRenderer::~myRenderer()
{
	delete m_camera;
	if(options & DEPTH_TEST)
	{
		delete[] bufferMap;
	}
}

void myRenderer::SetOptions(int Option)
{
	this->options=Option;

	if((options & DEPTH_TEST) && !bufferMap)
	{
		bufferMap=new list<IllumWithDepth>[width*height];
	}
}

int myRenderer::Render()
{
	if(options==0)
		return 1;

	int vert_count=modelVertice.size();
	if(modelViewVertice.size()<vert_count)
	{
		modelViewVertice.resize(vert_count);
		ProjectionVertice.resize(vert_count);

		for(int i=0;i<vert_count;i++)
		{
			ProjectionVertice[i].mat=modelVertice[i].mat;
			memcpy(ProjectionVertice[i].color,modelVertice[i].color,3*sizeof(double));
		}

	}

	for(int i=0;i<vert_count;i++)
	{
		modelViewVertice[i].pos=(*m_ModelViewMatrix)*modelVertice[i].pos;
		ProjectionVertice[i].pos=(*m_ProjectionMatrix)*modelViewVertice[i].pos;
		ProjectionVertice[i].pos.a[2]=modelViewVertice[i].pos.a[2];
		
	}

	//¼ÆËã¹âÕÕ
	for(int i=0;i<vert_count;i++)
	{
		memcpy(ProjectionVertice[i].I,modelVertice[i].I,3*sizeof(double));
	}

	int tri_count=modelTriangles.size();
	if(options & DRAW_BOARDER)
	{
		for(int i=0;i<tri_count;i++)
		{
			Triangle *triangle=&(modelTriangles[i]);			
			
			storeLine(&ProjectionVertice[triangle->vert[0]], &ProjectionVertice[triangle->vert[1]]);
			storeLine(&ProjectionVertice[triangle->vert[1]], &ProjectionVertice[triangle->vert[2]]);
			storeLine(&ProjectionVertice[triangle->vert[2]], &ProjectionVertice[triangle->vert[0]]);
			
		}
	}

	if(options & FILL)
	{
		for(int i=0;i<tri_count;i++)
			this->Rasterization(modelTriangles[i]);
	}

	if(options & DEPTH_TEST)
	{
		if(options & ENABLE_A_BUFFER)
		{
		}
		else
		{
			for(int i=0;i<width;i++)
			{
				for(int j=0;j<height;j++)
				{
					list<IllumWithDepth> *p=bufferMap+i+j*width;
					IllumWithDepth *b;
					if(!p->empty())
					{
						b=&*(p->begin());

						RGBQUAD RGB;
						RGB.rgbBlue=b->I[2]*255;
						RGB.rgbGreen=b->I[1]*255;
						RGB.rgbRed=b->I[0]*255;

						graphicsAPI::getInstance()->storePixel(i,j,RGB);
					}
				}
			}
		}
	}

	graphicsAPI::getInstance()->draw(m_hdc,xoff,yoff);
	return 0;
}

Camera *myRenderer::getCamera()
{
	return m_camera;
}




void myRenderer::AddVertex(const Vector4 &pos, const Material &mat, double color[], bool depth_test)
{
	Vertex v;
	v.pos=pos;
	v.mat=mat;

	if(depth_test)
	{
		memcpy(v.I,color,3*sizeof(double));
	}
	else
	{
		memcpy(v.color,color,3*sizeof(double));
	}

	modelVertice.push_back(v);
}

void myRenderer::AddTriangle(VertexID id1, VertexID id2, VertexID id3)
{
	Triangle t;
	t.vert[0]=id1;
	t.vert[1]=id2;
	t.vert[2]=id3;
	modelTriangles.push_back(t);
}

void myRenderer::mergeBuffer(int x, int y, const IllumWithDepth &v)
{
	x+=halfwidth;
	y+=halfheight;

	if(x<0 || x>=width || y<0 || y>=height ) 
		return;

	list<IllumWithDepth> *p=bufferMap+x+y*width;

	if(p->empty())
	{
		p->push_front(v);
	}
	else
	{
		if(options & ENABLE_A_BUFFER)
		{
		}
		else
		{
			if(v.depth<(p->begin())->depth)
			{
				*(p->begin())=v;
			}
		}
		
	}
}

void myRenderer::storeLine(const Vertex *v1, const Vertex *v2)
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
			graphicsAPI::getInstance()->storePixel(i+halfwidth,(int)y+halfheight,RGB4);

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
			graphicsAPI::getInstance()->storePixel((int)x+halfwidth,i+halfheight,RGB4);

		}

	}

	//graphicsAPI::getInstance()->draw(hDC,left,top);
}

void myRenderer::subRasterization(Vertex **v,int x, int y)
{
	RGBQUAD RGB;
	Vector origin(x,y,0);
	Vector p[3];
	double c[3];

	for(int i=0;i<3;i++)
	{
		p[i]=v[i]->pos;
		p[i].a[2]=0;
		p[i]=p[i]-origin;
	}

	for(int i=0;i<3;i++)
	{
		c[i]=norm(cross(p[(1+i)%3],p[(2+i)%3]));
	}

	double area=1/(c[0]+c[1]+c[2]);
	for(int i=0;i<3;i++)
	{
		c[i]*=area;
	}

	if(options & DEPTH_TEST)
	{
		IllumWithDepth tmp=c[0]*IllumWithDepth(*v[0])+c[1]*IllumWithDepth(*v[1])+c[2]*IllumWithDepth(*v[2]);
		mergeBuffer(x,y,tmp);
	}
	else
	{
		double color[3]={0};
		for(int i=0;i<3;i++)
		{
			color[i]+=v[0]->color[i]*c[0]+v[1]->color[i]*c[1]+v[2]->color[i]*c[2];
		}
		
		RGB.rgbRed=255*color[0];
		RGB.rgbGreen=255*color[1];
		RGB.rgbBlue=255*color[2];

		graphicsAPI::getInstance()->storePixel(x+halfwidth,y+halfheight,RGB);
	}
}

void myRenderer::Rasterization(const Triangle &t)
{
	Vertex *v[3];
	Vertex v0=ProjectionVertice[t.vert[0]];
	Vertex v1=ProjectionVertice[t.vert[1]];
	Vertex v2=ProjectionVertice[t.vert[2]];

	//v0.pos.a[2]=v1.pos.a[2]=v2.pos.a[2]=0;

	int label=0;
	if(v0.pos.a[0] > v1.pos.a[0])
		label++;
	if(v0.pos.a[0] > v2.pos.a[0])
		label++;
	v[label]=&v0;

	int label1=0;
	if(v1.pos.a[0] > v0.pos.a[0])
		label1++;
	if(v1.pos.a[0] > v2.pos.a[0])
		label1++;
	v[label1]=&v1;

	v[3-label1-label]=&v2;

	double m1=(v[1]->pos.a[1]-v[0]->pos.a[1])/(v[1]->pos.a[0]-v[0]->pos.a[0]);
	double m2=(v[2]->pos.a[1]-v[0]->pos.a[1])/(v[2]->pos.a[0]-v[0]->pos.a[0]);
	double m3=(v[2]->pos.a[1]-v[1]->pos.a[1])/(v[2]->pos.a[0]-v[1]->pos.a[0]);

	double y1=v[0]->pos.a[1];
	double y2=y1;
	int sign=(m2-m1)/fabs(m2-m1);

	RGBQUAD RGB={0};
	
	//Red.rgbRed=255;

	for(int x=round(v[0]->pos.a[0]);x<round(v[1]->pos.a[0]);x++)
	{
		for(int y=round(y1);y!=round(y2);y+=sign)
		{
			subRasterization(v,x,y);
		}

		y1+=m1;
		y2+=m2;

		sign=(y2-y1)/fabs(y2-y1);
	}

	for(int x=round(v[1]->pos.a[0]);x<round(v[2]->pos.a[0]);x++)
	{
		for(int y=round(y1);y!=round(y2);y+=sign)
		{
			subRasterization(v,x,y);
		}

		y1+=m3;
		y2+=m2;
		sign=(y2-y1)/fabs(y2-y1);
	}



}