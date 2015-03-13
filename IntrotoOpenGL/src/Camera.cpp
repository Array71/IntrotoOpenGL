#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <iostream>


//BaseCamera


BaseCamera::BaseCamera()
{
	m_bPerspectiveSet = false;
	UpdateProjectionViewTransform();
}

BaseCamera::BaseCamera(glm::vec3 position)
{
	m_worldTransform[3] = glm::vec4(position, 1);
	m_bPerspectiveSet = false;
	UpdateProjectionViewTransform();
}

BaseCamera::BaseCamera(glm::mat4 transform) : m_worldTransform(transform)
{
	m_bPerspectiveSet = false;
	UpdateProjectionViewTransform();
}

void BaseCamera::SetTransform(glm::mat4 transform)
{
	m_worldTransform = transform;
	UpdateProjectionViewTransform();
}

const glm::mat4 BaseCamera::GetTransform() const
{
	return m_worldTransform;
}

void BaseCamera::SetPosition(glm::vec3 position)
{
	m_worldTransform[3] = glm::vec4(position, 1);
	UpdateProjectionViewTransform();
}


void BaseCamera::LookAt(glm::vec3 lookAt, glm::vec3 worldUp)
{
	glm::vec4 vLocation = m_worldTransform[3];
	m_worldTransform = glm::inverse(glm::lookAt(vLocation.xyz(), lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::LookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 worldUp)
{
	m_worldTransform = glm::inverse(glm::lookAt(position, lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::SetUpPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
	m_bPerspectiveSet = true;
	
	UpdateProjectionViewTransform();
}

void BaseCamera::UpdateProjectionViewTransform()
{
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

void BaseCamera::Update(double dt)
{

}


// FlyCamera


void FlyCamera::Update(double dt)
{
	HandleKeyboardInput(dt);
	HandleMouseInput(dt);
}

void FlyCamera::HandleKeyboardInput(double dt)
{
	glm::vec3 vRight = m_worldTransform[0].xyz;
	glm::vec3 vUp = m_worldTransform[1].xyz;
	glm::vec3 vForward = m_worldTransform[2].xyz;

	glm::vec3 moveDir(0.0f);

	if(glfwGetKey(m_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_fFlySpeed = m_fOriginalSpeed * 10.0f;
	}
	else
	{
		m_fFlySpeed = m_fOriginalSpeed;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir -= vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir += vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		moveDir += vUp;
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDir -= vUp;
	}

	//if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	//{
	//	//moveDir -= vRight;
	//	/*SetTransform(GetTransform() * glm::mat4(	0.0f,0.0f,-1.0f,0.0f,
	//												0.0f,1.0f,0.0f,0.0f,
	//												1.0f,0.0f,0.0f,0.0f,
	//												0.0f,0.0f,0.0f,1.0f));*/
	//	SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * 0.25f * (float)dt, glm::vec3(0, 1, 0)));
	//}
	//
	//if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	//{
	//	SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * -0.25f * (float)dt, glm::vec3(0, 1, 0)));
	//}
	//
	//if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	//{
	//	SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * 0.25f * (float)dt, glm::vec3(1, 0, 0)));
	//	//moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	//}
	//if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	//{
	//	SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * -0.25f * (float)dt, glm::vec3(1, 0, 0)));
	//	//moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	//}
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * 0.25f * (float)dt, glm::vec3(0, 0, 1)));
		//moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * -0.25f * (float)dt, glm::vec3(0, 0, 1)));
		//moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	}

	float fLength = glm::length(moveDir);
	if (fLength > 0.01f)
	{
		moveDir = ((float)dt * m_fFlySpeed) * glm::normalize(moveDir);
		SetPosition(GetPosition() + moveDir);
	}
}

void FlyCamera::HandleMouseInput(double dt)
{
	if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (m_bViewButtonClicked == false)
		{
			int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);


			m_dCursorX = width / 2.0;
			m_dCursorY = height / 2.0;
			glfwSetCursorPos(m_pWindow, m_dCursorX, m_dCursorY);
			m_bViewButtonClicked = true;
		}
		else
		{
			/*int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);
			glfwSetCursorPos(m_pWindow, width / 2.0, height / 2.0);*/

			double mouseX, mouseY;
			glfwGetCursorPos(m_pWindow, &mouseX, &mouseY);

			double xOffset = mouseX - m_dCursorX;
			double yOffset = mouseY - m_dCursorY;

			m_dCursorX = mouseX;
			m_dCursorY = mouseY;

			//CalculateRotation(xOffset, yOffset);

			
			glm::vec3 moveDir(0.0f);

			//glm::vec3 rot = glm::vec3(GetTransform()[0][1], GetTransform()[1][1], GetTransform()[2][1]);		

			SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * (float)xOffset * -0.1f* (float)dt, glm::vec3(0, 1, 0)));
			SetTransform(GetTransform() * glm::rotate(glm::pi<float>() * (float)yOffset * -0.1f * (float)dt, glm::vec3(1, 0, 0)));

			//Rounding protection needed
			glm::mat4 oldTransform = GetTransform();
			glm::mat4 transform;		
			glm::vec3 worldUp = glm::vec3(0, 1, 0);

			transform[0] = glm::normalize(glm::vec4(glm::cross(worldUp, oldTransform[2].xyz()), 0));
			transform[1] = glm::normalize(glm::vec4(glm::cross(oldTransform[2].xyz(), transform[0].xyz()), 0));
			transform[2] = glm::normalize(oldTransform[2]);

			transform[3] = oldTransform[3];

			SetTransform(transform);
			

			std::cout << xOffset << " " << yOffset << std::endl;

			
		}
	}
	else
	{
		m_bViewButtonClicked = false;
	}
}