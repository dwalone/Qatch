#ifndef CustomGate_H
#define CustomGate_H

#include "Gate.h"

class CustomGate : public Gate
{
public:
	CustomGate(std::string name, std::vector<std::unique_ptr<Gate>>);
	void act(std::vector<c> &qregister);
protected:
	std::vector<std::unique_ptr<Gate>> m_gates;
};

#endif