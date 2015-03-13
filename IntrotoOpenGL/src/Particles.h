#ifndef PARTICLES_H
#define PARTICLES_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Particle;
struct ParticleVertex;
struct GPUParticle;

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Update(float a_deltaTime, const glm::mat4& a_cameraTransform);
	void Draw();

	void Initialise(unsigned int a_maxParticles, 
					unsigned int a_emitRate, 
					float a_lifetimeMin, float a_lifetimeMax,
					float a_velocityMin, float a_velocityMax,
					float a_startSize, float a_endSize,
					const glm::vec4& a_startColour, const glm::vec4& a_endColour);
	void Emit();

protected:
	Particle*		m_particles;
	unsigned int	m_firstDead;
	unsigned int	m_maxParticles;

	unsigned int	m_VAO, m_VBO, m_IBO;
	ParticleVertex* m_vertexData;

	glm::vec3 m_position;

	float m_emitTimer;
	float m_emitRate;

	float m_lifespanMin;
	float m_lifespanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
};

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 colour;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex
{
	glm::vec4 position;
	glm::vec4 colour;

};

class GPUParticleEmitter
{
public:
	GPUParticleEmitter();
	virtual ~GPUParticleEmitter();

	void Initialise(unsigned int a_maxParticles,
					float a_lifetimeMin, float a_lifetimeMax,
					float a_velocityMin, float a_velocityMax,
					float a_startSize, float a_endSize,
					const glm::vec4& a_startColour, 
					const glm::vec4& a_endColour);
	
	void Update(float deltaTime);
	void Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4& a_projectionView);

protected:
	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	unsigned int LoadShader(unsigned int type, const char* path);

	GPUParticle* m_particles;

	unsigned int m_maxParticles;

	glm::vec3 m_position;

	float m_lifespanMin, m_lifespanMax;
	float m_velocityMin, m_velocityMax;
	float m_startSize, m_endSize;
	glm::vec4 m_startColour, m_endColour;

	unsigned int m_activeBuffer;
	unsigned int m_VAO[2];
	unsigned int m_VBO[2];

	unsigned int m_drawShader;
	unsigned int m_updateShader;

	float m_lastDrawTime;
};

struct GPUParticle
{
	GPUParticle() : lifetime(1), lifespan(0){}

	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime;
	float lifespan;
};

#endif