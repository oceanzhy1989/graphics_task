#include "camera.h"
#include "graphicsAPI.h"
#include "utils.h"
#include "myRenderer.h"
#include "windows.h"

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1



class Controller
{
public:
	Controller();
	~Controller();

	void init(int winWidth, int winHeight, HDC hdc);
	void setDC(HDC hDC);
	void setKeyState(char key, bool state);
	void setMouseState(int button, bool state, LPARAM lParam);
	void mouseMove(LPARAM lParam);
	void scrollCamera(int dist);
	void switchDrawType();

	

	void display();
private:
	bool WisDown;
	bool AisDown;
	bool SisDown;
	bool DisDown;
	bool SpaceIsDown;

	bool rButtonIsDown;
	int old_mouseX,old_mouseY;
	int mouseX,mouseY;

	int drawType[2];
	int currentType;

	myRenderer *ren;

};