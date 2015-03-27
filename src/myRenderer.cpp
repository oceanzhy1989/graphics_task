#include "myRenderer.h"
#include "graphicsAPI.h"
#include "stdio.h"
#include <iostream>
#include <fstream>
using namespace std;


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


	bufferMap=NULL;
	bufferMap_single=NULL;
	bufferMap_single_backup=NULL;


}

myRenderer::~myRenderer()
{
	delete m_camera;
	if(options & DEPTH_TEST)
	{
		if(bufferMap)
		{	
			delete[] bufferMap;
		}

		if(bufferMap_single)
		{
			delete[] bufferMap_single;
			delete[] bufferMap_single_backup;
		}
	}
}

void myRenderer::setHDC(HDC hDC)
{
	m_hdc=hDC;
}

void myRenderer::setAmbient(double red, double green, double blue)
{
	ambient[0]=red;
	ambient[1]=green;
	ambient[2]=blue;
}

void myRenderer::setLightSource(Vector direction, double I[3])
{
	lSource.direction=direction;
	memcpy(lSource.I,I,3*sizeof(double));
}

void myRenderer::SetOptions(int Option)
{
	this->options=Option;

	if(options & DEPTH_TEST)
	{
		if((options & ENABLE_A_BUFFER) && !bufferMap)
		{
			bufferMap=new list<IllumWithDepth>[width*height];
			bufferMap_single=NULL;
		}
		else
		{
			if(bufferMap_single)
				return;

			bufferMap=NULL;
			bufferMap_single=new IllumWithDepth[width*height];
			bufferMap_single_backup=new IllumWithDepth[width*height];

			for(int i=0;i<width;i++)
			{
				for(int j=0;j<height;j++)
				{
					bufferMap_single_backup[i+j*width].depth=1e100;
				}
			}

			memcpy(bufferMap_single,bufferMap_single_backup,width*height*sizeof(IllumWithDepth));
		}
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

		//for(int i=0;i<vert_count;i++)
		//{
		//	ProjectionVertice[i].mat=modelVertice[i].mat;
		//	memcpy(ProjectionVertice[i].color,modelVertice[i].color,3*sizeof(double));
		//}

	}

	for(int i=0;i<vert_count;i++)
	{
		modelViewVertice[i].pos=(*m_ModelViewMatrix)*modelVertice[i].pos;
		ProjectionVertice[i].pos=(*m_ProjectionMatrix)*modelViewVertice[i].pos;
		//ProjectionVertice[i].pos=0.9*modelVertice[i].pos;
		ProjectionVertice[i].pos.a[2]=modelViewVertice[i].pos.a[2];
		
	}

	//¼ÆËã¹âÕÕ
	//this->calIllumination();

	

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
		else
		{
			for(int i=0;i<width;i++)
			{
				for(int j=0;j<height;j++)
				{
					IllumWithDepth *b=bufferMap_single+i+j*width;					

					RGBQUAD RGB;
					RGB.rgbBlue=b->I[2]*255;
					RGB.rgbGreen=b->I[1]*255;
					RGB.rgbRed=b->I[0]*255;

					graphicsAPI::getInstance()->storePixel(i,j,RGB);

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


void myRenderer::AddVertex(const Vertex &v)
{
	modelVertice.push_back(v);
}

void myRenderer::AddVertex(const Vector4 &pos, const Material &mat, double color[], bool depth_test)
{
	Vertex v;
	v.pos=pos;
	v.mat=mat;
	v.triangle_count=0;

	//if(depth_test)
	//{
	//	memcpy(v.I,color,3*sizeof(double));
	//}
	//else
	//{
		memcpy(v.color,color,3*sizeof(double));
	//}

	modelVertice.push_back(v);
}

void myRenderer::AddTriangle(VertexID id1, VertexID id2, VertexID id3)
{
	Triangle t;
	t.vert[0]=id1;
	t.vert[1]=id2;
	t.vert[2]=id3;
	
	calNormalVector(&t);
	for(int i=0;i<3;i++)
	{
		modelVertice[t.vert[i]].n+=t.n;
		modelVertice[t.vert[i]].triangle_count++;
	}
	modelTriangles.push_back(t);
}

bool myRenderer::ReadSTL(const char *filename, Material mat, double color[3], Matrix4 transformMat)
{
	int jinzhi=0;
	FILE *fp;
	
	cout<<"filename="<<filename<<endl;
	
	char check;
	fp=fopen(filename,"rb");
	if(!fp)
		return false;
	while(!feof(fp))
	{
		fread(&check,1,sizeof(char),fp);
		if(check==char(0))
		{
			jinzhi=1;
			break;
		}
	}
	fclose(fp);


	ifstream from;
	if(jinzhi==0) 
		from.open(filename);
	else
		fp=fopen(filename,"rb");

	if(!from){
		cout<<"file error"<<endl;
		return false;
	}

	const double epscopoint=0.000001;
	long triNum;
	FACET triangle;

	int start_num=modelVertice.size();
	
	unsigned short aa;
//	from.seekg(82,ios::beg);
	if(jinzhi)
	{
		fseek(fp,80l,SEEK_SET);
		fread(&triNum,1,sizeof(long),fp);

		if(triNum<=0)
			return false;

		for(int i=0;i<triNum;i++)
		{
			fread(&triangle,1,sizeof(FACET),fp);
			fread(&aa,1,sizeof(unsigned short),fp);

			Triangle t;
			
			t.n.a[0]=triangle.n.x;
			t.n.a[1]=triangle.n.y;
			t.n.a[2]=triangle.n.z;


			for(int j=0;j<3;j++)
			{
				Vertex v;
				
				v.pos.a[0]=triangle.dot[j].x;
				v.pos.a[1]=triangle.dot[j].y;
				v.pos.a[2]=triangle.dot[j].z;
				v.pos=transformMat*v.pos;
				v.mat=mat;
				v.n=t.n;
				v.triangle_count=1;
				v.tex_id=-1;
				memcpy(v.color,color,3*sizeof(double));
				t.vert[j]=start_num++;
				modelVertice.push_back(v);
				

			}

			modelTriangles.push_back(t);


			if(!from) break;
			//vtri.push_back(tri);
		}



	}
	
	else
	{
		char rubbish[50];


		from>>rubbish>>rubbish;

		for(;;)
		{
			from>>rubbish>>rubbish;
			Triangle t;

			from>>t.n.a[0]>>t.n.a[1]>>t.n.a[2];

			from>>rubbish>>rubbish;

			if(!from)
				break;

			for(int j=0; j<3; j++)
			{
				Vertex v;

				from>>rubbish;
				from>>v.pos.a[0]>>v.pos.a[1]>>v.pos.a[2];
				v.pos=transformMat*v.pos;
				v.mat=mat;
				v.triangle_count=1;
				v.n=t.n;
				v.tex_id=-1;
				memcpy(v.color,color,3*sizeof(double));
				t.vert[j]=start_num++;
				modelVertice.push_back(v);

			}
			from>>rubbish>>rubbish;
			modelTriangles.push_back(t);

		}	

		from.close();
	}


	return true;
}

void myRenderer::finishAdd()
{
	int vert_count=modelVertice.size();

	for(int i=0;i<vert_count;i++)
	{
		if(modelVertice[i].triangle_count>0)
		{
			modelVertice[i].n*=1.0/modelVertice[i].triangle_count;
			modelVertice[i].n=unitof(modelVertice[i].n);
		}

	}
}

void myRenderer::clearBuffer()
{
	if(options  & DEPTH_TEST)
	{
		if(options & ENABLE_A_BUFFER)
		{
			for(int i=0;i<width;i++)
			{
				for(int j=0;j<height;j++)
				{
					list<IllumWithDepth> *p=bufferMap+i+j*width;
					
					p->clear();
				}
			}
		}
		else
		{
			memcpy(bufferMap_single,bufferMap_single_backup,width*height*sizeof(IllumWithDepth));
		}
	}

	graphicsAPI::getInstance()->flush();
}

void myRenderer::mergeBuffer(int x, int y, const IllumWithDepth &v)
{
	x+=halfwidth;
	y+=halfheight;

	if(x<0 || x>=width || y<0 || y>=height ) 
		return;

	if(options & ENABLE_A_BUFFER)
	{
		list<IllumWithDepth> *p=bufferMap+x+y*width;

		if(p->empty())
		{
			p->push_front(v);
		}
		else
		{

			{
				if(v.depth<(p->begin())->depth)
				{
					*(p->begin())=v;
				}
			}
			
		}
	}
	else
	{
		IllumWithDepth *p=bufferMap_single+x+y*width;

		if(v.depth<p->depth)
			*p=v;
		
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

void myRenderer::calNormalVector(Triangle *t)
{
	Vector *v0=&(modelVertice[t->vert[0]].pos);
	Vector *v1=&(modelVertice[t->vert[1]].pos);
	Vector *v2=&(modelVertice[t->vert[2]].pos);

	Vector l1(*v1-*v0);
	Vector l2(*v2-*v1);

	t->n=unitof(cross(l1,l2));
}

void myRenderer::calIllumination()
{
	int vert_count=modelVertice.size();

	if(options & DEPTH_TEST)
	{
		double Id[3]={0};
		double Ihigh[3]={0};

		for(int i=0;i<vert_count;i++)
		{
			if(modelVertice[i].triangle_count<1) continue;

			
			double coe=-dot(lSource.direction,modelVertice[i].n);
			for(int k=0;k<3;k++)
			{
				Id[k]=coe*lSource.I[k];
				if(Id[k]<0)
					Id[k]=0;

				Material *mat=&modelVertice[i].mat;
				double newI=(mat->Kd)*Id[k]+(mat->Ka)*ambient[k]+(mat->Ks)*Ihigh[k];
				//ProjectionVertice[i].I[k]=newI*modelVertice[i].color[k];
				if((options & TEXTURE_MAPPING) && modelVertice[i].tex_id>=0)
					modelVertice[i].I[k]=newI;
				else
					modelVertice[i].I[k]=newI*modelVertice[i].color[k];
			}
		}
	}
	else
	{
		for(int i=0;i<vert_count;i++)
		{
			memcpy(ProjectionVertice[i].I,modelVertice[i].color,3*sizeof(double));
		}
	}
}

bool myRenderer::loadNewTexture(const char *tex_file_name)
{
	MYBITMAP newReader;

	if(newReader.readBmp(tex_file_name))
	{
		textures.push_back(newReader);
		return true;
	}

	return false;
}

void myRenderer::subRasterization(Vertex **v,int x, int y, int ModelVertexIndex[3])
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


	Vector normal[3];
	for(int i=0;i<3;i++)
	{
		normal[i]=cross(p[(1+i)%3],p[(2+i)%3]);
	}
	for(int i=0;i<3;i++)
	{
		if(dot(normal[(1+i)%3],normal[(2+i)%3])<0)
			return;
		else
			c[i]=norm(normal[i]);
	}

	double area=1/(c[0]+c[1]+c[2]);
	for(int i=0;i<3;i++)
	{
		c[i]*=area;
	}

	if(options & DEPTH_TEST)
	{
		IllumWithDepth tmp=c[0]*IllumWithDepth(modelVertice[ModelVertexIndex[0]],v[0]->pos.a[2])+
						   c[1]*IllumWithDepth(modelVertice[ModelVertexIndex[1]],v[1]->pos.a[2])+
						   c[2]*IllumWithDepth(modelVertice[ModelVertexIndex[2]],v[2]->pos.a[2]);

		if((options & TEXTURE_MAPPING) && tmp.tex_id>=0 && tmp.tex_id<textures.size() )
		{
			for(int k=0;k<3;k++)
			{
				tmp.I[k]*=textures[tmp.tex_id].readPixel(tmp.tex_u,tmp.tex_v).color[k];
			}
		}
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

	int Index[3];

	int label=0;
	if(v0.pos.a[0] > v1.pos.a[0])
		label++;
	if(v0.pos.a[0] > v2.pos.a[0])
		label++;
	v[label]=&v0;
	Index[0]=t.vert[label];

	int label1=0;
	if(v1.pos.a[0] > v0.pos.a[0])
		label1++;
	if(v1.pos.a[0] > v2.pos.a[0])
		label1++;
	v[label1]=&v1;
	Index[1]=t.vert[label1];

	v[3-label1-label]=&v2;
	Index[2]=t.vert[3-label1-label];


	double y1=v[0]->pos.a[1];
	double y2=y1;

	int sign;

	RGBQUAD RGB={0};
	


	int xmin=max(-halfwidth,round(v[0]->pos.a[0])-1);
	double xmid=min(halfwidth,v[1]->pos.a[0]);
	int xmax=min(halfwidth,round(v[2]->pos.a[0])+1);
	for(int x=xmin;x<xmid;x++)
	{
		if(cutLine(v[0]->pos,v[1]->pos,x,y1) && cutLine(v[0]->pos,v[2]->pos,x,y2))
		{
			if(y1<-halfheight) 
				y1=-halfheight;
			if(y1>halfheight) 
				y1=halfheight;
			if(y2<-halfheight) 
				y2=-halfheight;
			if(y2>halfheight) 
				y2=halfheight;
			sign=(y2-y1)/fabs(y2-y1);
			for(int y=round(y1)-2*sign;y!=round(y2)+2*sign;y+=sign)
			{
				subRasterization(v,x,y,Index);
			}
		}

	}

	for(int x=xmid;x<xmax;x++)
	{
		if(cutLine(v[1]->pos,v[2]->pos,x,y1) && cutLine(v[0]->pos,v[2]->pos,x,y2))
		{
			if(y1<-halfheight) 
				y1=-halfheight;
			if(y1>halfheight) 
				y1=halfheight;
			if(y2<-halfheight) 
				y2=-halfheight;
			if(y2>halfheight) 
				y2=halfheight;
			sign=(y2-y1)/fabs(y2-y1);
			for(int y=round(y1)-2*sign;y!=round(y2)+2*sign;y+=sign)
			{
				subRasterization(v,x,y,Index);
			}
		}

	}



}