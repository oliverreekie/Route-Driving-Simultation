/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Diamond.h"
#include "Minecart.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include <iostream>
#include <Array>

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pRockMesh = NULL;
	m_pAxeMesh = NULL;
	m_pHatMesh = NULL;
	m_pDiamond = NULL;
	m_pMinecart = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pRockMesh;
	delete m_pAxeMesh;
	delete m_pHatMesh;
	delete m_pDiamond;
	delete m_pMinecart;
	delete m_pAudio;
	delete m_pCatmullRom;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pRockMesh = new COpenAssetImportMesh;
	m_pAxeMesh = new COpenAssetImportMesh;
	m_pHatMesh = new COpenAssetImportMesh;
	m_pDiamond = new CDiamond;
	m_pMinecart = new CMinecart;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("diamondShader.vert");
	sShaderFileNames.push_back("diamondShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	//Shader for diamond
	CShaderProgram* pDiamondProgram = new CShaderProgram;
	pDiamondProgram->CreateProgram();
	pDiamondProgram->AddShaderToProgram(&shShaders[4]);
	pDiamondProgram->AddShaderToProgram(&shShaders[5]);
	pDiamondProgram->LinkProgram();
	m_pShaderPrograms->push_back(pDiamondProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pRockMesh->Load("resources\\models\\Rock\\rock2.obj");  // Downloaded from http://www.cgtrader.com/items/2048257/download-page on 16 Mar 2022
	m_pAxeMesh->Load("resources\\models\\Axe\\theAxe.obj");  // Downloaded from https://www.cgtrader.com/items/2676342/download-page on 17 Mar 2022
	m_pHatMesh->Load("resources\\models\\Hat\\hat.obj");  // Downloaded from https://www.cgtrader.com/items/3158323/download-page on 17 Mar 2022

	// Create a diamond
	m_pDiamond->Create("resources\\textures\\", "magic.png");  // Texture downloaded from https://www.nicepng.com/ourpic/u2t4u2a9q8o0o0y3_download-magic-light-effect-png/ on 9 Mar 2022
	glEnable(GL_CULL_FACE);

	// Create a minecart
	m_pMinecart->CreateBody();  // Texture downloaded from https://www.nicepng.com/ourpic/u2t4u2a9q8o0o0y3_download-magic-light-effect-png/ on 9 Mar 2022
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack();

	RenderRock(5, 1, 0);
	rockTrackPos[0] = 1;
	RenderRock(40, 0, 1);
	rockTrackPos[1] = 0;
	RenderRock(100, 2, 2);
	rockTrackPos[2] = 2;
	RenderRock(150, 1, 3);
	rockTrackPos[3] = 1;
	RenderRock(190, 2, 4);
	rockTrackPos[4] = 2;
	RenderRock(320, 1, 5);
	rockTrackPos[5] = 1;
	RenderRock(720, 0, 6);
	rockTrackPos[6] = 0;
	RenderRock(840, 2, 7);
	rockTrackPos[7] = 2;
	RenderRock(900, 1, 8);
	rockTrackPos[8] = 1;
	RenderRock(1000, 2, 9);
	rockTrackPos[9] = 2;


	RenderDiamond(60, 2, 0);
	diamondTrackPos[0] = 2;
	RenderDiamond(240, 0, 1);
	diamondTrackPos[1] = 0;
	RenderDiamond(810, 1, 2);
	diamondTrackPos[2] = 1;
	RenderDiamond(360, 1, 3);
	diamondTrackPos[3] = 1;
	RenderDiamond(750, 2, 4);
	diamondTrackPos[4] = 2;
	RenderDiamond(940, 0, 5);
	diamondTrackPos[5] = 0;
	RenderDiamond(1100, 2, 6);
	diamondTrackPos[6] = 2;


	RenderHat(200, 1, 0);
	RenderHat(400, 2, 1);
	RenderHat(600, 2, 2);
	RenderHat(800, 1, 3);
	RenderHat(1050, 2, 4);

	RenderAxe(100, 2, 0);
	RenderAxe(300, 1, 1);
	RenderAxe(500, 2, 2);
	RenderAxe(700, 1, 3);
	RenderAxe(900, 1, 4);

	pause = false;

	lightColour = 1.0;

}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(lightColour));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	pMainProgram->SetUniform("light1.exponent", 20.0f);
	pMainProgram->SetUniform("light1.cutoff", 30.0f);

	pMainProgram->SetUniform("light2.position", viewMatrix * glm::vec4(0.f, 10.f, 0.f, 1.f));
	pMainProgram->SetUniform("light2.La", glm::vec3(1.0f, 1.0f, 0.0f));
	pMainProgram->SetUniform("light2.Ld", glm::vec3(1.0f));
	pMainProgram->SetUniform("light2.Ls", glm::vec3(1.0f));
	pMainProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	pMainProgram->SetUniform("light2.exponent", 20.0f);
	pMainProgram->SetUniform("light2.cutoff", 30.0f);

	pMainProgram->SetUniform("light3.position", viewMatrix * glm::vec4(0.f, 10.f, 0.f, 1.f));
	pMainProgram->SetUniform("light3.La", glm::vec3(1.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("light3.Ld", glm::vec3(1.0f));
	pMainProgram->SetUniform("light3.Ls", glm::vec3(1.0f));
	pMainProgram->SetUniform("light3.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	pMainProgram->SetUniform("light3.exponent", 20.0f * 0.1f);
	pMainProgram->SetUniform("light3.cutoff", 30.0f * 0.1f);

	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property



	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();

	m_pCatmullRom->RenderCentreline();
	m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderTrack();

	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	// Rendering the rocks
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[0]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[1]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[2]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[3]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[4]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[5]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[6]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[7]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[8]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(rockArray[9]);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(0.1f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pRockMesh->Render();
		modelViewMatrixStack.Pop();

	// Render the axe 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(axeArray[0]);
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pAxeMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(axeArray[1]);
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pAxeMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(axeArray[2]);
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pAxeMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(axeArray[3]);
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pAxeMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(axeArray[4]);
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pAxeMesh->Render();
		modelViewMatrixStack.Pop();

	// Render the hat 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(hatArray[0]);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHatMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(hatArray[1]);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHatMesh->Render();
		modelViewMatrixStack.Pop();
	
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(hatArray[2]);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHatMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(hatArray[3]);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHatMesh->Render();
		modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(hatArray[4]);
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHatMesh->Render();
		modelViewMatrixStack.Pop();


	//Render the minecart
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(diaPos);
		modelViewMatrixStack.ApplyMatrix(playerRotation);
		modelViewMatrixStack.Scale(0.45f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pMinecart->Render();
	modelViewMatrixStack.Pop();

	//Setting diamond shader
	CShaderProgram* pDiamondProgram = (*m_pShaderPrograms)[2];
	pDiamondProgram->UseProgram();
	pDiamondProgram->SetUniform("bUseTexture", true);
	pDiamondProgram->SetUniform("sampler0", 0);
	pDiamondProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

	// Set the projection matrix
	pDiamondProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Set light and materials in main shader program
	pDiamondProgram->SetUniform("light1.position", viewMatrix* lightPosition1); // Position of light source *in eye coordinates*
	pDiamondProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pDiamondProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pDiamondProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pDiamondProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pDiamondProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pDiamondProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pDiamondProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	// Render the Diamond
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[0]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[1]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[2]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[3]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[4]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[5]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(diamondArray[6]);
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pDiamond->Render();
	modelViewMatrixStack.Pop();
		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());	

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	////Uses trigonometry to move the camera in a circle around the diamond - diamond positon at glm::vec3(30.0f, 8.0f, 100.0f)
	//rotateAroundPoint();

	//Sets the distance along the line
	m_currentDistance += m_dt * 0.0005f * speed;
	glm::vec3 p;
	m_pCatmullRom->Sample(m_currentDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance + 1.f, pNext);

	glm::vec3 pPrevious;
	m_pCatmullRom->Sample(m_currentDistance - 1.f, pPrevious);

	glm::vec3 tangent = glm::normalize(pNext - p);
	glm::vec3 lookingAt = glm::vec3(p + 1.0f * tangent);
	lookingAt.y = lookingAt.y + 5.0f;

	glm::vec3 behind = glm::normalize(pPrevious - p);
	glm::vec3 firstPerson = glm::vec3(p + 8.0f * behind);
	firstPerson.y = firstPerson.y + 6.0f;


	//Setting third person view
	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);
	glm::vec3 vectorAlongX = glm::vec3(p + 14.0f * crossOfXAxis);
	vectorAlongX.y += 5.0f;

	if (pause == false) {
		//Calculates rotation of cart around the track
		playerRotation = glm::inverse(glm::lookAt(glm::vec3(0, 0, 0), tangent, glm::vec3(0, 1, 0)));

		//Set cart position
		if (trackPos == 0) {
			diaPos = glm::vec3(p.x, p.y + 1.3f, p.z);
		}
		else if (trackPos == 1) {
			glm::vec3 leftPosition = glm::vec3(p + 2.2f * crossOfXAxis);
			diaPos = glm::vec3(leftPosition.x, leftPosition.y + 1.3f, leftPosition.z);
		}
		else if (trackPos == 2) {
			glm::vec3 rightPosition = glm::vec3(p + 2.2f * -crossOfXAxis);
			diaPos = glm::vec3(rightPosition.x, rightPosition.y + 1.3f, rightPosition.z);
		}
	}

	//Setting the camera view
	if (cameraView == 1) {
		m_pCamera->Set(firstPerson, lookingAt, glm::vec3(0, 1, 0));
	}
	else if (cameraView == 2) {
		m_pCamera->Set(vectorAlongX, diaPos, glm::vec3(0, 1, 0));
	}
	else if (cameraView == 3) {
		m_pCamera->Set(glm::vec3(0, 400, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
	}
	else if (cameraView == 4) {
		m_pCamera->Update(m_dt);
	}

	//Reduce speed every second
	if (pause == false) {
		if (speed > 0) {

			speedCounter += m_dt;
			if (speedCounter >= 1000) {
				speedCounter -= 1000;
				speed -= 0.5;
			}
		}
	}

	//Make sure speed doesn't turn negative
	if (speed <= 0) {
		speed = 0;
	}

	//Reduce light at end of game
	if (speed == 0) {
		lightColour = 0.1;
	}

	//Checks collision of player with rocks and diamond
	CollisionCheck(diaPos);

	m_pAudio->Update();
}

//Function to check collisions with diamonds and rocks
void Game::CollisionCheck(glm::vec3 playerPos) {
	//Check diamonds
	for (int i = 0; i < 7; i++) {
		if (diamondTrackPos[i] == trackPos) {
			if (glm::distance(diamondArray[i], diaPos) <= 3.0f) {
				score++;
				int newPos = rand() % 3;
				diamondArray[i] = MoveObject((rand() % 1500), newPos);
				diamondTrackPos[i] = newPos;
			}
		}
	}

	//Check rocks
	for (int i = 0; i < 9; i++) {
		if (rockTrackPos[i] == trackPos) {
			if (glm::distance(rockArray[i], diaPos) <= 3.0f) {
				speed -= 10;
				int newPos = rand() % 3;
				rockArray[i] = MoveObject((rand() % 1500), newPos);
				rockTrackPos[i] = newPos;
			}
		}
	}

}

glm::vec3 Game::MoveObject(float theDistance, int trackPosition) {
	glm::vec3 finalPos;

	glm::vec3 p;
	m_pCatmullRom->Sample(theDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(theDistance + 1.f, pNext);

	glm::vec3 tangent = glm::normalize(pNext - p);

	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);

	if (trackPosition == 0) {
		finalPos = glm::vec3(p.x, p.y + 1.3f, p.z);;
	}
	else if (trackPosition == 1) {
		glm::vec3 leftPosition = glm::vec3(p + 3.2f * crossOfXAxis);
		finalPos = glm::vec3(leftPosition.x, leftPosition.y + 1.3f, leftPosition.z);
	}
	else if (trackPosition == 2) {
		glm::vec3 rightPosition = glm::vec3(p + 3.2f * -crossOfXAxis);
		finalPos = glm::vec3(rightPosition.x, rightPosition.y + 1.3f, rightPosition.z);
	}

	return finalPos;
}

void Game::RenderRock(float theDistance, int trackPosition, int whichRock) {
	glm::vec3 finalPos;

	glm::vec3 p;
	m_pCatmullRom->Sample(theDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(theDistance + 1.f, pNext);

	glm::vec3 tangent = glm::normalize(pNext - p);

	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);

	if (trackPosition == 0) {
		finalPos = glm::vec3(p.x, p.y + 1.3f, p.z);;
	}
	else if (trackPosition == 1) {
		glm::vec3 leftPosition = glm::vec3(p + 3.2f * crossOfXAxis);
		finalPos = glm::vec3(leftPosition.x, leftPosition.y + 1.3f, leftPosition.z);
	}
	else if (trackPosition == 2) {
		glm::vec3 rightPosition = glm::vec3(p + 3.2f * -crossOfXAxis);
		finalPos = glm::vec3(rightPosition.x, rightPosition.y + 1.3f, rightPosition.z);
	}

	rockArray[whichRock] = finalPos;
}

void Game::RenderDiamond(float theDistance, int trackPosition, int whichDiamond) {
	glm::vec3 finalPos;

	glm::vec3 p;
	m_pCatmullRom->Sample(theDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(theDistance + 1.f, pNext);

	glm::vec3 tangent = glm::normalize(pNext - p);

	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);

	if (trackPosition == 0) {
		finalPos = glm::vec3(p.x, p.y + 1.3f, p.z);;
	}
	else if (trackPosition == 1) {
		glm::vec3 leftPosition = glm::vec3(p + 3.2f * crossOfXAxis);
		finalPos = glm::vec3(leftPosition.x, leftPosition.y + 1.3f, leftPosition.z);
	}
	else if (trackPosition == 2) {
		glm::vec3 rightPosition = glm::vec3(p + 3.2f * -crossOfXAxis);
		finalPos = glm::vec3(rightPosition.x, rightPosition.y + 1.3f, rightPosition.z);
	}

	diamondArray[whichDiamond] = finalPos;
}

void Game::RenderHat(float theDistance, int trackPosition, int whichHat) {
	glm::vec3 finalPos;

	glm::vec3 p;
	m_pCatmullRom->Sample(theDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(theDistance + 1.f, pNext);

	glm::vec3 tangent = glm::normalize(pNext - p);

	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);

	if (trackPosition == 1) {
		glm::vec3 leftPosition = glm::vec3(p + 6.2f * crossOfXAxis);
		finalPos = glm::vec3(leftPosition.x, leftPosition.y, leftPosition.z);
	}
	else if (trackPosition == 2) {
		glm::vec3 rightPosition = glm::vec3(p + 6.2f * -crossOfXAxis);
		finalPos = glm::vec3(rightPosition.x, rightPosition.y, rightPosition.z);
	}

	hatArray[whichHat] = finalPos;

}

void Game::RenderAxe(float theDistance, int trackPosition, int whichAxe) {
	glm::vec3 finalPos;

	glm::vec3 p;
	m_pCatmullRom->Sample(theDistance, p);
	glm::vec3 pNext;
	m_pCatmullRom->Sample(theDistance + 1.f, pNext);

	glm::vec3 tangent = glm::normalize(pNext - p);

	glm::vec3 crossOfXAxis = glm::cross(glm::vec3(0, 1, 0), tangent);

	if (trackPosition == 1) {
		glm::vec3 leftPosition = glm::vec3(p + 6.2f * crossOfXAxis);
		finalPos = glm::vec3(leftPosition.x, leftPosition.y, leftPosition.z);
	}
	else if (trackPosition == 2) {
		glm::vec3 rightPosition = glm::vec3(p + 6.2f * -crossOfXAxis);
		finalPos = glm::vec3(rightPosition.x, rightPosition.y, rightPosition.z);
	}
	axeArray[whichAxe] = finalPos;
}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {

		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		//Render FPS
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);

		//Rendering Speed
		m_pFtFont->Render(width - 130, height - 53, 30, "Speed: %d", speed);

		//Gold
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 0.00465f, 1.0f));
		m_pFtFont->Render(16, height - 53, 35, "Gold: 15");

		//Silver
		fontProgram->SetUniform("vColour", glm::vec4(0.52f, 0.52f, 0.52f, 1.0f));
		m_pFtFont->Render(16, height - 85, 30, "Silver: 10");

		//Bronze
		fontProgram->SetUniform("vColour", glm::vec4(0.6902f, 0.5529f, 0.3412f, 1.0f));
		m_pFtFont->Render(16, height - 110, 28, "Bronze: 5");

		//Rendering Score
		if (score <= 3) {
			fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (score >= 5 && score <= 9) {
			fontProgram->SetUniform("vColour", glm::vec4(0.6902f, 0.5529f, 0.3412f, 1.0f));
		}
		else if (score >= 10 && score <= 14) {
			fontProgram->SetUniform("vColour", glm::vec4(0.52f, 0.52f, 0.52f, 1.0f));
		}
		else if (score >= 15) {
			fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 0.00465f, 1.0f));
		}
		m_pFtFont->Render(width - 130, height - 25, 30, "Score: %d", score);

		//Render Game Over
		if (speed == 0) {
			fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.f, 0.f, 1.0f));
			m_pFtFont->Render(200, height - 290, 80, "Game Over");
		}
	}
}

void Game::rotateAroundPoint() {
	//Uses trigonometry to move the camera in a circle around the diamond - diamond positon at glm::vec3(30.0f, 8.0f, 100.0f)
	static float t = 0.0f;
	t += 0.0005f * (float)m_dt;
	double X = 30 + cos(t) * 14;
	double Y = 100 + sin(t) * 14;
	glm::vec3 x = glm::vec3(X, 13.0f, Y);
	m_pCamera->Set(x, glm::vec3(30.0f, 8.0f, 100.0f), glm::vec3(0, 1, 0));
}


// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	double angle = 0;

	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			//m_pAudio->PlayEventSound();
			cameraView = 1;
			pause = false;
			break;
		case '2':
			cameraView = 2;
			pause = false;
			break;
		case '3':
			cameraView = 3;
			pause = false;
			break;
		case '4':
			cameraView = 4;
			pause = true;
			break;
		case '5':
			score++;
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case 'A':
			if (trackPos == 0) {
				trackPos = 1;
			}
			else if (trackPos == 2) {
				trackPos = 0;
			}
				break;
		case 'D':
			if (trackPos == 0) {
				trackPos = 2;
			}
			else if (trackPos == 1) {
				trackPos = 0;
			}
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return int(game.Execute());
}
