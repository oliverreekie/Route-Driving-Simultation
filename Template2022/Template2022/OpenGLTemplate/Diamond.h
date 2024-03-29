#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a xz Diamond of a given size
class CDiamond
{
public:
	CDiamond();
	~CDiamond();
	void Create(string sDirectory, string sFilename);
	void Render();
	void Release();
	void CreateTriangle(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec2, glm::vec2, glm::vec2);
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	float m_width;
	float m_height;
};