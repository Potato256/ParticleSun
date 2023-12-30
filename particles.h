#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

class Particle
{
public:
	float theta;
	float z;
	float vel;
	float phase;
	float pos;
	float phi;
	float limit;
	Particle(float t = 0.0f, float _z = 0.0f, float v = 0.0f, float ph = 0.0f, float p = 0.0f, float r= 0.0f, float l = 1.0f)
	{
		theta = t;
		z = _z;
		vel = v;
		phase = ph;
		phi = p;
		pos = r;
		limit = l;
	}
};

#endif