#include "ParticleEffect.h"
#include "Random.h"

#define BUFFER_OFFSET(i) ((char *)0 + (i))

ParticleEffect::ParticleEffect()
{
	Gravity = 0.5;
	Mass = 2;
}

ParticleEffect::~ParticleEffect()
{
	if (_VAO != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VAO);
	}

	if (_VBO_Position != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Position);
	}

	if (_VBO_Size != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Size);
	}

	if (_VBO_Alpha != GL_NONE)
	{
		glDeleteVertexArrays(1, &_VBO_Alpha);
	}

	if (_Particles.Positions != nullptr)
	{
		delete[] _Particles.Positions;
		delete[] _Particles.Velocities;
		delete[] _Particles.Alpha;
		delete[] _Particles.Ages;
		delete[] _Particles.Lifetimes;
		delete[] _Particles.Size;
	}
}

bool ParticleEffect::Init(const std::string &textureFile, unsigned int maxParticles, unsigned int rate)
{
	if (!_Texture.Load(textureFile))
	{
		return false;
	}

	_MaxParticles = maxParticles;
	_Rate = (float)rate;

	_Particles.Positions = new glm::vec3[_MaxParticles];
	_Particles.Velocities = new glm::vec3[_MaxParticles];
	_Particles.Alpha = new float[_MaxParticles];
	_Particles.Ages = new float[_MaxParticles];
	_Particles.Lifetimes = new float[_MaxParticles];
	_Particles.Size = new float[_MaxParticles];

	//Setup OpenGL Memory
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO_Position);
	glGenBuffers(1, &_VBO_Size);
	glGenBuffers(1, &_VBO_Alpha);


	glBindVertexArray(_VAO);

	glEnableVertexAttribArray(0);	//Vertex
	glEnableVertexAttribArray(1);	//Size
	glEnableVertexAttribArray(2);	//Alpha

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Size);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)1, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Alpha);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _MaxParticles, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)2, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	return true;
}

void ParticleEffect::Update(float elapsed)
{
	int NumToSpawn = (int)_Rate;

		/// Create new particles ///
	while (
		//We have not reached the particle cap and...
		_NumCurrentParticles < _MaxParticles &&
		//We have more particles to generate this frame...
		NumToSpawn > 0)
	{
		_Particles.Alpha[_NumCurrentParticles] = RandomRangef(LerpAlpha.x, LerpAlpha.y);
		_Particles.Ages[_NumCurrentParticles] = 0.0f;
		_Particles.Lifetimes[_NumCurrentParticles] = RandomRangef(RangeLifetime.x, RangeLifetime.y);
		_Particles.Size[_NumCurrentParticles] = RandomRangef(LerpSize.x, LerpSize.y);
		//Missing .Set which is what the video uses
		//_Particles.Positions[_NumCurrentParticles] = vec3((RandomRangef(RangeX.x, RangeX.y), RandomRangef(RangeY.x, RangeY.y), RandomRangef(RangeZ.x, RangeZ.y)));
		_Particles.Positions[_NumCurrentParticles].x = RandomRangef(RangeX.x, RangeX.y);
		_Particles.Positions[_NumCurrentParticles].y = RandomRangef(RangeY.x, RangeY.y);
		_Particles.Positions[_NumCurrentParticles].z = RandomRangef(RangeZ.x, RangeZ.y);

		//send the particle in a random direction, with a velocity between our range
		//Missing .Set which is what the video uses
		//_Particles.Velocities[_NumCurrentParticles] = vec3((RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f)));
		_Particles.Velocities[_NumCurrentParticles].x = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles].y = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles].z = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles] = glm::normalize(_Particles.Velocities[_NumCurrentParticles]);
		_Particles.Velocities[_NumCurrentParticles] *= RandomRangef(RangeVelocity.x, RangeVelocity.y);

		//counters...
		_NumCurrentParticles++;
		NumToSpawn--;
	}
	

	/// Update existing particles ///
	for (unsigned i = 0; i < _NumCurrentParticles; i++)
	{
		_Particles.Ages[i] += elapsed;

		//Explanation of this is on Week 9 video at time 5:30 (maybe)
		if (_Particles.Ages[i] > _Particles.Lifetimes[i])
		{
			//remove the particle by replacing it with the one at the top of the stack
			_Particles.Alpha[i]		= _Particles.Alpha[_NumCurrentParticles - 1];
			_Particles.Ages[i]		= _Particles.Ages[_NumCurrentParticles - 1];
			_Particles.Lifetimes[i] = _Particles.Lifetimes[_NumCurrentParticles - 1];
			_Particles.Positions[i] = _Particles.Positions[_NumCurrentParticles - 1];
			_Particles.Size[i]		= _Particles.Size[_NumCurrentParticles - 1];
			_Particles.Velocities[i] = _Particles.Velocities[_NumCurrentParticles - 1];

			_NumCurrentParticles--;
			continue;
		}

		if (HaveGravity)
		{
			//physics update
			force = glm::vec3(0, 0 - Gravity, 0);
			acceleration = force / Mass;
			_Particles.Velocities[i] += acceleration;
			_Particles.Positions[i] += _Particles.Velocities[i] * elapsed;
		}
		else
			_Particles.Positions[i] += _Particles.Velocities[i] * elapsed;

		float interp = _Particles.Ages[i] / _Particles.Lifetimes[i];
		
		_Particles.Alpha[i] = glm::mix(LerpAlpha.x, LerpAlpha.y, interp);
		_Particles.Size[i]	= glm::mix(LerpSize.x, LerpSize.y, interp);
	}

	//Update OpenGL on the changes

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Position);
	//Call this because glBufferData will reallocate the entire array :O
	//This will update the data in an existing array, update a small subset as you need to
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * _NumCurrentParticles, &_Particles.Positions[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Size);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * _NumCurrentParticles, &_Particles.Size[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO_Alpha);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * _NumCurrentParticles, &_Particles.Alpha[0]);

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void ParticleEffect::Render()
{
	if (_NumCurrentParticles == 0)
	{
		return;
	}

	_Texture.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Depth mask disables/enables the ability for a render to write to the depth buffer
	glDepthMask(GL_FALSE);

	glBindVertexArray(_VAO);
	glDrawArrays(GL_POINTS, 0, _NumCurrentParticles); //Thousands of particles can be drawn in one call! 
	glBindVertexArray(GL_NONE);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	_Texture.UnBind();
}

void ParticleEffect::Reset()
{
	for (unsigned i = 0; i < _NumCurrentParticles; i++)
	{
		//remove the particle by replacing it with the one at the top of the stack
		_Particles.Alpha[i] = _Particles.Alpha[_NumCurrentParticles - 1];
		_Particles.Ages[i] = _Particles.Ages[_NumCurrentParticles - 1];
		_Particles.Lifetimes[i] = _Particles.Lifetimes[_NumCurrentParticles - 1];
		_Particles.Positions[i] = _Particles.Positions[_NumCurrentParticles - 1];
		_Particles.Size[i] = _Particles.Size[_NumCurrentParticles - 1];
		_Particles.Velocities[i] = _Particles.Velocities[_NumCurrentParticles - 1];

		_NumCurrentParticles--;
	}
	while (
		//We have not reached the particle cap and...
		_NumCurrentParticles < _MaxParticles)
	{
		_Particles.Alpha[_NumCurrentParticles] = RandomRangef(LerpAlpha.x, LerpAlpha.y);
		_Particles.Ages[_NumCurrentParticles] = 0.0f;
		_Particles.Lifetimes[_NumCurrentParticles] = RandomRangef(RangeLifetime.x, RangeLifetime.y);
		_Particles.Size[_NumCurrentParticles] = RandomRangef(LerpSize.x, LerpSize.y);
		//Missing .Set which is what the video uses
		//_Particles.Positions[_NumCurrentParticles] = vec3((RandomRangef(RangeX.x, RangeX.y), RandomRangef(RangeY.x, RangeY.y), RandomRangef(RangeZ.x, RangeZ.y)));
		_Particles.Positions[_NumCurrentParticles].x = RandomRangef(RangeX.x, RangeX.y);
		_Particles.Positions[_NumCurrentParticles].y = RandomRangef(RangeY.x, RangeY.y);
		_Particles.Positions[_NumCurrentParticles].z = RandomRangef(RangeZ.x, RangeZ.y);

		//send the particle in a random direction, with a velocity between our range
		//Missing .Set which is what the video uses
		//_Particles.Velocities[_NumCurrentParticles] = vec3((RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f), RandomRangef(-1.0f, 1.0f)));
		_Particles.Velocities[_NumCurrentParticles].x = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles].y = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles].z = RandomRangef(-1.0f, 1.0f);
		_Particles.Velocities[_NumCurrentParticles] = glm::normalize(_Particles.Velocities[_NumCurrentParticles]);
		_Particles.Velocities[_NumCurrentParticles] *= RandomRangef(RangeVelocity.x, RangeVelocity.y);

		//counters...
		_NumCurrentParticles++;
		continue;
	}
}
