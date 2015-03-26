#ifndef __MY_RENDERER_H__
#define __MY_RENDERER_H__

#include "bmpReader.h"
#include "camera.h"
#include "utils.h"
#include <vector>
#include <list>
using namespace std;



enum RenderOptions
{
	DRAW_BOARDER=1,
	FILL=1<<1,
	DEPTH_TEST=1<<2,
	ENABLE_A_BUFFER=1<<3,
	TEXTURE_MAPPING=1<<4,
	

};

struct Pxyz{float x,y,z;};
struct FACET{Pxyz n;Pxyz dot[3];};		//法向量，三节点

class myRenderer
{
public:

	myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset=0, int yoffset=0);
	~myRenderer();

	void setHDC(HDC hDC);

	void setAmbient(double red, double green, double blue);
	void setLightSource(Vector direction, double I[3]);
	void SetOptions(int Option);
	void calIllumination();
	bool loadNewTexture(const char *tex_file_name);
	int Render();

	Camera *getCamera();

public://测试阶段函数

	void AddVertex(const Vertex &v);
	void AddVertex(const Vector4 &pos, const Material &mat, double color[3], bool depth_test=false);
	void AddTriangle(VertexID id1, VertexID id2, VertexID id3);
	bool ReadSTL(const char *filename, Material mat, double color[3], Matrix4 transformMat=IdentityMatrix);
	void finishAdd();

	void clearBuffer();

private:

	void mergeBuffer(int x, int y, const IllumWithDepth &v);
	void storeLine(const Vertex *v1, const Vertex *v2);
	void Rasterization(const Triangle &t);
	void subRasterization(Vertex **v,int x, int y, int ModelVertexIndex[3]);
	void calNormalVector(Triangle *t);
	

private:

	int width,height;
	int halfwidth,halfheight;

	list<IllumWithDepth> *bufferMap;

	Camera *m_camera;
	Matrix4 *m_ModelViewMatrix;
	Matrix4 *m_ProjectionMatrix;

	vector<Vertex> modelVertice;
	vector<Triangle> modelTriangles;

	vector<Vertex> modelViewVertice;
	vector<Vertex> ProjectionVertice;

	LightSource lSource;
	double ambient[3];
	vector<MYBITMAP> textures;

	HDC m_hdc;
	int xoff;
	int yoff;

	int options;

};

#endif
