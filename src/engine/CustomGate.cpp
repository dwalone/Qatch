#include "CustomGate.h"

typedef std::complex<double> c;

const double pi = acos(-1.0);

CustomGate::CustomGate(std::string name, std::vector<std::unique_ptr<Gate>> gates)
{
    m_name = name;
    m_gates = std::move(gates);
}
void CustomGate::act(std::vector<c> &qregister)
{
    for (auto&& g : m_gates)
    {
        g->act(qregister);
    }
}