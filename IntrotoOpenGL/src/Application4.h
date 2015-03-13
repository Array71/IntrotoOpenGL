#ifndef APPLICATION4_H
#define APPLICATION4_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MeshArray.h"

#include "tiny_obj_loader.h"

class MeshArray;
class FlyCamera;
struct OpenGLInfo;
struct KeyFrame;

class Application4
{
public:
	Application4();
	~Application4();

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

	glm::vec3 m_positions[4];
	glm::quat m_rotations[4];

	unsigned int anim;
	unsigned int anim2;
	float animCountdown;

	struct KeyFrame
	{
		glm::vec3 position;
		glm::quat rotation;
	};

	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	glm::mat4 m_hipBone;
	glm::mat4 m_kneeBone;
	glm::mat4 m_ankleBone;
};




#endif