#include <gl/glut.h>
#include <math.h>  
#include <iostream>

using namespace std;

// 창의 크기
#define winSize		500

int corrWin = (winSize / 2); // 윈도우 사이즈에 따른 중앙 보정값

// 플레이어 설정
int		selectedPlayer = -1; // 플레이어가 선택 되었는지 판단
float	playerRadius = 20.0; // 플레이어의 반지름
float	height = 20.0; // 뚜껑의 높이

// 카메라 설정
float	cameraZoom = 610.0;

// 게임판 사이즈 설정
float	boardWidth = 250.0;
float	boardHeight = -5.0;

// 좌표들
float playerX = 0.0; // 클릭 좌표
float playerY = 0.0;
float mouseX = 0.0; // 마우스땐 좌표
float mouseY = 0.0;
float movingX = 0.0; // 계산된 움직일 위치
float movingY = 0.0;
float corrClickX = 0.0; // 보정된 클릭 좌표(가운데가 (0,0), 보정 전은 좌상단이 (0,0))
float corrClickY = 0.0;

GLUquadricObj* qobj; // 원기둥 오브젝트

GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f }; // 주변광
GLfloat diffuseLight[] = { 0.7f,0.7f,0.7f,1.0f }; // 분산광
GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f }; // 반사광
GLfloat position[] = { 400.0f,300.0f,700.0f,1.0f }; // 광원의 위치

GLfloat		vertices[][3] = {
	// x, y, z
	{ -boardWidth, -boardWidth,  0 },	// 0 윗쪽 좌하단
	{ -boardWidth,  boardWidth,  0 },	// 1 윗쪽 우하단
	{ boardWidth,  boardWidth,  0 },	// 2 윗쪽 우상단
	{ boardWidth, -boardWidth,  0 },	// 3 윗쪽 좌상단
	{ -boardWidth, -boardWidth, boardHeight },	// 4 아랫쪽 좌하단
	{ -boardWidth,  boardWidth, boardHeight },	// 5 아랫쪽 우하단
	{ boardWidth,  boardWidth, boardHeight },	// 6 아랫쪽 우상단
	{ boardWidth, -boardWidth, boardHeight } };	// 7 아랫쪽 좌상단

GLfloat		redPosition[][2] = {
	{20, 10},
	{50, 50},
	{100, 30}

};

// 초기설정
void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); //단위 법선 벡터
	glEnable(GL_SMOOTH);  // 안티 알리아싱 제거
	glEnable(GL_LIGHTING); // 조명

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL); // 재질 색감 표현

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

}


void DrawRedPlayer(int num) {

	glPushMatrix();
	// 원기둥
	glTranslated(redPosition[num][0], redPosition[num][1], 0);
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GL_POLYGON);
	gluCylinder(qobj, playerRadius, playerRadius, height, 50, 50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	// 뚜껑
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
	glColor3f(0, 1, 0); // 테스트용
	glVertex3fv(vertices[a]);
	glColor3f(0, 1, 1); // 테스트용
	glVertex3fv(vertices[b]);
	glColor3f(0, 0, 1); // 테스트용
	glVertex3fv(vertices[c]);
	glColor3f(1, 0, 0); // 테스트용
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

void DrawAimingLine(GLint X, GLint Y)
{
	GLfloat targetX, targetY;

	cout << "작동중" << endl;


	targetX = redPosition[selectedPlayer][0] + ((abs(playerX) - abs(X)) / 2);
	targetY = redPosition[selectedPlayer][1] - ((abs(playerY) - abs(Y)) / 2);

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); // red color

	glVertex3f(redPosition[selectedPlayer][0], redPosition[selectedPlayer][1], 0.0);
	glVertex3f(targetX, targetY, 0.0);
	glEnd();
	glFlush();

}

// 화면의 비율 유지
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 1000);
}

// 카메라 위치 설정
void cameraSetting(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	eyeX, eyeY, eyeZ : 눈의 위치(카메라의 위치)
	centerX, centerY, centerZ : 카메라의 초점(참조 위치)
	upX, upY, upZ : 카메라의 위쪽벡터 방향 지정(x가 1이면 x축으로 누워있고, y가 1이면 y축을 중심으로 세워져있다.)
	*/
	gluLookAt(0.0, 0.0, cameraZoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// 화면 그리기
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

bool IsSelected(int i)
{
	int corrRadius = playerRadius; // 카메라 위치에 따른 반지름 보정값

	if (redPosition[i][0] + corrRadius >= corrClickX && redPosition[i][0] - corrRadius <= corrClickX)
		if ((-1 * redPosition[i][1]) + corrRadius >= corrClickY && (-1 * redPosition[i][1]) - corrRadius <= corrClickY)
			return true;

	return false;
}

void mouseEvent(GLint Button, GLint State, GLint X, GLint Y)
{

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
	{
		selectedPlayer = -1; // 예외처리 조건 초기화

		// 마우스 누른 위치                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                의 좌표 저장
		playerX = X;
		playerY = Y;

		// 보정된 마우스 클릭 위치
		corrClickX = X - corrWin;
		corrClickY = Y - corrWin;

		// 플레이어를 선택 하였는가?
		for (int i = 0; i < sizeof(redPosition) / (sizeof(int) * 2); i++)
		{
			if (IsSelected(i))
				selectedPlayer = i;
		}
	}

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
	{
		// 예외처리
		if (selectedPlayer == -1)
			return;

		// 마우스 땐 위치
		mouseX = X;
		mouseY = Y;

		// 움직일 거리 계산 (단위벡터)
		movingX = playerX - mouseX;
		movingY = playerY - mouseY;

		// 계산된 거리만큼 플레이어 이동
		redPosition[selectedPlayer][0] += movingX;
		redPosition[selectedPlayer][1] -= movingY;

		glutPostRedisplay();
		selectedPlayer = -1;		// 예외조건 초기화

	}
}

void mouseMotion(GLint X, GLint Y)
{
	// 마우스의 좌표
	mouseX = X - corrWin;
	mouseY = Y - corrWin;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(winSize, winSize);
	glutInitWindowSize(winSize, winSize);
	glutCreateWindow("알까기 게임");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMouseFunc(mouseEvent);
	glutMotionFunc(mouseMotion);

	glutMainLoop();

	return 0;
}

