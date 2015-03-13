#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW\glfw3.h>

class BaseCamera
{
public:
	BaseCamera();
	BaseCamera(glm::vec3 position);
	BaseCamera(glm::mat4 transform);
	~BaseCamera() {};

	virtual void Update(double dt);

	void SetTransform(glm::mat4 transform);
	const glm::mat4 GetTransform() const;

	void SetPosition(glm::vec3 position);
	glm::vec3 GetPosition() const { return m_worldTransform[3].xyz(); }

	void LookAt(glm::vec3 lookAt, glm::vec3 up);
	void LookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);

	void SetUpPerspective(float fieldOfView, float aspectRatio, float near = 0.1f, float far = 1000.0f);

	const glm::mat4& GetProjection() const { return m_projectionTransform; }
	const glm::mat4& GetView() const { return m_viewTransform; }
	const glm::mat4& GetProjectionView() const { return m_projectionViewTransform; }

	bool GetPerspectivesSet() const {return m_bPerspectiveSet;}

protected:
	void UpdateProjectionViewTransform();

	glm::mat4 m_worldTransform;


	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;

	bool m_bPerspectiveSet;
	
};



class FlyCamera : public BaseCamera
{
public:
	FlyCamera()
	{
		m_bViewButtonClicked = false;
	}
	FlyCamera(float flySpeed) : BaseCamera(), m_fFlySpeed(flySpeed) // ?
	{
		m_bViewButtonClicked = false;
		m_fOriginalSpeed = m_fFlySpeed;
	}
	~FlyCamera() {};

	virtual void Update(double dt);

	void SetFlySpeed(float fSpeed);
	float GetFlySpeed() const { return m_fFlySpeed; }

	//
	void SetInputWindow(GLFWwindow* pWindow) { m_pWindow = pWindow; }

protected:
	void HandleKeyboardInput(double dt);
	void HandleMouseInput(double dt);

	//void CalculateRotation(double xOffset, double yOffset);

	GLFWwindow* m_pWindow;
	float m_fFlySpeed;
	float m_fOriginalSpeed;

	bool m_bViewButtonClicked;
	double m_dCursorX, m_dCursorY;
};

#endif