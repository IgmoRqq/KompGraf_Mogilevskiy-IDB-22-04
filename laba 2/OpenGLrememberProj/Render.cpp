#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	double A1[] = { 0, 0, 0 };
	double B1[] = { 7, -3, 0 };
	double C1[] = { 4,1, 0 };
	double D1[] = { 6,5, 0 };
	double E1[] = { 1, 3, 0 };
	double F1[] = { -2, 10, 0 };
	double G1[] = { -9, 2, 0 };
	double H1[] = { -3,-3, 0 };
	double A1z[] = { 0, 0, 5 };
	double B1z[] = { 7, -3, 5 };
	double C1z[] = { 4,1, 5 };
	double D1z[] = { 6,5, 5 };
	double E1z[] = { 1, 3, 5 };
	double F1z[] = { -2, 10, 5 };
	double G1z[] = { -9, 2, 5 };
	double H1z[] = { -3,-3, 5 };

	glBegin(GL_POLYGON);

	glColor3d(0.2, 0.4, 0);

	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(H1);
	glVertex3dv(A1);





	glEnd();

	glBegin(GL_POLYGON);

	glColor3d(0.4, 0.2, 0);

	glVertex3dv(A1z);
	glVertex3dv(B1z);
	glVertex3dv(C1z);
	glVertex3dv(D1z);
	glVertex3dv(E1z);
	glVertex3dv(F1z);
	glVertex3dv(G1z);
	glVertex3dv(H1z);
	glVertex3dv(A1z);





	glEnd();

	glBegin(GL_POLYGON);

	glColor3d(0.7, 0.2, 0);

	glVertex3dv(A1);
	glVertex3dv(A1z);
	glVertex3dv(B1z);
	glVertex3dv(B1);
	glEnd();
	glBegin(GL_POLYGON);

	glVertex3dv(B1);
	glVertex3dv(B1z);
	glVertex3dv(C1z);
	glVertex3dv(C1);
	glEnd();

	glBegin(GL_POLYGON);

	glVertex3dv(C1);
	glVertex3dv(C1z);
	glVertex3dv(D1z);
	glVertex3dv(D1);
	glEnd();
	glBegin(GL_POLYGON);

	glVertex3dv(D1);
	glVertex3dv(D1z);
	glVertex3dv(E1z);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_POLYGON);

	glVertex3dv(E1);
	glVertex3dv(E1z);
	glVertex3dv(F1z);
	glVertex3dv(F1);
	glEnd();
	glBegin(GL_POLYGON);

	glVertex3dv(F1);
	glVertex3dv(F1z);
	glVertex3dv(G1z);
	glVertex3dv(G1);

	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(G1);
	glVertex3dv(G1z);
	glVertex3dv(H1z);
	glVertex3dv(H1);

	glEnd();
	glBegin(GL_POLYGON);

	glVertex3dv(H1);
	glVertex3dv(H1z);
	glVertex3dv(A1z);
	glVertex3dv(A1);





	glEnd();


	//������ ��������� ���������� ��������
	


	double A11[] = { 0.1, 0.1, 0.1 };
	double B11[] = { 7.1, -3.1, 0.1 };
	double C11[] = { 4.1,1.1, 0.1 };
	double D11[] = { 6.1,5.1, 0.1 };
	double E11[] = { 1.1, 3.1, 0.1 };
	double F11[] = { -2.1, 10.1, 0.1 };
	double G11[] = { -9.1, 2.1, 0.1 };
	double H11[] = { -3.1,-3.1, 0.1 };
	double A11z[] = { 0.1, 0.1, 5.1 };
	double B11z[] = { 7.1, -3.1, 5.1 };
	double C11z[] = { 4.1,1.1, 5.1 };
	double D11z[] = { 6.1,5.1, 5.1 };
	double E11z[] = { 1.1, 3.1, 5.1 };
	double F11z[] = { -2.1, 10.1, 5.1 };
	double G11z[] = { -9.1, 2.1, 5.1 };
	double H11z[] = { -3.1,-3.1, 5.1 };

	 
	
	// ������� �����
	glColor3d(0.7, 0.2, 0);
	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(A11);
	glTexCoord2d(1, 0);
	glVertex3dv(A11z);
	glTexCoord2d(1, 1);
	glVertex3dv(B11z);
	glTexCoord2d(0, 1);
	glVertex3dv(B11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(B11);
	glTexCoord2d(1, 0);
	glVertex3dv(B11z);
	glTexCoord2d(1, 1);
	glVertex3dv(C11z);
	glTexCoord2d(0, 1);
	glVertex3dv(C11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(C11);
	glTexCoord2d(1, 0);
	glVertex3dv(C11z);
	glTexCoord2d(1, 1);
	glVertex3dv(D11z);
	glTexCoord2d(0, 1);
	glVertex3dv(D11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(D11);
	glTexCoord2d(1, 0);
	glVertex3dv(D11z);
	glTexCoord2d(1, 1);
	glVertex3dv(E11z);
	glTexCoord2d(0, 1);
	glVertex3dv(E11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(E11);
	glTexCoord2d(1, 0);
	glVertex3dv(E11z);
	glTexCoord2d(1, 1);
	glVertex3dv(F11z);
	glTexCoord2d(0, 1);
	glVertex3dv(F11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(F11);
	glTexCoord2d(1, 0);
	glVertex3dv(F11z);
	glTexCoord2d(1, 1);
	glVertex3dv(G11z);
	glTexCoord2d(0, 1);
	glVertex3dv(G11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(G11);
	glTexCoord2d(1, 0);
	glVertex3dv(G11z);
	glTexCoord2d(1, 1);
	glVertex3dv(H11z);
	glTexCoord2d(0, 1);
	glVertex3dv(H11);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3dv(H11);
	glTexCoord2d(1, 0);
	glVertex3dv(H11z);
	glTexCoord2d(1, 1);
	glVertex3dv(A11z);
	glTexCoord2d(0, 1);
	glVertex3dv(A11);
	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}