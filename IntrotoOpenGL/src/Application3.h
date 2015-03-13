#ifndef APPLICATION3_H
#define APPLICATION3_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "MeshArray.h"

#include "tiny_obj_loader.h"

class MeshArray;
class FlyCamera;
struct OpenGLInfo;

class Application3
{
public:
	Application3();
	~Application3();

	int Run();

	void CreateSimpleShader();
	void CreateTextureShader();
	void CreateSimpleObjShader();

	void Load();
	
	void Update(float deltaTime);
	void Draw();

private:	
//	void GenerateGrid(unsigned int rows, unsigned int cols);
//	void GenerateCube();
	void GenerateSimpleTexturePlane();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void cleanupOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	
private:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_VAOtest;

	unsigned int m_programID; //unused
	unsigned int m_textureProgramID;
	unsigned int m_simpleObjProgramID;
	
	unsigned int m_texture, m_normalMap;

	//unsigned int m_texture;

	MeshArray* m_meshArray;
	FlyCamera* m_camera;
	GLFWwindow* m_window;

	std::vector<OpenGLInfo> m_gl_info;
};

//struct OpenGLInfo
//{
//	unsigned int m_VAO;
//	unsigned int m_VBO;
//	unsigned int m_IBO;
//	unsigned int m_index_count;
//};

//struct VertexAdv
//{
//	float x, y, z, w;
//	float nx, ny, nz, nw;
//	float tx, ty, tz, tw;
//	float s, t;
//};


#endif