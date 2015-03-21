#ifndef __GRAPHICS_API_H__
#define __GRAPHICS_API_H__

#include <windows.h>

#define SrcCopy 0xcc0020

class graphicsAPI
{
public:

	void init(int Width, int Height);
	static graphicsAPI *getInstance();

	bool storePixel(int x, int y, RGBQUAD color);
	void flush();
	void draw(HDC hDC, int left=0, int top=0);


private:

	graphicsAPI();
	~graphicsAPI();

	static graphicsAPI *instance;

	BYTE *pixels;
	int width;
	int height;
	int rectSize;

	BITMAPINFO m_bitmapInfo;
};
	

#endif
