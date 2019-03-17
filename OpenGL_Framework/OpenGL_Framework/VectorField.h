#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <fstream>

using namespace std;

//base class for any fields
struct Field {
	bool active = false;
	float weight = 1.0f;
	glm::vec2 direction = glm::vec2(0.0f, 0.0f);
	glm::vec2 location = glm::vec2(0.0f, 0.0f);

	glm::vec2 getWeight(glm::vec2 position) {
		float dist = glm::distance(position, location);
		return direction * (weight / (dist + 1.0f));
	};
};


class VectorField {
public:
	VectorField();
	~VectorField();

	void init(glm::vec2 p1, glm::vec2 p2);

	//returns the total weight of all fields, or 0
	glm::vec2 totalWeight(glm::vec2 position);

	void addSwing(int player, glm::vec2 dir);

	void update(glm::vec2 p1, glm::vec2 p1Dir, glm::vec2 p2, glm::vec2 p2Dir);

	float range;

private:
	Field p1Legs;
	Field p2Legs;
	Field p1Head;
	Field p2Head;
	Field p1Swing;
	Field p2Swing;

	vector<Field>* fields;


};