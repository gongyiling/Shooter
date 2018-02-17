#pragma once
#include <string>

class Collider
{
public:
	virtual void OnCollisionEnter(Collider* other) {};
public:
	std::string m_type;
};
