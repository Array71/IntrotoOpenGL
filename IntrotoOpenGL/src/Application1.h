#ifndef APPLICATION1_H
#define APPLICATION1_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "MeshArray.h"

class MeshArray;
class FlyCamera;

class Application1
{
public:
	Application1();
	~Application1();

	int Run();

	void CreateSimpleShader();
	void CreateTextureShader();

	void Update(float deltaTime);

private:	
	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateCube();
	void GenerateSimpleTexturePlane();
	//void LoadObj();

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;
	unsigned int m_textureProgramID;

	//unsigned int m_row;
	//unsigned int m_column;
	
	unsigned int m_texture;

	MeshArray* m_meshArray;
	FlyCamera* m_camera;
};


//class MeshArray
//{
//public:
//	MeshArray() { 
//		m_iterator = 0;
//		m_meshCount = 0;
//	};
//	~MeshArray(){};
//
//
//	void Iterate() { m_iterator++; };
//
//	unsigned int GetVAO() {
//		//m_iterator++;
//		//return m_VAO[m_iterator - 1];
//		return m_VAO[m_iterator];
//	}
//	unsigned int GetIndexCount() {
//		return m_indexCount[m_iterator];
//	}
//
//	void SetVAO(unsigned int VAO) {
//		//m_iterator++;
//		m_VAO[m_iterator] = VAO;
//	}
//
//	void SetIndexCount(unsigned int indexCount) {
//		m_indexCount[m_iterator] = indexCount;
//	}
//
//	void AddedMesh() { m_meshCount++; };
//	unsigned int GetMeshCount() { return m_meshCount; };
//	void resetIterator(){ m_iterator = 0; };
//	void resetMeshCount(){ m_meshCount = 0; };
//
//public:
//	unsigned int m_VAO[500];
//	unsigned int m_indexCount[500];
//	unsigned int m_iterator;
//	unsigned int m_meshCount;
//	
//};

#endif