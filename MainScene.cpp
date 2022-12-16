#include <gl/glut.h>
#include <math.h>  
#include <iostream>

using namespace std;


#define winSize		500		// â�� ũ��
#define playerNum	10		// �÷��̾� ��

int corrWin = (winSize / 2); // ������ ����� ���� �߾� ������

// ���� ��
int aliveRed, aliveBlue;

// ���� ��
char currentTurn = 'r';

// �÷��̾� ����
int		selectedPlayer = -1; // �÷��̾ ���� �Ǿ����� �Ǵ�
float	playerRadius = 20.0; // �÷��̾��� ������
float	height = 20.0;		 // �Ѳ��� ����
int		speedControl = 80;  	 // �ӵ� ���

// ī�޶� ����
float	cameraZoom = 610.0;
int		cameraMode = 1;
int		cameraCount = 0;

// ������ ������ ����
float	boardWidth = 230.0;
float	boardHeight = -100.0;

// ��ǥ��
float playerX = 0.0; // Ŭ�� ��ǥ
float playerY = 0.0;
float mouseX = 0.0; // ���콺�� ��ǥ
float mouseY = 0.0;
float movingX = 0.0; // ���� ������ ��ġ
float movingY = 0.0;
float corrClickX = 0.0; // ������ Ŭ�� ��ǥ(����� (0,0), ���� ���� �»���� (0,0))
float corrClickY = 0.0;

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

class Player
{
public:
	GLfloat x, y;
	GLfloat dirX = 0.0, dirY = 0.0;
	GLfloat velocity;
	bool alive = true;
	char teamColor;
};

Player* player = new Player[playerNum];

void RunPhysics();
bool IsCollision(int j, int k);

// �ʱ⼳��
void init(void)
{
	player[0].x = -160.0;		player[0].y = -180.0;		player[0].teamColor = 'r';
	player[1].x = -80;			player[1].y = -180.0;		player[1].teamColor = 'r';
	player[2].x = 0.0;			player[2].y = -180.0;		player[2].teamColor = 'r';
	player[3].x = 80.0;			player[3].y = -180.0;		player[3].teamColor = 'r';
	player[4].x = 160.0;;		player[4].y = -180.0;		player[4].teamColor = 'r';

	player[5].x = -160.0;		player[5].y = 180.0;		player[5].teamColor = 'b';
	player[6].x = -80;			player[6].y = 180.0;		player[6].teamColor = 'b';
	player[7].x = 0.0;			player[7].y = 180.0;		player[7].teamColor = 'b';
	player[8].x = 80.0;			player[8].y = 180.0;		player[8].teamColor = 'b';
	player[9].x = 160.0;;		player[9].y = 180.0;		player[9].teamColor = 'b';

	aliveRed = playerNum / 2;
	aliveBlue = playerNum / 2;

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


void DrawPlayer(int num) {
	if (player[num].teamColor == 'r')		glColor3f(1.0, 0.0, 0.0); // red color
	else if (player[num].teamColor == 'b') 	glColor3f(0.0, 0.0, 1.0); // blue color

	glPushMatrix();
	// �����
	glTranslated(player[num].x, player[num].y, 0);
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GL_POLYGON);

	gluCylinder(qobj, playerRadius, playerRadius, height, 50, 50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	// �Ѳ�
	glTranslated(player[num].x, player[num].y, height);
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

	if (cameraMode == 1) 
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
	
	else if (cameraMode == 2)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		/*
		eyeX, eyeY, eyeZ : ���� ��ġ(ī�޶��� ��ġ)
		centerX, centerY, centerZ : ī�޶��� ����(���� ��ġ)
		upX, upY, upZ : ī�޶��� ���ʺ��� ���� ����(x�� 1�̸� x������ �����ְ�, y�� 1�̸� y���� �߽����� �������ִ�.)
		*/
		gluLookAt(0.0, -660.0, 400, player[selectedPlayer].x, player[selectedPlayer].y, 0.0, 0.0, 1.0, 0.0);
	}

}

void renderbitmap(float x, float y, void* font, char* string) {
	char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void DrawText(void)
{
	char buf[100] = { 0 };
	glColor3f(0.0, 0.0, 0.0);
	sprintf_s(buf, "Current Turn :");
	renderbitmap(-220, -220, GLUT_BITMAP_TIMES_ROMAN_24, buf);

	if (currentTurn == 'r')
	{
		glColor3f(1.0, 0.0, 0.0);
		sprintf_s(buf, "Red");
		renderbitmap(-70, -220, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}

	else if (currentTurn == 'b')
	{
		glColor3f(0.0, 0.0, 1.0);
		sprintf_s(buf, "Blue");
		renderbitmap(-70, -220, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}

	// �¸� ����
	if (aliveBlue == 0)
	{
		glColor3f(1.0, 0.0, 0.0);
		sprintf_s(buf, "RED Win");
		renderbitmap(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}
	else if (aliveRed == 0)
	{
		glColor3f(0.0, 0.0, 1.0);
		sprintf_s(buf, "Blue Win");
		renderbitmap(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}

}

// ȭ�� �׸���
void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraSetting();

	DrawText();
	drawGameBoard();

	for (int i = 0; i < playerNum; i++)
		DrawPlayer(i);

	RunPhysics();

	glFlush();
}

void OutCheck(int i)
{
	if ((player[i].x < -boardWidth || player[i].x > boardWidth) && (player[i].alive == true))
	{
		player[i].x *= 10;
		player[i].alive = false;
		if (player[i].teamColor == 'r') aliveRed -= 1;
		else if (player[i].teamColor == 'b') aliveBlue -= 1;
	}

	else if ((player[i].y < -boardWidth || player[i].y > boardWidth) && (player[i].alive == true))
	{
		player[i].y *= 10;
		player[i].alive = false;
		if (player[i].teamColor == 'r') aliveRed -= 1;
		else if (player[i].teamColor == 'b') aliveBlue -= 1;
	}


}

void RunPhysics()
{
	for (int i = 0; i < playerNum; i++)
	{
		if (player[i].velocity > 0)
		{
			player[i].x += (player[i].dirX / speedControl) * (player[i].velocity / speedControl);
			player[i].y -= (player[i].dirY / speedControl) * (player[i].velocity / speedControl);
		}



		for (int j = 0; j < playerNum; j++)
		{
			if (IsCollision(i, j))
			{
				player[j].velocity = player[i].velocity;
				player[j].dirX = player[i].dirX;
				player[j].dirY = player[i].dirY;

				player[i].x -= (player[i].dirX / speedControl) * (player[i].velocity / speedControl);
				player[i].y += (player[i].dirY / speedControl) * (player[i].velocity / speedControl);
				player[i].velocity = player[i].velocity/2;
			}
		}
		
		player[i].velocity -= 0.5;

		if (player[i].velocity < 0.0)
			player[i].velocity = 0.0;

		OutCheck(i); // �÷��̾ ����� ������ �����°�?

		glutPostRedisplay();

	}
}

bool IsCollision(int i, int j)
{
	float xSquare, ySquare, radiusSquare;

	if (i == j)	return false;

	xSquare = (player[i].x - player[j].x) * (player[i].x - player[j].x);
	ySquare = (player[i].y - player[j].y) * (player[i].y - player[j].y);
	radiusSquare = (playerRadius + playerRadius) * (playerRadius + playerRadius);

	if (xSquare + ySquare < radiusSquare)
			return true;

	return false;
}

bool IsSelected(int i)
{
	if (player[i].x + playerRadius >= corrClickX && player[i].x - playerRadius <= corrClickX)
		if ((-1 * player[i].y) + playerRadius >= corrClickY && (-1 * player[i].y) - playerRadius <= corrClickY)
			return true;

	return false;
}

void CameraTimer(int value) {
	cameraMode = 2;

	cameraCount++;

	if (cameraCount > 30)
	{
		cameraCount = 0;
		cameraMode = 1;
		return;
	}

	glutPostRedisplay();
	glutTimerFunc(40, CameraTimer, 1);
}

void mouseEvent(GLint Button, GLint State, GLint X, GLint Y)
{

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
	{
		selectedPlayer = -1; // ����ó�� ���� �ʱ�ȭ

		// ���콺 ���� ��ġ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                �� ��ǥ ����
		playerX = X;
		playerY = Y;

		// ������ ���콺 Ŭ�� ��ġ
		corrClickX = X - corrWin;
		corrClickY = Y - corrWin;

		// �÷��̾ ���� �Ͽ��°�?
		for (int i = 0; i < playerNum; i++)
		{
			if (IsSelected(i))
				selectedPlayer = i;
		}

	}

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
	{
		// ����ó��
		if (selectedPlayer == -1)
			return;
		if (currentTurn != player[selectedPlayer].teamColor)
			return;

		// ���콺 �� ��ġ
		mouseX = X;
		mouseY = Y;

		// ������ �Ÿ� ��� (��������)
		movingX = playerX - mouseX;
		movingY = playerY - mouseY;

		// �̵����� ����
		player[selectedPlayer].dirX = movingX;
		player[selectedPlayer].dirY = movingY;
		// �ӵ� ����
		float squareDirX, squareDirY;
		squareDirX = player[selectedPlayer].dirX * player[selectedPlayer].dirX;
		squareDirY = player[selectedPlayer].dirY * player[selectedPlayer].dirY;
	
		player[selectedPlayer].velocity = sqrt(squareDirX + squareDirY);
		if (player[selectedPlayer].velocity > 120.0)	player[selectedPlayer].velocity = 120.0;

		glutPostRedisplay();
		selectedPlayer = -1;		// �������� �ʱ�ȭ
		glutTimerFunc(40, CameraTimer, 1);	// ī�޶� ��

		// �� �ѱ�

		if (currentTurn == 'r')		currentTurn = 'b';
		else if (currentTurn == 'b')		currentTurn = 'r';

	}
}

void mouseMotion(GLint X, GLint Y)
{
	// ���콺�� ��ǥ
	mouseX = X - corrWin;
	mouseY = Y - corrWin;
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

