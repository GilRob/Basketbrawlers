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
	//init vector fields
	p1Legs.active = false;
	p1Legs.weight = 1;
	p1Legs.direction = glm::vec2(0.0f,0.0f);
	p1Legs.location = p1;

	p2Legs.active = false;
	p2Legs.weight = 1;
	p2Legs.direction = glm::vec2(0.0f, 0.0f);
	p2Legs.location = p2;

	p1Head.active = false;
	p1Head.weight = 1;
	p1Head.direction = glm::vec2(0.0f, 0.0f);
	p1Head.location = p1 + glm::vec2(0.0f,headOffSet);

	p2Head.active = false;
	p2Head.weight = 1;
	p2Head.direction = glm::vec2(0.0f, 0.0f);
	p2Head.location = p2 + glm::vec2(0.0f, headOffSet);
	p1Swing.active = false;
	p1Swing.weight = 4;
	p1Swing.direction = glm::vec2(0.0f, 0.0f);
	p1Swing.location = p1Head.location;
	  
	p2Swing.active = false;
	p2Swing.weight = 4;
	p2Swing.direction = glm::vec2(0.0f, 0.0f);
	p2Swing.location = p2Head.location;

}

glm::vec2 VectorField::totalWeight(glm::vec2 position)
{
	glm::vec2 totalWeight = glm::vec2(0.0f, 0.0f);

	//adds weight of p1's movement
	if (p1Head.active && glm::distance(position, p1Head.location) < range ) {
		totalWeight += p1Head.getWeight(position);
		if (glm::distance(position, p1Legs.location) < range) {
			totalWeight += p1Legs.getWeight(position);
		}
	}
	//adds weight of p1's hits
	if (p1Swing.active && glm::distance(position, p1Swing.location) < range) {
		totalWeight += p1Swing.getWeight(position);
	}

	//adds weight of p2's movement
	if (p2Head.active && glm::distance(position, p2Head.location) < range) {
		totalWeight += p2Head.getWeight(position);
		if (glm::distance(position, p2Legs.location) < range) {
			totalWeight += p2Legs.getWeight(position);
		}
	}
	//adds weight of p2's hits
	if (p2Swing.active && glm::distance(position, p2Swing.location) < range) {
		totalWeight += p2Swing.getWeight(position);
	}

	return totalWeight;

}

void VectorField::addSwing(int player, glm::vec2 dir)
{
	//inpulse vector if players attack
	if (player == 1) {
		p1Swing.active = true;
		p1Swing.direction = dir;
	}
	else {
		p2Swing.active = true;
		p2Swing.direction = dir;
	}
}

void VectorField::update(glm::vec2 p1, glm::vec2 p1Dir, glm::vec2 p2, glm::vec2 p2Dir)
{
	
	//update player 1 attributes
	if (p1Dir != glm::vec2(0.0f, 0.0f)) {
		p1Legs.active = p1Head.active = true;
		p1Legs.location = p1;
		p1Head.location = p1Swing.location = p1 + glm::vec2(0.0f, headOffSet);
		p1Head.direction = p1Legs.direction = p1Dir;
	}
	else {
		p1Legs.active = p1Head.active = false;
	}
	
	//update player 2 attributes
	if (p2Dir != glm::vec2(0.0f, 0.0f)) {
		p2Legs.active = p2Head.active = true;
		p2Legs.location = p2;
		p2Head.location = p2Swing.location = p2 + glm::vec2(0.0f, headOffSet);
		p2Head.direction = p2Legs.direction = p2Dir;
	}
	else {
		p2Legs.active = p2Head.active = false;
	}

	p1Swing.active = false;
	p2Swing.active = false;
}
