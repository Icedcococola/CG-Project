#pragma once
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl/glut.h>  
#include <cmath>

//粒子类
class particle {

	friend class emitter;
private:

	bool always;
	float x, y;
	float sizex;
	float sizey;
	float vx;
	float vy;
	float ax;
	float ay;
	float restlife;
	float theta;
	float color[3];

	void drawparticle();
	void display();


public:
	particle(float sx, float sy, float vx0, float vy0, float ax0, float ay0, float rl, float theta0, float* color0, bool always0);
};

void particle::drawparticle()
{
	/*glBegin(GL_QUADS);
	glVertex2f(-2, 1.2);
	glVertex2f(-1.6, 0.4);
	glVertex2f(1.2, -0.8);
	glVertex2f(0.8, 0.8);
	glEnd();*/
	glScalef(0.1, 0.1, 0.1);
	glutSolidIcosahedron();

}


void particle::display()
{
	if (restlife > 0) {
		glPushMatrix();
		glColor3f(color[0], color[1], color[2]);
		glTranslatef(x, y, 0);
		glRotatef(theta, 0, 0, 1);
		glScalef(sizex, sizey, 0);
		drawparticle();
		glPopMatrix();
		restlife -= 0.2;
		x += vx;
		y += vy;
		vy += ay;
		vx += ax;
	}
}

particle::particle(float sx, float sy, float vx0, float vy0, float ax0, float ay0, float rl, float theta0, float* color0, bool always0) {
	sizex = sx;
	sizey = sy;
	vx = vx0;
	vy = vy0;
	ax = ax0;
	ay = ay0;
	restlife = rl;
	theta = theta0;
	color[0] = *color0;
	color[1] = *(color0 + 1);
	color[2] = *(color0 + 2);
	always = always0;
}

//发射器类
class emitter {
private:
	int v0;
	float x1, y1, x2, y2;
	particle* (*f)();
	particle **p;

public:
	emitter(int v0, float x, float xx, float y, float yy);
	void emit(particle* (b)());
	void flush();
};


emitter::emitter(int v, float x, float xx, float y, float yy) {

	p = new particle*[v];
	v0 = v;
	x1 = x;
	x2 = xx;
	y1 = y;
	y2 = yy;
}

void emitter::emit(particle* (b)()) {
	for (int i = 0; i < v0; i++) {
		f = b;
		p[i] = b();
		float a = (float)(rand() % v0);
		p[i]->x = x1 + a / v0 * (x2 - x1);
		p[i]->y = y1 + a / v0 * (y2 - y1);
	}
}

void emitter::flush() {
	for (int i = 0; i < v0; i++) {

		p[i]->display();
		if (p[i]->restlife < 0) {
			delete p[i];
			p[i] = f();
			float a = (float)(rand() % v0);
			p[i]->x = x1 + a / v0 * (x2 - x1);
			p[i]->y = y1 + a / v0 * (y2 - y1);
		}
	}
}
