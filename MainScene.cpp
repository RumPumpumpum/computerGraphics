#include <gl/glut.h>
#include <math.h>  

using namespace std;

float	Radius = 1.0;

float	cameraRadius = 5.0;
float	cameraTheta = 0.0;
float	cameraPhi = 0.0;

GLfloat		vertices[][3] = {
	// x, y, z
	{ -1.0, -1.0,  0.5 },	// 0 ���� ���ϴ�
	{ -1.0,  1.0,  0.5 },	// 1 ���� ���ϴ�
	{ 1.0,  1.0,  0.5 },	// 2 ���� ����
	{ 1.0, -1.0,  0.5 },	// 3 ���� �»��
	{ -1.0, -1.0, 0 },	// 4 �Ʒ��� ���ϴ�
	{ -1.0,  1.0, 0 },	// 5 �Ʒ��� ���ϴ�
	{ 1.0,  1.0, 0 },	// 6 �Ʒ��� ����
	{ 1.0, -1.0, 0 } };	// 7 �Ʒ��� �»��

// �ʱ⼳��
void init(void)
{
	glEnable(GL_DEPTH_TEST);
}


void polygon(int a, int b, int c, int d) {
	glBegin(GL_POLYGON);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void drawGameBoard(void) {
	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(3, 0, 4, 7);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(5, 4, 0, 1);
}

void drawAxis(void) {

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); // red color
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(100.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0); // green color
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 100.0, 0.0);

	glColor3f(0.0, 0.0, 1.0); // blue color
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 100.0);
	glEnd();
}

// ȭ���� ���� ����
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 1000);
}

// ī�޶� ��ġ ����
void cameraSetting(void)
{
	float	x, y, z;

	x = cameraRadius * cos(cameraTheta) * cos(cameraPhi);
	y = cameraRadius * sin(cameraTheta) * cos(cameraPhi);
	z = cameraRadius * sin(cameraPhi);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}

// ȭ�� �׸���
void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraSetting();

	drawAxis();

	drawGameBoard();

	glFlush();
}

void inputKey(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		cameraTheta -= 0.01; 	break;

	case 'd':
		cameraTheta += 0.01;	break;

	case 'w':
		cameraPhi += 0.01;		break;

	case 's':
		cameraPhi -= 0.01;		break;

	case 'q':
		cameraRadius += 0.1; break;

	case 'e':
		cameraRadius -= 0.1; break;

	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("�˱�� ����");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(inputKey);

	glutMainLoop();

	return 0;
}

