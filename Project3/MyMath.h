#pragma once
#include <math.h>
#include <iostream>

#include <fstream>
#include <string>
#include <GL/glut.h>


using namespace std;

const float PI = 3.1415926f;

class Vector3f {
public:
	float x, y, z, *array;

	Vector3f(float x = 0.0, float y = 0.0, float z = 0.0)
		: x(x), y(y), z(z) {
		array = new float[3];
	}

	Vector3f operator * (const float lambda) {
		return Vector3f(x * lambda, y * lambda, z * lambda);
	}

	Vector3f operator + (const Vector3f & tempVector3f) {
		return Vector3f(x + tempVector3f.x, y + tempVector3f.y,
			z + tempVector3f.z);
	}

	Vector3f operator - (const Vector3f & tempVector3f) {
		return Vector3f(x - tempVector3f.x, y - tempVector3f.y,
			z - tempVector3f.z);
	}

	Vector3f operator * (const Vector3f & tempVector3f) {
		return Vector3f(y * tempVector3f.z - z * tempVector3f.y,
			z * tempVector3f.x - x * tempVector3f.z,
			x * tempVector3f.y - y * tempVector3f.x);
	}

	Vector3f & operator = (const Vector3f & tempVector3f) {
		x = tempVector3f.x;
		y = tempVector3f.y;
		z = tempVector3f.z;

		return (*this);
	}

	~Vector3f() {
		delete[] array;
	}

	float Modulus() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3f & Unitization() {
		float modulus = Modulus();
		x /= modulus;
		y /= modulus;
		z /= modulus;

		return (*this);
	}

	float * operator () () {
		array[0] = x;
		array[1] = y;
		array[2] = z;
		return array;
	}

	
};



void LoadPly(char *file_name, int style);
double r_ang = 0.0;
double R = 255.0, G = 0.0, B = 0.0;
int style = GL_POLYGON;
/*
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 1.0f, 15.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 1.0f };
*/
void LoadPly(char *file_name, int style)
{
	ifstream fin(file_name, ios_base::in);

	if (!fin.is_open())
	{
		cout << "Cannot read the file." << endl;
		exit(0);
	}

	string str;
	int vertex, face;
	char ch;

	while (!fin.eof())
	{
		fin.get(ch);
		if (ch != ' ' && ch != '\t' && ch != '\n')
		{
			str.push_back(ch);
		}
		else
		{
			if (str == "vertex")
			{
				str.clear();
				getline(fin, str, '\n');
				vertex = atoi(str.c_str());
			}
			else if (str == "face")
			{
				str.clear();
				getline(fin, str, '\n');
				face = atoi(str.c_str());
			}
			else if (str == "end_header")
			{
				str.clear();
				break;
			}
			else
				str.clear();
		}
	}

	double *vertex_arrayX = new double[vertex];
	double *vertex_arrayY = new double[vertex];
	double *vertex_arrayZ = new double[vertex];

	int pos = 0;
	int counter = 0;
	double number;
	double max_edge = 0;
	static double temp_max = 0;

	while (!fin.eof())
	{
		fin.get(ch);
		if (ch != ' ' && ch != '\t' && ch != '\n')
			str.push_back(ch);

		else
		{
			if (counter == vertex)	break;
			if (str == "")	continue;

			else if (pos % 3 == 0)
			{
				number = atof(str.c_str());
				vertex_arrayX[counter] = number;
				str.clear();
			}
			else if (pos % 3 == 1)
			{
				number = atof(str.c_str());
				vertex_arrayY[counter] = number;
				str.clear();
			}
			else if (pos % 3 == 2)
			{
				number = atof(str.c_str());
				vertex_arrayZ[counter] = number;
				str.clear();
				counter++;
			}
			pos++;

			if (abs((int)number) > max_edge)
				max_edge = abs((int)number);
		}
	}

	int point[4];
	int i = 0;
	counter = 0;

	while (!fin.eof())
	{
		fin.get(ch);
		if (ch != ' ' && ch != '\t' && ch != '\n')
			str.push_back(ch);
		else
		{
			if (counter == face)		break;
			if (ch == '\n')
			{   
				GLfloat vc1[3], vc2[3];
				GLfloat a, b, c;
				GLdouble r;
				vc1[0] = vertex_arrayX[point[2]] - vertex_arrayX[point[1]];
				vc1[1] = vertex_arrayY[point[2]] - vertex_arrayY[point[1]];
				vc1[2] = vertex_arrayZ[point[2]] - vertex_arrayZ[point[1]];
				vc2[0] = vertex_arrayX[point[3]] - vertex_arrayX[point[1]];
				vc2[1] = vertex_arrayY[point[3]] - vertex_arrayY[point[1]];
				vc2[2] = vertex_arrayZ[point[3]] - vertex_arrayZ[point[1]];
				a = vc1[1] * vc2[2] - vc2[1] * vc1[2];
				b = vc2[0] * vc1[2] - vc1[0] * vc2[2];
				c = vc1[0] * vc2[1] - vc2[0] * vc1[1];
				r = sqrt(a * a + b * b + c * c);
				float nor[3];
				nor[0] = a / r;
				nor[1] = b / r;
				nor[2] = c / r;

				glNormal3f(nor[0], nor[1], nor[2]);

				glBegin(style);

				for (int i = 1; i <= point[0]; i++)
					glVertex3f(vertex_arrayX[point[i]], vertex_arrayY[point[i]], vertex_arrayZ[point[i]]);
				glEnd();

				counter++;
			}
			else if (str == "")	continue;

			else
			{
				point[i % 4] = atoi(str.c_str());
				i++;
				str.clear();
			}
		}
	}
	fin.close();

	if (max_edge > temp_max)
	{
		glLoadIdentity();
		glOrtho(-(max_edge * 2), (max_edge * 2), -(max_edge * 2), (max_edge * 2), -(max_edge * 2), (max_edge * 2));
	}
	temp_max = max_edge;
}
