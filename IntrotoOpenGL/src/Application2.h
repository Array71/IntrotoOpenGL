#ifndef APPLICATION2_H
#define APPLICATION2_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "MeshArray.h"

#include "tiny_obj_loader.h"

class MeshArray;
class FlyCamera;
struct OpenGLInfo;

class Application2
{
public:
	Application2();
	~Application2();

	int Run();

	void CreateSimpleShader();
	void CreateTextureShader();
	void CreateSimpleObjShader();

	void Load();
	
	void Update(float deltaTime);
	void Draw();

private:	
	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateCube();
	void GenerateSimpleTexturePlane();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;
	unsigned int m_textureProgramID;
	unsigned int m_simpleObjProgramID;
	
	unsigned int m_texture;

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


#endif