#include <gl/glut.h>
#include <math.h>  
#include <iostream>

using namespace std;

// â�� ũ��
#define winSize		500

// �÷��̾� ����
float	playerRadius = 20.0;
int		selectingPlayer = -1;
float	height = 20.0; // �Ѳ��� ����

// ī�޶� ����
float	cameraZoom = 610.0;

// ������ ������ ����
float	boardWidth = 250.0;
float	boardHeight = -5.0;

// �÷��̾��� ��ǥ, ���콺�� ��ǥ, ������ �Ÿ�
float playerX = 0.0;
float playerY = 0.0;
float mouseX = 0.0;
float mouseY = 0.0;
float movingX = 0.0;
float movingY = 0.0;

GLUquadricObj* qobj; // ����� ������Ʈ

GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f }; // �ֺ���
GLfloat diffuseLight[] = { 0.7f,0.7f,0.7f,1.0f }; // �л걤
GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f }; // �ݻ籤
GLfloat position[] = { 400.0f,300.0f,700.0f,1.0f }; // ������ ��ġ

GLfloat		vertices[][3] = {
	// x, y, z
	{ -boardWidth, -boardWidth,  0 },	// 0 ���� ���ϴ�
	{ -boardWidth,  boardWidth,  0 },	// 1 ���� ���ϴ�
	{ boardWidth,  boardWidth,  0 },	// 2 ���� ����
	{ boardWidth, -boardWidth,  0 },	// 3 ���� �»��
	{ -boardWidth, -boardWidth, boardHeight },	// 4 �Ʒ��� ���ϴ�
	{ -boardWidth,  boardWidth, boardHeight },	// 5 �Ʒ��� ���ϴ�
	{ boardWidth,  boardWidth, boardHeight },	// 6 �Ʒ��� ����
	{ boardWidth, -boardWidth, boardHeight } };	// 7 �Ʒ��� �»��

GLfloat		redPosition[][2] = {
	{0, 0},
	{50, 50},
	{30, 30}

};

// �ʱ⼳��
void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); //���� ���� ����
	glEnable(GL_SMOOTH);  // ��Ƽ �˸��ƽ� ����
	glEnable(GL_LIGHTING); // ����

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL); // ���� ���� ǥ��

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

}


void DrawRedPlayer(int num) {

	glPushMatrix();
	// �����
	glTranslated(redPosition[num][0], redPosition[num][1], 0);
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GL_POLYGON);
	gluCylinder(qobj, playerRadius, playerRadius, height, 50, 50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	// �Ѳ�
	glTranslated(redPosition[num][0], redPosition[num][1], height);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
	{
		float angle = i * 3.141592 / 180;
		glVertex2f((cos(angle) * playerRadius), (sin(angle) * playerRadius));
	}
	glEnd();
	glPopMatrix();

}


void polygon(int a, int b, int c, int d) {
	glBegin(GL_POLYGON);
	glColor3f(0, 1, 0); // �׽�Ʈ��
	glVertex3fv(vertices[a]);
	glColor3f(0, 1, 1); // �׽�Ʈ��
	glVertex3fv(vertices[b]);
	glColor3f(0, 0, 1); // �׽�Ʈ��
	glVertex3fv(vertices[c]);
	glColor3f(1, 0, 0); // �׽�Ʈ��
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	eyeX, eyeY, eyeZ : ���� ��ġ(ī�޶��� ��ġ)
	centerX, centerY, centerZ : ī�޶��� ����(���� ��ġ)
	upX, upY, upZ : ī�޶��� ���ʺ��� ���� ����(x�� 1�̸� x������ �����ְ�, y�� 1�̸� y���� �߽����� �������ִ�.)
	*/
	gluLookAt(0.0, 0.0, cameraZoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// ȭ�� �׸���
void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraSetting();

	drawAxis();

	drawGameBoard();

	for (int i = 0; i < sizeof(redPosition) / (sizeof(int) * 2); i++)
		DrawRedPlayer(i);

	glFlush();
}

bool IsSelecting(int i)
{
	int corrWin = (winSize / 2); // ������ ����� ���� �߾� ������
	int corrRadius = playerRadius; // ī�޶� ��ġ�� ���� ������ ������

	if (redPosition[i][0] + corrRadius >= playerX - corrWin && redPosition[i][0] - corrRadius <= playerX - corrWin)
		if ((-1 * redPosition[i][1]) + corrRadius >= playerY - corrWin && (-1 * redPosition[i][1]) - corrRadius <= playerY - corrWin)
			return true;

	return false;
}

void mouseEvent(GLint Button, GLint State, GLint X, GLint Y)
{
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
	{
		// ��Ŭ���� �ٵϾ�                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   �� ��ǥ ����
		playerX = X;
		playerY = Y;
	}

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
	{
		selectingPlayer = -1;

		for (int i = 0; i < sizeof(redPosition) / (sizeof(int) * 2); i++)
		{
			if (IsSelecting(i))
				selectingPlayer = i;
		}

		if (selectingPlayer == -1)
		{
			cout << "����" << endl;//test
			cout << -1 * redPosition[0][0] << " " << -1 * redPosition[0][1] << endl;//test
			cout << playerX - (winSize / 2) << " " << playerY - (winSize / 2) << endl;//test
			return;

		}

		// ������ �Ÿ� ���
		movingX = abs(redPosition[selectingPlayer][0]) - abs(mouseX);
		movingY = abs(redPosition[selectingPlayer][1]) - abs(mouseY);
		cout << "��� " << selectingPlayer << endl;//test
		cout << "movingX " << movingX << endl; //test
		cout << "movingY " << movingY << endl; //test

		redPosition[selectingPlayer][0] += movingX / 10;
		redPosition[selectingPlayer][1] -= movingY / 10;

		glutPostRedisplay();

	}
}

void mouseMotion(GLint X, GLint Y)
{
	// ���콺�� ��ǥ
	mouseX = X;
	mouseY = Y;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(winSize, winSize);
	glutInitWindowSize(winSize, winSize);
	glutCreateWindow("�˱�� ����");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMouseFunc(mouseEvent);
	glutMotionFunc(mouseMotion);

	glutMainLoop();

	return 0;
}

