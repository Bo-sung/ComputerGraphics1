
#define NOMINMAX


#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Light_Button.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Fl/Fl_Double_Window.h>

#include "stdio.h"
#include "math.h"
#include "3DUtils.h"

#include "Viewer.h"
#include <vector>

#include "core.h"
#include "Mover.h"	
#include "MyContact.h"


class MyGlWindow : public Fl_Gl_Window {
public:
	MyGlWindow(int x, int y, int w, int h);


	Fl_Slider* time;
	/// <summary>
	/// 1 : 시뮬레이션 실행중 0 : 시뮬레이션 멈춤
	/// </summary>
	int run;
	void update();
	void drawStuff();
	void doPick();
	void test();
	void Step();
	int selected;

private:
	void draw();					// standard FlTk


	int handle(int);				// standard FlTk

	float fieldOfView;
	Viewer* m_viewer;

	Mover* m_mover;

	cyclone::ParticleContact m_contact[2]; //contact 발생할 수 있는 충돌 수 = 2 (넉넉하게)
	//각종 충돌을 위한 충돌생성기 : 위에 정의한 MyGroundContact 저장 위한 컨테이너
	//그런데, 왜 type이 ParticleContactGenerator일까? => 일반적으로 상속받은 클래스의 경우
	//parent 타입으로 설정함
	std::vector<cyclone::ParticleContactGenerator*> m_contactGenerators;
	//충돌 해결기 (impulse를 계산해서 속도를 변환시키고, 위치를 변경함)
	cyclone::ParticleContactResolver* m_resolver;

	int maxPossibleContact = 2; //충돌 발생할 수 있는 최대 수

	void putText(char* string, int x, int y, float r, float g, float b);
	void setProjection(int clearProjection = 1);
	void getMouseNDC(float& x, float& y);
	void setupLight(float x, float y, float z);

public:
	//Widgets
	Fl_Light_Button* m_btn_run;
	Fl_Button* m_btn_test;
	Fl_Button* m_btn_step;
};

