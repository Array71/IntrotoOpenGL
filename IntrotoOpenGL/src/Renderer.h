#ifndef RENDERER_H
#define RENDERER_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Gizmos.h"
#include <stb_image.h>
#include "tiny_obj_loader.h"
#include "FBXFile.h"

#include "Particles.h"
#include "MeshArray.h"
#include "Camera.h"
#include "Vertex.h"



class Renderer
{
public:
	Renderer(FlyCamera* camera);
	~Renderer();

	void Update(float deltaTime);
	void Draw();

	void Load();

	unsigned int LoadShader(unsigned int type, const char* path);
	void CreateShader(unsigned int &shader, const char* vert, const char* frag);

	void CreateTextureShader();
	void CreateObjShader();

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);


private:

	//SHADERS
	unsigned int m_programObjID;
	unsigned int m_programTexturePlaneID;
	unsigned int m_programTexturePlaneSimpleID;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;		

	//ParticleEmitter* m_particleEmitter;
	GPUParticleEmitter* m_gpuParticleEmitter;

	FBXFile* m_FBX;

	MeshArray* m_meshArray;
	FlyCamera* m_camera;
	//GLFWwindow* m_window;

	std::vector<OpenGLInfo> m_gl_info;
		
	//For gizmo 'leg' animation
	struct KeyFrame
	{
		glm::vec3 position;
		glm::quat rotation;
	};
	
private:
	//Test content

	//Texture plane
	void GenerateTexturePlane();
	unsigned int m_VAOtest;
	unsigned int m_texture, m_normalMap;

	//Gizmo movement animation
	glm::vec3 m_positions[4];
	glm::quat m_rotations[4];

	unsigned int anim;
	unsigned int anim2;
	float animCountdown;

	//Gizmo test animation
	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	glm::mat4 m_hipBone;
	glm::mat4 m_kneeBone;
	glm::mat4 m_ankleBone;

};



#endif