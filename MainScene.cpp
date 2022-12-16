#define _CRT_SECURE_NO_WARNINGS
#include <gl/glut.h>
#include <math.h>  
#include <iostream>
#include <mmsystem.h>    
#pragma comment(lib,"winmm")

#define SOUND_FILE_NAME_BGM "bgm.wav"
#define SOUND_FILE_NAME_WIN "win.wav"

using namespace std;

#define winSize		500		// 창의 크기
#define playerNum	10		// 플레이어 수

int corrWin = (winSize / 2); // 윈도우 사이즈에 따른 중앙 보정값

// 남은 돌
int aliveRed, aliveGreen;

// 현재 턴
char currentTurn = 'r';

// 플레이어 설정
int		selectedPlayer = -1; // 플레이어가 선택 되었는지 판단
float	playerRadius = 20.0; // 플레이어의 반지름
float	height = 20.0;		 // 뚜껑의 높이
int		speedControl = 80;  	 // 속도 제어값

// 장애물 설정
//const int obstacleNum = 1; // 장애물 갯수
//float obstaclePos[obstacleNum] = {0}; // 장애물 X좌표
int cubeSize = 40; // 장애물 크기

// 카메라 설정
float	cameraZoom = 610.0;
int		cameraMode = 1;
int		cameraCount = 0; // 카메라 지속시간
int		cameraObject; // 카메라가 따라갈 플레이어

// 게임판 사이즈 설정
float	boardWidth = 230.0;
float	boardHeight = -100.0;

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

GLfloat AmbientLightValue[] = { 0.3f, 0.3f, 5.3f, 1.0f };
GLfloat DiffuseLightValue[] = { 0.7f, 0.7f, 5.7f, 1.0f };
GLfloat SpecularLightValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat PositionLightValue[] = { 0.0, 300.0, 0.0, 0.0};

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

// 텍스쳐 매핑
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info);
GLubyte* pBytes; // 데이터를 가리킬 포인터
GLubyte* pBytes_2; // 데이터를 가리킬 포인터
BITMAPINFO* info; // 비트맵 헤더 저장할 변수
GLuint textures[2];
void MappingInit();
// 텍스쳐 매핑

void RunPhysics();
bool IsCollision(int j, int k);

void MainLight()
{
	glClearColor(0, 0, 0, 0);
	glEnable(GL_LIGHTING); //조명을 사용할 것이다.
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLightValue); //Ambient 조명의 성질을 설정한다.
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLightValue); //Diffuse 조명의 성질을 설정한다.
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLightValue); //Specular 조명의 성질을 설정한다. 
	glLightfv(GL_LIGHT0, GL_POSITION, PositionLightValue); //조명의 위치(광원)를 설정한다.
	glEnable(GL_LIGHT0); //조명 중 0 번 조명을 사용할 것이다.

}

GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info)
{
	FILE* fp;
	GLubyte* bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;
	// 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;
	// 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}
	// 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}
	// BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	// 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO*)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}
	// 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}
	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
			(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);
	// 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (unsigned char*)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}
	// 비트맵 데이터를 bit(GLubyte 타입)에 저장한다.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}

void MappingInit()
{
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("board.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info->bmiHeader.biWidth, info->bmiHeader.biHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes_2 = LoadDIBitmap("obstacle.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info->bmiHeader.biWidth, info->bmiHeader.biHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
}

// 초기설정
void init(void)
{
	PlaySound(TEXT(SOUND_FILE_NAME_BGM), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);


	player[0].x = -160.0;		player[0].y = -180.0;		player[0].teamColor = 'r';
	player[1].x = -80;			player[1].y = -180.0;		player[1].teamColor = 'r';
	player[2].x = 0.0;			player[2].y = -180.0;		player[2].teamColor = 'r';
	player[3].x = 80.0;			player[3].y = -180.0;		player[3].teamColor = 'r';
	player[4].x = 160.0;;		player[4].y = -180.0;		player[4].teamColor = 'r';
	
	player[5].x = -160.0;		player[5].y = 180.0;		player[5].teamColor = 'g';
	player[6].x = -80;			player[6].y = 180.0;		player[6].teamColor = 'g';
	player[7].x = 0.0;			player[7].y = 180.0;		player[7].teamColor = 'g';
	player[8].x = 80.0;			player[8].y = 180.0;		player[8].teamColor = 'g';
	player[9].x = 160.0;;		player[9].y = 180.0;		player[9].teamColor = 'g';

	aliveRed = playerNum / 2;
	aliveGreen = playerNum / 2;

	MappingInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); //단위 법선 벡터
	glEnable(GL_SMOOTH);  // 안티 알리아싱 제거

	glEnable(GL_COLOR_MATERIAL); // 재질 색감 표현
	glEnable(GL_TEXTURE_2D);

	MainLight();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

}

void DrawObstacle()
{
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glColor3f(1, 1, 1);

	glTranslatef(0.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-cubeSize, cubeSize, cubeSize); // { Front }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-cubeSize, -cubeSize, cubeSize); // { Front }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(cubeSize, -cubeSize, cubeSize); // { Front }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(cubeSize, cubeSize, cubeSize); // { Front }

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(cubeSize, cubeSize, cubeSize); // { Right }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(cubeSize, -cubeSize, cubeSize); // { Right }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(cubeSize, -cubeSize, -cubeSize); // { Right }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(cubeSize, cubeSize, -cubeSize); // { Right }

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(cubeSize, cubeSize, -cubeSize); // { Back }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(cubeSize, -cubeSize, -cubeSize); // { Back }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-cubeSize, -cubeSize, -cubeSize); // { Back }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-cubeSize, cubeSize, -cubeSize); // { Back }

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-cubeSize, cubeSize, -cubeSize); // { Left }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-cubeSize, -cubeSize, -cubeSize); // { Left }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-cubeSize, -cubeSize, cubeSize); // { Left }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-cubeSize, cubeSize, cubeSize); // { Left }

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-cubeSize, cubeSize, cubeSize); // { Top }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(cubeSize, cubeSize, cubeSize); // { Top }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(cubeSize, cubeSize, -cubeSize); // { Top }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-cubeSize, cubeSize, -cubeSize); // { Top }

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(cubeSize, -cubeSize, cubeSize); // { Bottom }
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-cubeSize, -cubeSize, cubeSize); // { Bottom }
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-cubeSize, -cubeSize, -cubeSize); // { Bottom }
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(cubeSize, -cubeSize, -cubeSize); // { Bottom }
	glEnd();
	glPopMatrix();
}

void DrawPlayer(int num) {

	if (player[num].teamColor == 'r')		glColor3f(1.0, 0.0, 0.0); // red color
	else if (player[num].teamColor == 'g') 	glColor3f(0.0, 1.0, 0.0); // green color

	glPushMatrix();
	// 원기둥
	glTranslated(player[num].x, player[num].y, 0);
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GL_POLYGON);

	gluCylinder(qobj, playerRadius, playerRadius, height, 50, 50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	// 뚜껑
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
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glColor3f(1, 1, 1);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vertices[a]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vertices[b]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vertices[c]);
	glTexCoord2f(0.0f, 0.0f);
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

	if (cameraMode == 1)
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

	else if (cameraMode == 2)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		/*
		eyeX, eyeY, eyeZ : 눈의 위치(카메라의 위치)
		centerX, centerY, centerZ : 카메라의 초점(참조 위치)
		upX, upY, upZ : 카메라의 위쪽벡터 방향 지정(x가 1이면 x축으로 누워있고, y가 1이면 y축을 중심으로 세워져있다.)
		*/
		gluLookAt(player[cameraObject].x, player[cameraObject].y - 500, 400, player[cameraObject].x, player[cameraObject].y, 0.0, 0.0, 1.0, 0.0);
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
	if (cameraCount != 0)	return; // 카메라 이동중에는 표시 X

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

	else if (currentTurn == 'g')
	{
		glColor3f(0.0, 1.0, 0.0);
		sprintf_s(buf, "Green");
		renderbitmap(-70, -220, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}

	// 승리 판정
	if (aliveGreen == 0)
	{
		PlaySound(TEXT(SOUND_FILE_NAME_WIN), NULL, SND_FILENAME | SND_ASYNC);
		aliveGreen = -1;
	}

	else if (aliveRed == 0)
	{
		PlaySound(TEXT(SOUND_FILE_NAME_WIN), NULL, SND_FILENAME | SND_ASYNC);
		aliveRed = -1;
	}

	if (aliveGreen == -1)
	{
			glColor3f(1.0, 0.0, 0.0);
			sprintf_s(buf, "RED Win");
			renderbitmap(0, 100, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}
	else if (aliveRed == -1)
	{
			glColor3f(0.0, 1.0, 0.0);
			sprintf_s(buf, "Green Win");
			renderbitmap(0, 100, GLUT_BITMAP_TIMES_ROMAN_24, buf);
	}
}

// 화면 그리기
void display(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraSetting();

	DrawText();
	drawGameBoard();
	DrawObstacle();

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
		else if (player[i].teamColor == 'g') aliveGreen -= 1;
	}

	else if ((player[i].y < -boardWidth || player[i].y > boardWidth) && (player[i].alive == true))
	{
		player[i].y *= 10;
		player[i].alive = false;
		if (player[i].teamColor == 'r') aliveRed -= 1;
		else if (player[i].teamColor == 'g') aliveGreen -= 1;
	}


}

void RunPhysics()
{
	for (int i = 0; i < playerNum; i++)
	{
		// 속도가 있으면 이동
		if (player[i].velocity > 0)
		{
			player[i].x += (player[i].dirX / speedControl) * (player[i].velocity / speedControl);
			player[i].y -= (player[i].dirY / speedControl) * (player[i].velocity / speedControl);
		}

		// 장애물 충돌 판정
		if ((player[i].x >= 0-cubeSize) && (player[i].x <= 0+cubeSize))
			if ((player[i].y >= 0-cubeSize) && (player[i].y <= 0+cubeSize))
			{
				player[i].x -= (player[i].dirX / speedControl) * (player[i].velocity / speedControl);
				player[i].y += (player[i].dirY / speedControl) * (player[i].velocity / speedControl);
				player[i].dirX *= -1;
				player[i].dirY *= -1;
				player[i].velocity = player[i].velocity / 1.5;
			}

		// 충돌 판정
		for (int j = 0; j < playerNum; j++)
		{
			if (IsCollision(i, j))
			{
				player[j].velocity = player[i].velocity;
				player[j].dirX = player[i].dirX;
				player[j].dirY = player[i].dirY;

				player[i].x -= (player[i].dirX / speedControl) * (player[i].velocity / speedControl);
				player[i].y += (player[i].dirY / speedControl) * (player[i].velocity / speedControl);
				player[i].velocity = player[i].velocity / 2;

			}
		}

		player[i].velocity -= 0.5;

		if (player[i].velocity < 0.0)
			player[i].velocity = 0.0;

		OutCheck(i); // 플레이어가 경기장 밖으로 나갔는가?

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
		selectedPlayer = -1; // 예외처리 조건 초기화

		// 마우스 누른 위치                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                의 좌표 저장
		playerX = X;
		playerY = Y;

		// 보정된 마우스 클릭 위치
		corrClickX = X - corrWin;
		corrClickY = Y - corrWin;

		// 플레이어를 선택 하였는가?
		for (int i = 0; i < playerNum; i++)
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
		if (currentTurn != player[selectedPlayer].teamColor)
			return;

		// 마우스 땐 위치
		mouseX = X;
		mouseY = Y;

		// 움직일 거리 계산 (단위벡터)
		movingX = playerX - mouseX;
		movingY = playerY - mouseY;

		// 이동방향 설정
		player[selectedPlayer].dirX = movingX;
		player[selectedPlayer].dirY = movingY;
		// 속도 설정
		float squareDirX, squareDirY;
		squareDirX = player[selectedPlayer].dirX * player[selectedPlayer].dirX;
		squareDirY = player[selectedPlayer].dirY * player[selectedPlayer].dirY;

		player[selectedPlayer].velocity = sqrt(squareDirX + squareDirY);
		if (player[selectedPlayer].velocity > 120.0)	player[selectedPlayer].velocity = 120.0;

		glutPostRedisplay();
		cameraObject = selectedPlayer;
		selectedPlayer = -1;		// 예외조건 초기화
		glutTimerFunc(20, CameraTimer, 1);	// 카메라 줌

		// 턴 넘김

		if (currentTurn == 'r')		currentTurn = 'g';
		else if (currentTurn == 'g')		currentTurn = 'r';

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

