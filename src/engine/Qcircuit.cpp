#include "QCircuit.h"

typedef std::complex<double> c;

Qcircuit::Qcircuit()
{
    ;
}

void Qcircuit::readFile(std::string filename)
{
    m_parser.scanLines(filename);
    m_parser.parse(m_gateList, m_numQubits, m_qregister);
    m_parser.reset();
}

void Qcircuit::run()
{
    for (auto&& g : m_gateList)
    {
        g->act(m_qregister);
    }
}

std::string Qcircuit::binary(int a, int n)
{
    std::string b{""};
    int mask = 1;
    for(int i{}; i < n; i++)
    {
        if (mask & a)
        {
            b = "1"+b;
        } else {
            b = "0"+b;
        }
        mask<<=1;
    }
    return b;
};

void Qcircuit::printRegister()
{
    c weight;
    double mod;
    std::cout<<std::endl;
    for(int i{};i<m_qregister.size();i++)
    {
        weight = m_qregister[i];
        std::cout<<real(weight) 
            <<(imag(weight) >= 0.0 ? "+" : "")
            <<imag(weight)<<"i"
            <<" |"<<binary(i, m_numQubits)<<"> +"
            <<std::endl;
        mod += pow(abs(m_qregister[i]),2);
    }
    std::cout<<std::endl;
    for(int i{};i<m_qregister.size();i++)
    {
        std::cout<<"P("<<i
            <<") = "
            <<pow(abs(m_qregister[i]),2)/mod
            <<std::endl;
    }
};