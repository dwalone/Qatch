#include "engine/QCircuit.h"

int main(int argc, char** argv)
{
    Qcircuit circuit;
    circuit.readFile(argv[1]);
    circuit.run();
    circuit.printRegister();
    return 0;
}