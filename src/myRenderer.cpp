#include "myRenderer.h"
#include "graphicsAPI.h"


myRenderer::myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset, int yoffset) : options(DRAW_BOARDER),m_hdc(hDC),xoff(xoffset),yoff(yoffset)
{
	m_camera=new Camera(0,winWidth,winHeight,max(winWidth,winHeight));
	m_ModelViewMatrix=m_camera->getModelViewMatrix();
	m_ProjectionMatrix=m_camera->getProjectionMatrix();

	graphicsAPI::getInstance()->init(winWidth,winHeight);

	modelVertice.reserve(100);
	modelTriangles.reserve(100);

}

myRenderer::~myRenderer()
{
	delete m_camera;
}

void myRenderer::SetOptions(int Option)
{
	this->options=Option;
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

	graphicsAPI::getInstance()->draw(m_hdc,xoff,yoff);
	return 0;
}

Camera *myRenderer::getCamera()
{
	return m_camera;
}




void myRenderer::AddVertex(const Vector4 &pos, const Material &mat, double color[])
{
	Vertex v;
	v.pos=pos;
	v.mat=mat;
	memcpy(v.color,color,3*sizeof(double));

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


void myRenderer::Rasterization(const Triangle &t)
{
	Vertex *v[3];
	Vertex v0=ProjectionVertice[t.vert[0]];
	Vertex v1=ProjectionVertice[t.vert[1]];
	Vertex v2=ProjectionVertice[t.vert[2]];

	v0.pos.a[2]=v1.pos.a[2]=v2.pos.a[2]=0;

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
			Vector origin(x,y,0);
			Vector p[3];
			double c[3];

			for(int i=0;i<3;i++)
				p[i]=v[i]->pos-origin;

			for(int i=0;i<3;i++)
			{
				c[i]=norm(cross(p[(1+i)%3],p[(2+i)%3]));
			}

			double area=1/(c[0]+c[1]+c[2]);
			for(int i=0;i<3;i++)
			{
				c[i]*=area;
			}
			double color[3]={0};
			for(int i=0;i<3;i++)
			{
				color[i]+=v[0]->color[i]*c[0]+v[1]->color[i]*c[1]+v[2]->color[i]*c[2];
			}
			
			RGB.rgbRed=255*color[0];
			RGB.rgbGreen=255*color[1];
			RGB.rgbBlue=255*color[2];

			graphicsAPI::getInstance()->storePixel(x,y,RGB);
		}

		y1+=m1;
		y2+=m2;
	}

	for(int x=round(v[1]->pos.a[0]);x<round(v[2]->pos.a[0]);x++)
	{
		for(int y=round(y1);y!=round(y2);y+=sign)
		{
			Vector origin(x,y,0);
			Vector p[3];
			double c[3];

			for(int i=0;i<3;i++)
				p[i]=v[i]->pos-origin;

			for(int i=0;i<3;i++)
			{
				c[i]=norm(cross(p[(1+i)%3],p[(2+i)%3]));
			}

			double area=1/(c[0]+c[1]+c[2]);
			for(int i=0;i<3;i++)
			{
				c[i]*=area;
			}
			double color[3]={0};
			for(int i=0;i<3;i++)
			{
				color[i]+=v[0]->color[i]*c[0]+v[1]->color[i]*c[1]+v[2]->color[i]*c[2];
			}
			
			RGB.rgbRed=255*color[0];
			RGB.rgbGreen=255*color[1];
			RGB.rgbBlue=255*color[2];

			graphicsAPI::getInstance()->storePixel(x,y,RGB);
		}

		y1+=m3;
		y2+=m2;
	}



}