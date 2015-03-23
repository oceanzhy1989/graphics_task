#include "camera.h"
#include "utils.h"
#include <vector>
using namespace std;

enum RenderOptions
{
	DRAW_BOARDER=1,
	FILL=1<<1,
	DEPTH_TEST=1<<2,
	TEXTURE_MAPPING=1<<3,
	

};

class myRenderer
{
public:

	myRenderer(int winWidth, int winHeight, HDC hDC, int xoffset=0, int yoffset=0);
	~myRenderer();

	void SetOptions(int Option);
	int Render();

	Camera *getCamera();

public://²âÊÔ½×¶Îº¯Êý

	void AddVertex(const Vector4 &pos, const Material &mat, double color[3]);
	void AddTriangle(VertexID id1, VertexID id2, VertexID id3);

private:

	int width,height;
	Camera *m_camera;
	Matrix4 *m_ModelViewMatrix;
	Matrix4 *m_ProjectionMatrix;

	vector<Vertex> modelVertice;
	vector<Triangle> modelTriangles;

	vector<Vertex> modelViewVertice;
	vector<Vertex> ProjectionVertice;

	HDC m_hdc;
	int xoff;
	int yoff;

	int options;

};