#include "myRenderer.h"
#include "graphicsAPI.h"
#include "stdio.h"
#include <iostream>
#include <fstream>
using namespace std;

const int bit[6]={1,1<<1,1<<2,1<<3,1<<4,1<<5};

myRenderer::myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset, int yoffset) : options(DRAW_BOARDER),m_hdc(hDC),xoff(xoffset),yoff(yoffset)
{
	width=winWidth;
	height=winHeight;
	halfwidth=width/2;
	halfheight=height/2;
	

	m_camera=new Camera(0,winWidth,winHeight,max(winWidth,winHeight));
	double z0=m_camera->getZ0();
	cut_bound[0]=2-halfwidth;cut_bound[3]=halfwidth-2;
	cut_bound[1]=3-halfheight;cut_bound[4]=halfheight-3;
	cut_bound[2]=z0+(halfwidth>>1);cut_bound[5]=z0+(width<<2);

	m_ModelViewMatrix=m_camera->getModelViewMatrix();
	m_ProjectionMatrix=m_camera->getProjectionMatrix();

	graphicsAPI::getInstance()->init(winWidth,winHeight);

	modelVertice.reserve(100);
	modelTriangles.reserve(100);


	bufferMap=NULL;
	bufferMap_single=NULL;
	bufferMap_single_backup=NULL;

	Vertex tmp;
	for(int i=0;i<24;i++)
		modelVertice.push_back(tmp);


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
	lSource.direction=unitof(direction);
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

int myRenderer::subcut(int bound_id, int rcode[])
{
	int newtmptrinum=tmpTriNum;
	for(int tri_id=0;tri_id<newtmptrinum;tri_id++)
	{
		Triangle &t=tmpTriangles[tri_id]; 
		if(t.vert[0]<0)
			continue;

		Vertex *v[3]={&ProjectionVertice[t.vert[0]],&ProjectionVertice[t.vert[1]],&ProjectionVertice[t.vert[2]]};
		for(int i=0;i<3;i++)
		{
			if(t.vert[i]<20)
				continue;

			memcpy(v[i]->I,modelVertice[t.vert[i]].I,3*sizeof(double));
			v[i]->tex_coord=modelVertice[t.vert[i]].tex_coord;
			v[i]->tex_id=modelVertice[t.vert[i]].tex_id;
			v[i]->triangle_count=modelVertice[t.vert[i]].triangle_count;
		}

		double z0=getCamera()->getZ0();
		int sign=bound_id>2?-1:1;
		int subrcode[3];//={rcode[0]&bit[bound_id], rcode[1]&bit[bound_id], rcode[2]&bit[bound_id] };
		for(int i=0;i<3;i++)
		{
			subrcode[i]=sgn(sign*(cut_bound[bound_id]-ProjectionVertice[t.vert[i]].pos.a[bound_id%3]));
		}

		int test1=subrcode[0] | subrcode[1] | subrcode[2];//全在内
		int test2=subrcode[0] & subrcode[1] & subrcode[2];//全在外
		if(test2 != 0)
		{
			t.vert[0]=-1;
			continue;
		}
		if(test1 ==0)
			continue;

		
		Vertex *vresult[4];		
		int resnum=0;

		for(int i=0;i<3;i++)
		{
			Vertex *vstart=v[i];
			Vertex *vend=v[(i+1)%3];

			double t_near=(cut_bound[bound_id]-vstart->pos.a[bound_id%3])/(vend->pos.a[bound_id%3]-vstart->pos.a[bound_id%3]);

			if(subrcode[i]==0)
				vresult[resnum++]=v[i];

			if(t_near>0 && t_near<1)
			{
				IllumWithDepth ilstart(*vstart,vstart->pos.a[2],1/(vstart->pos.a[2]-z0));
				IllumWithDepth ilend(*vend,vend->pos.a[2],1/(vend->pos.a[2]-z0));
				vresult[resnum]=&ProjectionVertice[tmpVertNum];
				//*(vresult[resnum])=(1-t_near)/(vstart->pos.a[2]-z0)*(*vstart)+t_near/(vend->pos.a[2]-z0)*(*vend);
				//*(vresult[resnum])*=z0/(vresult[resnum]->pos.a[2]-1);
				*(vresult[resnum])=(1-t_near)*(*vstart)+t_near*(*vend);
				IllumWithDepth ilres=(1-t_near)*ilstart+t_near*ilend;
				ilres*=z0/(ilres.depth-1);
				vresult[resnum]->pos.a[2]=ilres.depth;
				vresult[resnum]->tex_coord.u=ilres.tex_u;
				vresult[resnum]->tex_coord.v=ilres.tex_v;
				//vresult[resnum]->


				resnum++;
				tmpVertNum++;
			}
		}

		Vertex *start_idv=&ProjectionVertice[0];
		if(resnum>0)
		{
			t.n=t.n;
			t.vert[0]=vresult[0]-start_idv;
			t.vert[1]=vresult[1]-start_idv;
			t.vert[2]=vresult[2]-start_idv;

		}
		if(resnum>3)
		{
			tmpTriNum++;

			tmpTriangles[tmpTriNum-1].n=t.n;
			tmpTriangles[tmpTriNum-1].vert[0]=vresult[0]-start_idv;
			tmpTriangles[tmpTriNum-1].vert[1]=vresult[2]-start_idv;
			tmpTriangles[tmpTriNum-1].vert[2]=vresult[3]-start_idv;
		}
		

	}

	return 0;
}

int myRenderer::cut(const Triangle &t)
{
	Vertex *v[3]={&ProjectionVertice[t.vert[0]],&ProjectionVertice[t.vert[1]],&ProjectionVertice[t.vert[2]]};
	int rcode[3]={sgn(cut_bound[0]-v[0]->pos.a[0])*bit[0] | sgn(cut_bound[1]-v[0]->pos.a[1])*bit[1] | sgn(cut_bound[2]-v[0]->pos.a[2])*bit[2] | sgn(-(cut_bound[3]-v[0]->pos.a[0]))*bit[3] | sgn(-(cut_bound[4]-v[0]->pos.a[1]))*bit[4] | sgn(-(cut_bound[5]-v[0]->pos.a[2]))*bit[5],
				  sgn(cut_bound[0]-v[1]->pos.a[0])*bit[0] | sgn(cut_bound[1]-v[1]->pos.a[1])*bit[1] | sgn(cut_bound[2]-v[1]->pos.a[2])*bit[2] | sgn(-(cut_bound[3]-v[1]->pos.a[0]))*bit[3] | sgn(-(cut_bound[4]-v[1]->pos.a[1]))*bit[4] | sgn(-(cut_bound[5]-v[1]->pos.a[2]))*bit[5],
				  sgn(cut_bound[0]-v[2]->pos.a[0])*bit[0] | sgn(cut_bound[1]-v[2]->pos.a[1])*bit[1] | sgn(cut_bound[2]-v[2]->pos.a[2])*bit[2] | sgn(-(cut_bound[3]-v[2]->pos.a[0]))*bit[3] | sgn(-(cut_bound[4]-v[2]->pos.a[1]))*bit[4] | sgn(-(cut_bound[5]-v[2]->pos.a[2]))*bit[5]
	};

	if((rcode[0] & rcode[1] & rcode[2]) != 0)
	{	
		return 0;
	}
	if((rcode[0] | rcode[1] | rcode[2]) == 0)
	{	
		return 1;
	}

	//return 0;
	tmpVertNum=0;
	tmpTriNum=1;
	tmpTriangles[0]=t;
	for(int i=0;i<6;i++)
		subcut(i,rcode);

	if(tmpVertNum>0)
		memcpy(&modelVertice[0],&ProjectionVertice[0],tmpVertNum*sizeof(Vertex));
	return 10+tmpTriNum;


}
int myRenderer::cut(const Triangle &t, double z_front, double z_back)
{
	Vertex *v[3]={&ProjectionVertice[t.vert[0]],&ProjectionVertice[t.vert[1]],&ProjectionVertice[t.vert[2]]};

	double z0=getCamera()->getZ0();

	unsigned char rcode[3]={
		(sgn(-halfwidth-v[0]->pos.a[0])*bit[0]) | (sgn(v[0]->pos.a[0]-halfwidth)*bit[1]) | (sgn(-halfheight-v[0]->pos.a[1])*bit[2]) | (sgn(v[0]->pos.a[1]-halfheight)*bit[3]) | (sgn(z_front+z0-v[0]->pos.a[2])*bit[4]) | (sgn(v[0]->pos.a[2]-z_back-z0)*bit[5]),
		(sgn(-halfwidth-v[1]->pos.a[0])*bit[0]) | (sgn(v[1]->pos.a[0]-halfwidth)*bit[1]) | (sgn(-halfheight-v[1]->pos.a[1])*bit[2]) | (sgn(v[1]->pos.a[1]-halfheight)*bit[3]) | (sgn(z_front+z0-v[1]->pos.a[2])*bit[4]) | (sgn(v[1]->pos.a[2]-z_back-z0)*bit[5]),
		(sgn(-halfwidth-v[2]->pos.a[0])*bit[0]) | (sgn(v[2]->pos.a[0]-halfwidth)*bit[1]) | (sgn(-halfheight-v[2]->pos.a[1])*bit[2]) | (sgn(v[2]->pos.a[1]-halfheight)*bit[3]) | (sgn(z_front+z0-v[2]->pos.a[2])*bit[4]) | (sgn(v[2]->pos.a[2]-z_back-z0)*bit[5])
	};

	if((rcode[0] & rcode[1] & rcode[2]) != 0)
		return 0;
	if((rcode[0] | rcode[1] | rcode[2]) == 0)
		return 1;
	else
	{
		for(int i=0;i<3;i++)
		{
			memcpy(v[i]->I,modelVertice[t.vert[i]].I,3*sizeof(double));
			v[i]->tex_coord=modelVertice[t.vert[i]].tex_coord;
			v[i]->tex_id=modelVertice[t.vert[i]].tex_id;
			v[i]->triangle_count=modelVertice[t.vert[i]].triangle_count;
		}
		
		double z0=getCamera()->getZ0();
		Vertex *vresult=&ProjectionVertice[0];
		int resnum=0;
		for(int i=0;i<3;i++)
		{
			Vertex *vstart=v[i];
			Vertex *vend=v[(i+1)%3];

			double t_near,t_far;

			if(rcode[i]==0)
				vresult[resnum++]=*v[i];

			if(lineinsblock(vstart->pos,vend->pos,cut_bound,t_near,t_far))
			{
				if(t_near>0)
				{
					(vresult[resnum])=(1-t_near)/(vstart->pos.a[2]-z0)*(*vstart)+t_near/(vend->pos.a[2]-z0)*(*vend);
					(vresult[resnum])*=z0/(vresult[resnum].pos.a[2]-1);
					resnum++;
				}
				if(t_far<1)
				{
					(vresult[resnum])=(1-t_far)/(vstart->pos.a[2]-z0)*(*vstart)+t_far/(vend->pos.a[2]-z0)*(*vend);
					(vresult[resnum])*=z0/(vresult[resnum].pos.a[2]-1);
					resnum++;
				}
			}
		}

		if(resnum>0)
		{
			tmpTriangles[0].n=t.n;
			tmpTriangles[0].vert[0]=0;
			tmpTriangles[0].vert[1]=1;
			tmpTriangles[0].vert[2]=2;
		}
		if(resnum>2)
		{
			tmpTriangles[1].n=t.n;
			tmpTriangles[1].vert[0]=0;
			tmpTriangles[1].vert[1]=2;
			tmpTriangles[1].vert[2]=3;
		}

		memcpy(&modelVertice[0],vresult,4*sizeof(Vertex));

		return 2+resnum/3;
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

	//计算光照
	//this->calIllumination();

	

	int tri_count=modelTriangles.size();
	double z0=getCamera()->getZ0();
	cut_bound[2]=z0+(halfwidth>>1);
	cut_bound[5]=z0+(width<<2);
	Triangle *triangle;

	if(options & DRAW_BOARDER)
	{
		for(int i=0;i<tri_count;i++)
		{
			int caseid=cut(modelTriangles[i]);
			switch(caseid)
			{
			case 0:
				continue;
			case 1:
				triangle=&(modelTriangles[i]);	
			
				storeLine(&ProjectionVertice[triangle->vert[0]], &ProjectionVertice[triangle->vert[1]]);
				storeLine(&ProjectionVertice[triangle->vert[1]], &ProjectionVertice[triangle->vert[2]]);
				storeLine(&ProjectionVertice[triangle->vert[2]], &ProjectionVertice[triangle->vert[0]]);
				break;
			default:
				for(int j=0;j<caseid-10;j++)
				{
					if(tmpTriangles[j].vert[0]<0)
						continue;
					triangle=&(tmpTriangles[j]);	
					storeLine(&ProjectionVertice[triangle->vert[0]], &ProjectionVertice[triangle->vert[1]]);
					storeLine(&ProjectionVertice[triangle->vert[1]], &ProjectionVertice[triangle->vert[2]]);
					storeLine(&ProjectionVertice[triangle->vert[2]], &ProjectionVertice[triangle->vert[0]]);
				}
				break;
			}
				
			
		}
	}

	
	if(options & FILL)
	{
		for(int i=0;i<tri_count;i++)
		{
			int caseid=cut(modelTriangles[i]);
			switch(caseid)
			{
			case 0:
				continue;
			case 1:
				this->Rasterization(modelTriangles[i]);
				break;
			default:
				for(int j=0;j<caseid-10;j++)
				{
					if(tmpTriangles[j].vert[0]<0)
						continue;
					this->Rasterization(tmpTriangles[j]);
				}
				break;
			}

		}
	}

	if((options & DEPTH_TEST) && !(options & DRAW_BOARDER))
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
						RGB.rgbBlue=max(255,b->I[2]*255);
						RGB.rgbGreen=max(255,b->I[1]*255);
						RGB.rgbRed=max(255,b->I[0]*255);

						//RGB.rgbBlue=min(255,RGB.rgbBlue);
						//RGB.rgbGreen=min(255,RGB.rgbGreen);
						//RGB.rgbRed=min(255,RGB.rgbRed);

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

					//RGB.rgbBlue=min(255,RGB.rgbBlue);
					//RGB.rgbGreen=min(255,RGB.rgbGreen);
					//RGB.rgbRed=min(255,RGB.rgbRed);

					

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

	//if(x<0 || x>=width || y<0 || y>=height ) 
	//	return;

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

	Vertex *startv=&ProjectionVertice[0];
	
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

		int id1=sz_v1-startv;
		int id2=sz_v2-startv;

		double y=y1;
		
		dt=1/fabs(dx);
		for(int i=x1;i<x2;i++)
		{
			t+=dt;
			y+=m;

			double color[3];
			for(int k=0;k<3;k++)
			{
				color[k]=(1.0001-t)*(modelVertice[id1].I[k])+t*(modelVertice[id2].I[k]);
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

		int id1=sz_v1-startv;
		int id2=sz_v2-startv;
		double x=x1;
		
		dt=1/fabs(dy);
		for(int i=y1;i<y2;i++)
		{
			t+=dt;
			x+=m;

			double color[3];
			for(int k=0;k<3;k++)
			{
				color[k]=(1.0001-t)*(modelVertice[id1].I[k])+t*(modelVertice[id2].I[k]);
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

void myRenderer::MakeInterpolateTriInfo(Vertex *vert[3])
{
	Vector v[3]={vert[0]->pos,vert[1]->pos,vert[2]->pos};
	for(int i=0;i<3;i++)
	{
		v[i].a[2]=0;
	}
	currentInterpolateInfo.v0=v[0];
	Vector edge[3]={v[2]-v[1],v[0]-v[2],v[1]-v[0]};
	double edge_length[3]={norm(edge[0]),norm(edge[1]),norm(edge[2])};
	Vector normalv=cross(edge[0],edge[1]);
	double area=norm(normalv);
	Vector vertical[3]={unitof(cross(normalv,edge[0])),unitof(cross(normalv,edge[1])),unitof(cross(normalv,edge[2]))};
	Vector vx(1,0,0);
	Vector vy(0,1,0);
	for(int i=0;i<3;i++)
	{
		currentInterpolateInfo.x_gain[i]=dot(vx,vertical[i])*edge_length[i]/area;
		currentInterpolateInfo.y_gain[i]=dot(vy,vertical[i])*edge_length[i]/area;
	}


}
bool myRenderer::interpolate(double x, double y, double c[3])
{
	double dx=x-currentInterpolateInfo.v0.a[0];
	double dy=y-currentInterpolateInfo.v0.a[1];

	c[0]=1+dx*currentInterpolateInfo.x_gain[0]+dy*currentInterpolateInfo.y_gain[0];
	if(c[0]<0) return false;
	c[1]=dx*currentInterpolateInfo.x_gain[1]+dy*currentInterpolateInfo.y_gain[1];
	if(c[1]<0) return false;
	c[2]=dx*currentInterpolateInfo.x_gain[2]+dy*currentInterpolateInfo.y_gain[2];
	if(c[2]<0) return false;

	return true;
	
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
	Vector vz0=getCamera()->getPosition()+getCamera()->getFacingDirection()*getCamera()->getZ0();

	if(options & DEPTH_TEST)
	{
		double Id[3]={0};
		double Ihigh[3]={0};

		for(int i=0;i<vert_count;i++)
		{
			if(modelVertice[i].triangle_count<1) continue;

			
			double coe=-dot(lSource.direction,modelVertice[i].n);
			if(coe<0)
				coe=0;
			//double reflectv=dot(findReflectVector(unitof(modelVertice[i].pos-vz0),modelVertice[i].n),lSource.direction);
			//double coe_reflect;
			//if(reflectv>0)
			//	coe_reflect=pow(reflectv,modelVertice[i].mat.roughn);
			//else
			//	coe_reflect=0;

			for(int k=0;k<3;k++)
			{
				Id[k]=coe*lSource.I[k];

				//Ihigh[k]=coe_reflect*lSource.I[k];
				//if(Ihigh[k]<0)
				//	Ihigh[k]=0;

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
	double c[3];
	if(!interpolate(x,y,c))
		return;

	if(options & DEPTH_TEST)
	{
		double z0=getCamera()->getZ0();
		double coe[3]={1/(v[0]->pos.a[2]-z0),1/(v[1]->pos.a[2]-z0),1/(v[2]->pos.a[2]-z0)};
	
		IllumWithDepth tmp(modelVertice[ModelVertexIndex[0]],v[0]->pos.a[2],coe[0]);
		tmp*=c[0];
		IllumWithDepth tmp1(modelVertice[ModelVertexIndex[1]],v[1]->pos.a[2],coe[1]);
		tmp1*=c[1];
		IllumWithDepth tmp2(modelVertice[ModelVertexIndex[2]],v[2]->pos.a[2],coe[2]);
		tmp2*=c[2];

		tmp+=tmp1;
		tmp+=tmp2;
		tmp*=z0/(tmp.depth-1);
		

		if((options & TEXTURE_MAPPING) && tmp.tex_id>=0 && tmp.tex_id<textures.size() )
		{
			floatColor res=textures[tmp.tex_id].readPixel(tmp.tex_u,tmp.tex_v);
			for(int k=0;k<3;k++)
			{
				tmp.I[k]*=res.color[k];
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

		RGBQUAD RGB;
		
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
	int label1=0;
	if(v0.pos.a[0] > v1.pos.a[0])
		label++;
	else
		label1++;

	if(v0.pos.a[0] > v2.pos.a[0])
		label++;
	v[label]=&v0;
	Index[label]=t.vert[0];

	if(v1.pos.a[0] > v2.pos.a[0])
		label1++;
	v[label1]=&v1;
	Index[label1]=t.vert[1];

	v[3-label1-label]=&v2;
	Index[3-label1-label]=t.vert[2];

	MakeInterpolateTriInfo(v);


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
			sign=(y2-y1)>0?1:-1;
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
			sign=(y2-y1)>0?1:-1;
			for(int y=round(y1)-2*sign;y!=round(y2)+2*sign;y+=sign)
			{
				subRasterization(v,x,y,Index);
			}
		}

	}



}