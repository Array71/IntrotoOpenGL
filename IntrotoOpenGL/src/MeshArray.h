#ifndef MESHARRAY_H
#define MESHARRAY_H

class MeshArray
{
public:
	MeshArray() { 
		m_iterator = 0;
		m_meshCount = 0;
	};
	~MeshArray(){};


	void Iterate() { m_iterator++; };

	unsigned int GetVAO() {
		//m_iterator++;
		//return m_VAO[m_iterator - 1];
		return m_VAO[m_iterator];
	}
	unsigned int GetIndexCount() {
		return m_indexCount[m_iterator];
	}

	void SetVAO(unsigned int VAO) {
		//m_iterator++;
		m_VAO[m_iterator] = VAO;
	}

	void SetIndexCount(unsigned int indexCount) {
		m_indexCount[m_iterator] = indexCount;
	}

	void AddedMesh() { m_meshCount++; };
	unsigned int GetMeshCount() { return m_meshCount; };
	void resetIterator(){ m_iterator = 0; };
	void resetMeshCount(){ m_meshCount = 0; };

public:
	unsigned int m_VAO[500];
	unsigned int m_indexCount[500];
	unsigned int m_iterator;
	unsigned int m_meshCount;
	
};

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

#endif