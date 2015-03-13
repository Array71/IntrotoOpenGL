#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialise();

private:
	unsigned int m_programID;

	unsigned int m_FBO;
	unsigned int m_FBOTexture;
	unsigned int m_FBODepth;
};

#endif