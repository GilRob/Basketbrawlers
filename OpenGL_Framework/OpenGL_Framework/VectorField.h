#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <fstream>

using namespace std;

//base class for any fields
struct Field {
	float weight = 1.0f;
	glm::vec2 direction = glm::vec2(0.0f, 0.0f);
	glm::vec2 location = glm::vec2(0.0f, 0.0f);

	glm::vec2 getWeight(glm::vec2 position);
};


class VectorField {
public:
	VectorField();
	~VectorField();

	void addField(float weight, glm::vec2 range, glm::vec2 dir);


private:
	vector<Field> fields;
	glm::vec2 range;


};