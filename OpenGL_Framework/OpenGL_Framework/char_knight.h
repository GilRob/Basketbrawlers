#pragma once
#include "character.h"

//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Knight : public Character {

public:
	Knight() {}
	Knight(const std::string& body, const std::string& texture);

	int storedCharge;

	Transform jab();
	Transform sAttack();
	Transform dAttack();
	Transform uAttack();
	Transform nSpecial(bool charging);
	Transform sSpecial();
	Transform dSpecial();
	Transform uSpecial();
	Transform nAir();
	Transform sAir();
	Transform dAir();
	Transform uAir();

protected:

private:
};