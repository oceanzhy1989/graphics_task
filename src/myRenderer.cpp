#include "myRenderer.h"

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

	int size=modelVertice.size();
	if(modelViewVertice.size()<size)
	{
		modelViewVertice.resize(size);
		ProjectionVertice.resize(size);

		for(int i=0;i<size;i++)
		{
			ProjectionVertice[i].mat=modelVertice[i].mat;
			memcpy(ProjectionVertice[i].color,modelVertice[i].color,3*sizeof(double));
		}

	}

	for(int i=0;i<size;i++)
	{
		modelViewVertice[i].pos=(*m_ModelViewMatrix)*modelVertice[i].pos;
		ProjectionVertice[i].pos=(*m_ProjectionMatrix)*modelViewVertice[i].pos;
		ProjectionVertice[i].pos.a[2]=modelViewVertice[i].pos.a[2];
		
	}

	if(options & DRAW_BOARDER)
	{
		int size=modelTriangles.size();

		for(int i=0;i<size;i++)
		{
			Triangle *triangle=&(modelTriangles[i]);			
			
			storeLine(&ProjectionVertice[triangle->vert[0]], &ProjectionVertice[triangle->vert[1]]);
			storeLine(&ProjectionVertice[triangle->vert[1]], &ProjectionVertice[triangle->vert[2]]);
			storeLine(&ProjectionVertice[triangle->vert[2]], &ProjectionVertice[triangle->vert[0]]);
			
		}
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