#ifndef PLANETS_H
#define PLANETS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

class star 
{
public:
	glm::vec3 pos;
	glm::vec3 vel;
	float mass;
	star(float m = 0.0f, glm::vec3 p = glm::vec3(0.0f), glm::vec3 v = glm::vec3(0.0f))
	{
		pos = p;
		mass = m;
		vel = v;
	}
};

#endif