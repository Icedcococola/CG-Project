#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "MyMath.h"
#include "particle.h"

#include <fstream>
#include <string>
using namespace std;

const int XINIT = 700, YINIT = 700;
#define BMP_Header_Length 54  
const int XMAX = 147, YMAX = 189;

float height[XMAX + 1][YMAX + 1];
float a = 0;
float b = 0.5;
float density = 0.0f;
GLuint back;
GLuint book;
GLuint darkroom;
GLuint word1;
GLuint word2;
GLuint word3;
GLuint word4;
GLuint background;
int sum;
float tt = 0;
int bomb = 0;
int dh = 0;
float h = 2;
float s = 6;
GLfloat hei = 0.85;
GLint green = 1;
GLint blue = 1;
float r = 0;

int ds = 0;
int last = 0;
float cycles = 0;
int times = 0;
int t = 0;
int k = 0;
float r1 = 0.8;
float r2 = 0.25;
float r3 = 0.03;
float r4 = 0.05;
float r5 = 0.25;
GLUquadricObj *c1 = gluNewQuadric();
GLUquadricObj *c2 = gluNewQuadric();
GLUquadricObj *c3 = gluNewQuadric();
GLfloat length = 0.8;
emitter *emit1;


const int MY_MODE = 1;

Vector3f position[XMAX + 1][YMAX + 1], normal[XMAX + 1][YMAX + 1];

GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (//!power_of_two(width)
			//||
			//!power_of_two(height)
			//||
			width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256;
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);
	free(pixels);
	return texture_ID;
}

void InitNormal() {
	for (int i = 0; i <= XMAX; i++)
		for (int j = 0; j <= YMAX; j++) {
			
			normal[i][j].x = 0.0;
			normal[i][j].y = 0.0;
			normal[i][j].z = 1.0;
			
			if (i != XMAX)
				normal[i][j] = normal[i][j] + (position[i][j] - position[i + 1][j]).Unitization() * (height[i][j] - height[i + 1][j]);
			if (i != 0)
				normal[i][j] = normal[i][j] + (position[i][j] - position[i - 1][j]).Unitization() * (height[i][j] - height[i - 1][j]);
			if (j != YMAX)
				normal[i][j] = normal[i][j] + (position[i][j] - position[i][j + 1]).Unitization() * (height[i][j] - height[i][j + 1]);
			if (j != 0)
				normal[i][j] = normal[i][j] + (position[i][j] - position[i][j - 1]).Unitization() * (height[i][j] - height[i][j - 1]);
			normal[i][j].Unitization();
		}
}
void RenderScene() {


	glEnable(GL_FOG);
	GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 0.1f };
	GLuint fogMode = GL_EXP2;
	glFogi(GL_FOG_MODE, fogMode);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, density);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 20.0f);


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	if (times >= 500 && times < 560) {
		background = word1;
	}
	else if (times >= 560 && times < 620) {
		background = word2;
	}
	else if (times >= 620 && times < 680) {
		background = word3;
	}
	else if (times >= 680) {
		background = word4;
	}
	else {
		background = back;
	}
	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-3.20f, -2.7f, -2.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-3.20f, 2.7f, -2.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(3.20f, 2.7f, -2.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(3.20f, -2.7f, -2.0f);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	if (times > 0 && times < 450) {
		glPushMatrix();
		glTranslatef(-0.1,0.20, 0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(cycles, 0.0, 0.0, 1.0);
		glScalef(30, 5, 35);
		glutSolidCube(0.05);
		
		glutSolidCube(0.05);

		glPopMatrix();
		/*
		glPushMatrix();
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(cycles, 0.0, 0.0, 1.0);
		glTranslatef(-0.17, 0.15, 0);
		glScalef(4, 1, 7);
		glutSolidCube(0.35);

		glPopMatrix();*/
	}
	glPushMatrix();

	glTranslatef(0, 0, 0.4);
	{
		GLfloat sunLightPosition[] = { 0.0f,a, 1.0f, 0.0f };
		GLfloat sunLightAmbient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		GLfloat sunLightDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		GLfloat sunLightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, sunLightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, sunLightAmbient);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, sunLightSpecular);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}


	if (times >= 450 && times < 1000) {

		{
			GLfloat triangleMatAmbient[] = { 0.231250, 0.231250, 0.231250, 1.000000 };
			GLfloat triangleMatDiffuse[] = { 0.277500, 0.277500, 0.277500, 1.000000 };
			GLfloat triangleMatSpecular[] = { 0.773911, 0.773911, 0.773911, 1.000000 };
			GLfloat triangleMatShininess = 89.599998;
			glMaterialfv(GL_FRONT, GL_AMBIENT, triangleMatAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, triangleMatDiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, triangleMatSpecular);
			glMaterialf(GL_FRONT, GL_SHININESS, triangleMatShininess);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
		{
			for (int i = 0; i < XMAX; i++)
				for (int j = 0; j < YMAX; j++) {
					glNormal3fv(normal[i][j]());
					glVertex3fv(position[i][j]());
					glNormal3fv(normal[i + 1][j]());
					glVertex3fv(position[i + 1][j]());
					glNormal3fv(normal[i][j + 1]());
					glVertex3fv(position[i][j + 1]());
				}
			for (int i = 0; i < XMAX; i++)
				for (int j = 0; j < YMAX; j++) {
					glNormal3fv(normal[i + 1][j]());
					glVertex3fv(position[i + 1][j]());
					glNormal3fv(normal[i + 1][j + 1]());
					glVertex3fv(position[i + 1][j + 1]());
					glNormal3fv(normal[i][j + 1]());
					glVertex3fv(position[i][j + 1]());
				}
		}
		glEnd();
	}

	if (times > 1500) {
		glDisable(GL_LIGHT0);
		{
			GLfloat stonea[] = { 0.147250, 0.149500, 0.144500, 1.000000 };
			GLfloat stoned[] = { 0.011640, 0.016480, 0.016480, 1.000000 };
			GLfloat stones[] = { 0.000281, 0.005802, 0.006065, 1.000000 };
			GLfloat stoneshi = 0.0001;
			glMaterialfv(GL_FRONT, GL_AMBIENT, stonea);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, stoned);
			glMaterialfv(GL_FRONT, GL_SPECULAR, stones);
			glMaterialf(GL_FRONT, GL_SHININESS, stoneshi);
			glShadeModel(GL_SMOOTH);
		}
		{
			GLfloat sunLightPosition[] = { 0.0f,-1.0f, 7.0f, 1.0f };
			GLfloat sunLightAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
			GLfloat sunLightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
			GLfloat sunLightSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
			glLightfv(GL_LIGHT1, GL_POSITION, sunLightPosition);
			glLightfv(GL_LIGHT1, GL_AMBIENT, sunLightAmbient);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, sunLightDiffuse);
			glLightfv(GL_LIGHT1, GL_SPECULAR, sunLightSpecular);
			glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 7.0);
			GLfloat dir[] = { 0,0.1,-1 };
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);

			//
		}
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, darkroom);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-3.20f, -2.3f, -1.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-3.20f, 2.3f, -1.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(3.20f,2.3f, -1.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(3.20f, -2.3f, -1.5f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHT0);
		if (last == 0)
		{
			glPushMatrix();
			glTranslatef(0, -0.4, -1);
			glScalef(0.3, 0.3, 0.3);
			glutSolidIcosahedron();
			glTranslatef(-1, -1, 0);
			glutSolidIcosahedron();
			glTranslatef(2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-1, -1, 0);
			glutSolidIcosahedron();
			glTranslatef(-2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(4, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-1, -1, 0);
			glutSolidIcosahedron();
			glTranslatef(-2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(6, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-1, 0, 1);
			glutSolidIcosahedron();
			glTranslatef(-2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(1, 1, 0);
			glutSolidIcosahedron();
			glTranslatef(2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-1, 1, 0);
			glutSolidIcosahedron();
			glTranslatef(0, -1, 1);
			glutSolidIcosahedron();
			glTranslatef(-1, -1, 0);
			glutSolidIcosahedron();
			glTranslatef(2, 0, 0);
			glutSolidIcosahedron();
			glTranslatef(-1, 0, 1);
			glutSolidIcosahedron();
			//glPopMatrix();
		}


		if (bomb == 1) {
			glPushMatrix();
			glTranslatef(0, h, s);
			glRotatef(-90, 1.0, 0.0, 0.0);
			glRotatef(45, 1.0, 1.0, 0.0);
			glutSolidSphere(r1, 40, 50);
			glTranslatef(0.00, 0.00, 0.7);
			gluCylinder(c1, r2, r2, 0.25, 100, 3);

			glTranslatef(0.00, 0.00, 0.25);

			gluDisk(c2, r4, r5, 100, 3);
			glColor3f(1, 1, 0);
			gluCylinder(c3, r3, r3, length, 100, 3);

			glTranslatef(0.00, 0.00, hei);
			{
				GLfloat fire1[] = { 0.0f,0.0f, 0.0f, 1.0f };
				GLfloat fire2[] = { 0.9f, 0.9f, 0.9f, 1.0f };
				GLfloat fire3[] = { 0.1f, 0.1f, 0.1f, 1.0f };
				GLfloat fire4[] = { 0.01f, 0.01f, 0.01f, 1.0f };
				glLightfv(GL_LIGHT2, GL_POSITION, fire1);
				glLightfv(GL_LIGHT2, GL_AMBIENT, fire2);
				glLightfv(GL_LIGHT2, GL_DIFFUSE, fire3);
				glLightfv(GL_LIGHT2, GL_SPECULAR, fire4);

			}
			glEnable(GL_LIGHT2);
			glEnable(GL_COLOR_MATERIAL);
			glColor3f(1, green, blue);
			glutSolidSphere(r, 40, 50);

			glutSolidCone(r * 2 / 3, 3 * r, 100, 3);
			glutSolidCone(r * 2 / 3, 3 * r, 100, 3);
			for (int i = 0; i < 8; i++) {
				glRotatef(-45, 0.0, 1.0, 0.0);
				glutSolidCone(r * 2 / 3, 3 * r, 100, 3);
			}
			for (int i = 0; i < 8; i++) {
				glRotatef(-45, 1.0, 0.0, 0.0);
				glutSolidCone(r * 2 / 3, 3 * r, 100, 3);
			}
			glPopMatrix();
			glTranslatef(0, 0, -1);

			if (length < 0 && last < 100) {
				glDisable(GL_LIGHT1);
				glDisable(GL_LIGHT0);
				//glDisable(GL_LIGHT2);
				glTranslatef(0, -0.18, 0);
				emit1->flush();
			}
			//glPopMatrix();

			
		}
		if (last >= 100) {

			/*{
				const GLfloat light_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
				const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				const GLfloat light_position[] = { 0.0f, 1.0f, 15.0f, 0.0f };
				glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
				glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}*/
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			{

				const GLfloat mat_ambient[] = { 0.592250, 0.592250, 0.592250, 1.000000 };
				const GLfloat mat_diffuse[] = { 0.907540, 0.907540, 0.907540, 1.000000 };
				const GLfloat mat_specular[] = { 0.508273, 0.508273, 0.508273, 1.000000 };
				const GLfloat high_shininess[] = { 1.200001 };
				glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
				glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
			}
            glEnable(GL_LIGHT0);
			
			glColor3f(0.4, 0.4, 0.4);
			//glLoadIdentity();
			glOrtho(-0.1, 0.1, 0.05, 0.25, -0.1, 0.1);
			glRotatef(180, 0, 1, 0);
			glTranslatef(0, -0.08, 0);
			if (bomb == 1) {
				glPushMatrix();
				glScalef(1.2, 1.2, 1.2);
				glTranslatef(0, -0.04, 0);
				glScalef(0.8, 0.8, 0.8);
				glTranslatef(0, 0.05, 0);
				LoadPly("happy_vrip_res4.ply", style);
				glPopMatrix();
			}
			

			if (bomb == 2) {
				GLfloat pos[] = { 0.0f,-1.0f, 7.0f, 1.0f };
				GLfloat amb[] = { 0.9f, 0.9f, 0.9f, 1.0f };
				GLfloat dif[] = { 0.9f, 0.9f, 0.9f, 1.0f };
				GLfloat spe[] = { 0.8f, 0.8f, 0.8f, 1.0f };
				glLightfv(GL_LIGHT1, GL_POSITION, pos);
				glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
				glLightfv(GL_LIGHT1, GL_SPECULAR, spe);
				glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 2.0);
				GLfloat dire[] = { 0,0.195,-1 };
				glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dire);
				glEnable(GL_LIGHT1);
				/*
				　　//亮金
　　                0.247250, 0.224500, 0.064500, 1.000000,
　　                0.346150, 0.314300, 0.090300, 1.000000,
　　                0.797357, 0.723991, 0.208006, 1.000000,
　　                83.199997,

					//金银
					0.240000, 0.228500, 0.194500, 1.000000
					0.306150, 0.294300, 0.180300, 1.000000
					0.787357, 0.743991, 0.508006, 1.000000

					//亮银
					0.231250, 0.231250, 0.231250, 1.000000,
　                　0.277500, 0.277500, 0.277500, 1.000000,
　                　0.773911, 0.773911, 0.773911, 1.000000,
　                　89.599998,
				*/
				if (tt >= 2) {
					const GLfloat mat_ambient1[] = { 0.247250, 0.224500, 0.064500, 1.000000 };
					const GLfloat mat_diffuse1[] = { 0.346150, 0.314300, 0.090300, 1.000000 };
					const GLfloat mat_specular1[] = { 0.797357, 0.723991, 0.208006, 1.000000 };
					const GLfloat high_shininess1[] = { 0.38 };
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
					glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
					glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess1);
					glPushMatrix();
					glScalef(0.999, 0.999, 0.999);
					glScalef(0.8, 0.8, 0.8);
					glTranslatef(0, 0.05, 0);
					LoadPly("happy_vrip.ply", style);
					glPopMatrix();
				}
				
				
				if (tt < 2 && tt >=1) {
					const GLfloat mat_ambient1[] = { 0.240000, 0.228500, 0.194500, 1.000000 };
					const GLfloat mat_diffuse1[] = { 0.306150, 0.294300, 0.180300, 1.000000 };
					const GLfloat mat_specular1[] = { 0.787357, 0.743991, 0.508006, 1.000000 };
					const GLfloat high_shininess1[] = { 0.38 };
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
					glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
					glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess1);
					glPushMatrix();
					glScalef(0.9995, 0.9995, 0.9995);
					glScalef(0.8, 0.8, 0.8);
					glTranslatef(0, 0.05, 0);
					LoadPly("happy_vrip_res2.ply", style);
					glTranslatef(0, 0.1, 0);
					glPopMatrix();
				}
				
					
				
				
				if (tt < 1) {
					const GLfloat mat_ambient[] = { 0.231250, 0.231250, 0.231250, 1.000000 };
					const GLfloat mat_diffuse[] = { 0.277500, 0.277500, 0.277500, 1.000000 };
					const GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.000000 };
					const GLfloat high_shininess[] = { 0.38 };
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
					glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
					glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
					glScalef(0.8, 0.8, 0.8);
					glTranslatef(0, 0.05, 0);
					LoadPly("happy_vrip_res3.ply", style);
				}
			}
			
		}



		glPopMatrix();
		glDisable(GL_LIGHT2);
		glDisable(GL_COLOR_MATERIAL);






	}
	glPopMatrix();
	glutSwapBuffers();
}


void ReshapeWindow(int w, int h) {
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, w / h, 1, 18);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0);
}

void idle() {

	times++;
	cycles += 0.4;
	if (cycles >= 225) {
		cycles = 225;
	}
	if (bomb == 2) {
		tt += 0.5;
	}
	if (bomb == 1 ) {
		t++;
		if (t < 200) {
			h += 0.005;
		}
		if (t >= 100) {
			h -= 0.005;
		}
		s -= 0.018;

		if (length > 0) {
			r = 0.1;
		}
		else {
			last++;
			r = 0;
			r1 = 0;
			r2 = 0;
			r3 = 0;
			r4 = 0;
			r5 = 0;
		}
		length -= 0.0025;

		if (hei > 0.05) {
			hei -= 0.0025;
		}
		if (green == 1) {
			green = 0;
			k = 1 - k;
		}
		else {
			green = 1;
		}
		if (green == 1 && k == 1) {
			blue = 1;
		}
		else {
			blue = 0;
		}



	}
	if (times > 1500) {
		a = 1;
	}
	if (times >= 450 && last < 100) {
		a += b;
		if (a == 5) {
			b = -0.5;
			sum++;
		}
		else if (a == -5) {
			b = 0.5;
			sum++;
		}
		if (sum >= 10 && sum < 30) {
			density += 0.0015;
		}
		else if (sum >= 30) {
			density -= 0.0002;
		}


	}


	glutPostRedisplay();
}


void keyboardEvents(unsigned char key, int x, int y) {
	switch (key)
	{
	case ' ':
		glEnable(GL_LIGHT1); break;
	case 'b':
		bomb = bomb +1 ;

		break;
	
	}
}


particle* stone()
{
	float theta[8] = { 0,45,90,135,180,225,270,315 };
	float color[] = { 0.2f,0.2f,0.2f };
	float size = rand() % 15 * 0.2;
	float vx = (rand() % 10 - 4) / 80;
	float ax = -1 / 8000;
	particle* p = new particle(size, size, float(rand() % 10 - 4) / 80, float(rand() % 10 - 4) / 80, 0, -4.9 / 40000, rand() % 100, theta[rand() % 8], color, false);
	return p;
}


void MyInit() {
	for (int i = 0; i <= XMAX; i++)
		for (int j = 0; j <= YMAX; j++) {
			{
				if (i % 20 >= 10 && i % 20 < 19 && j % 20 >= 10 && j % 20 < 19)
					height[i][j] = 2.0;
				else
					height[i][j] = 0.0;
			}
			position[i][j].x = i * 1.4 / XMAX - 0.8;
			position[i][j].y = j * 1.6 / YMAX - 0.6;
			position[i][j].z = 0.0;
		}

	InitNormal();

	emit1 = new emitter(1000, 0, 0, 0, 0);
	emit1->emit(stone);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 40);
	glutInitWindowSize(XINIT, YINIT);
	glutCreateWindow("盗墓笔记");
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	back = load_texture("mountain-hole.bmp");
	book = load_texture("book.bmp");
	darkroom = load_texture("door.bmp");
	word1 = load_texture("1.bmp");
	word2 = load_texture("2.bmp");
	word3 = load_texture("3.bmp");
	word4 = load_texture("4.bmp");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ReshapeWindow);
	MyInit();
	glutKeyboardFunc(&keyboardEvents);
	glutIdleFunc(idle);
	glutMainLoop();
}