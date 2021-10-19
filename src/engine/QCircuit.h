#ifndef Qcircuit_H
#define Qcircuit_H

#include "Parser.h"

typedef std::complex<double> c;

class Qcircuit
{
public:
    Qcircuit();
    void readFile(std::string filename);
	void run();
    void printRegister();
    std::string binary(int a, int n);
    //void addGate(Gate* gate);
    ~Qcircuit(){};
private:
	int m_numQubits;
    std::vector<std::unique_ptr<Gate>> m_gateList;
    std::vector<c> m_qregister;
    Parser m_parser;
};

#endif