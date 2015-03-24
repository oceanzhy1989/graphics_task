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

class myRenderer
{
public:

	myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset=0, int yoffset=0);
	~myRenderer();

	void setAmbient(double red, double green, double blue);
	void setLightSource(Vector direction, double I[3]);
	void SetOptions(int Option);
	int Render();

	Camera *getCamera();

public://²âÊÔ½×¶Îº¯Êý

	void AddVertex(const Vector4 &pos, const Material &mat, double color[3], bool depth_test=false);
	void AddTriangle(VertexID id1, VertexID id2, VertexID id3);
	void finishAdd();

private:

	void mergeBuffer(int x, int y, const IllumWithDepth &v);
	void storeLine(const Vertex *v1, const Vertex *v2);
	void Rasterization(const Triangle &t);
	void subRasterization(Vertex **v,int x, int y);
	void calNormalVector(Triangle *t);
	void calIllumination();

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

	HDC m_hdc;
	int xoff;
	int yoff;

	int options;

};