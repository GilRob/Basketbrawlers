#include "VectorField.h"

VectorField::VectorField()
{
	range = 5;
}

VectorField::~VectorField()
{
}

void VectorField::init(glm::vec2 p1, glm::vec2 p2)
{
	p1Legs.active = false;
	p1Legs.weight = 1;
	p1Legs.direction = glm::vec2(0.0f,0.0f);
	p1Legs.location = p1;

	p2Legs.active = false;
	p2Legs.weight = 1;
	p2Legs.direction = glm::vec2(0.0f, 0.0f);
	p2Legs.location = p2;

}

glm::vec2 VectorField::totalWeight(glm::vec2 position)
{
	return glm::vec2();
}

void VectorField::addSwing(int player, glm::vec2 dir)
{
}

void VectorField::update(glm::vec2 p1, glm::vec2 p1Dir, glm::vec2 p2, glm::vec2 p2Dir)
{
}
