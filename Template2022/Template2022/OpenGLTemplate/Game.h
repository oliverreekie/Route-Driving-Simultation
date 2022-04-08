#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class CDiamond;
class CMinecart;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh* m_pRockMesh;
	COpenAssetImportMesh* m_pAxeMesh;
	COpenAssetImportMesh* m_pHatMesh;
	CDiamond* m_pDiamond;
	CMinecart* m_pMinecart;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom* m_pCatmullRom;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;

	float m_currentDistance = 0.0f;

	glm::vec3 diaPos = glm::vec3(30.0f, 8.0f, 100.0f);

	glm::mat4 playerRotation;

	//Holds position on track
	//0 = center, 1 = left, 2 = right
	int trackPos = 0;

	//Holds the camera view
	//1 = first person, 2 = 3rd person, 3 = arial
	int cameraView = 1;

	//Rock Positions
	glm::vec3 rockArray[9];

	//Diamond Positions
	glm::vec3 diamondArray[7];

	//Hat Positions
	glm::vec3 hatArray[4];

	//Axe Positions
	glm::vec3 axeArray[5];

	int diamondTrackPos[7];

	int rockTrackPos[9];


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	int score = 0;
	int speed = 90;
	float speedCounter = 0;

	void RenderRock(float f, int trackPos, int whichRock);
	void RenderDiamond(float f, int trackPos, int whichDiamond);
	void RenderHat(float f, int trackPos, int whichHat);
	void RenderAxe(float f, int trackPos, int whichAxe);

	void CollisionCheck(glm::vec3 playerPos);

	void rotateAroundPoint();

};
