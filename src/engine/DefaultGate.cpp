#include "DefaultGate.h"

typedef std::complex<double> c;

const double pi = acos(-1.0);

void DefaultGate::setActive(int activeQubit)
{
	m_activeQubit = activeQubit;
};

void DefaultGate::setControl(std::vector<int> controlQubits)
{
	m_controlQubits = controlQubits;
};

void MatrixGate::act(std::vector<c> &qregister)
{
    bool controls_passed = true;
    std::vector<c> temp(qregister.size(), c(0.0, 0.0));
    for (int n=0; n<qregister.size(); ++n) 
    {
        for (auto cq : m_controlQubits) 
        {
            if (!(n % static_cast<int>(pow(2,cq)) >= pow(2,cq-1)))
            {
                controls_passed = false;
            }       
        }
        if (controls_passed)
        {
            if (n % static_cast<int>(pow(2,m_activeQubit)) >= pow(2,m_activeQubit-1))
            {
                temp[n] += qregister[n]*(m_matrix[3]);
                temp[n - pow(2, m_activeQubit-1)] += qregister[n]*(m_matrix[1]);
            } else {
                temp[n] += qregister[n]*(m_matrix[0]);
                temp[n + pow(2, m_activeQubit-1)] += qregister[n]*(m_matrix[2]);
            }               
        } else {
            temp[n] += qregister[n];    
        }
        controls_passed=true;
    }
    for (int n{}; n<qregister.size(); n++)
    {
        qregister[n] = temp[n];
    }
}


HadamardGate::HadamardGate() {};
HadamardGate::HadamardGate(int activeQubit) 
{
    m_name = "fbejwkf";
    m_activeQubit = activeQubit;
    m_matrix = {c(1.0/(pow(2.0,0.5)), 0.0), c(1.0/(pow(2.0,0.5)), 0.0), c(1.0/(pow(2.0,0.5)), 0.0), c(-1.0/(pow(2.0,0.5)), 0.0)};

}
HadamardGate::HadamardGate(int activeQubit, std::vector<int> controlQubits) : HadamardGate(activeQubit) 
{
    m_controlQubits = controlQubits;
}

XGate::XGate() {};
XGate::XGate(int activeQubit) 
{
    m_activeQubit = activeQubit; 
    m_matrix = {c(0.0, 0.0), c(1.0, 0.0), c(1.0, 0.0), c(0.0, 0.0)};
}
XGate::XGate(int activeQubit, std::vector<int> controlQubits) : XGate(activeQubit) 
{
    m_controlQubits = controlQubits; 
}

YGate::YGate() {};
YGate::YGate(int activeQubit)
{
    m_activeQubit = activeQubit; 
    m_matrix = {c(0.0, 0.0), c(0.0, -1.0), c(0.0, 1.0), c(0.0, 0.0)};
}
YGate::YGate(int activeQubit, std::vector<int> controlQubits) : YGate(activeQubit)
{
    m_controlQubits = controlQubits; 
}

ZGate::ZGate() {};
ZGate::ZGate(int activeQubit) 
{
    m_activeQubit = activeQubit; 
    m_matrix = {c(1.0, 0.0), c(0.0, 0.0), c(0.0, 0.0), c(-1.0, 0.0)};
}
ZGate::ZGate(int activeQubit, std::vector<int> controlQubits) : ZGate(activeQubit)
{
    m_controlQubits = controlQubits; 
}

PhaseShiftGate::PhaseShiftGate() {};
PhaseShiftGate::PhaseShiftGate(int activeQubit, double phi) 
{
    m_activeQubit = activeQubit; 
    m_phase = phi;
    m_matrix = {c(1.0, 0.0), c(0.0, 0.0), c(0.0, 0.0), c(std::polar(1.0,phi))};
}
PhaseShiftGate::PhaseShiftGate(int activeQubit, double phase, std::vector<int> controlQubits) : PhaseShiftGate(activeQubit, phase)
{
    m_controlQubits = controlQubits;
}

RotationXGate::RotationXGate() {};
RotationXGate::RotationXGate(int activeQubit, double phi) 
{
    m_activeQubit = activeQubit; 
    m_theta = phi;
    m_matrix = {c(std::cos(phi/2), 0.0), c(0.0, -std::sin(phi/2)), c(0.0, -std::sin(phi/2)), c(std::cos(phi/2), 0.0)};
}
RotationXGate::RotationXGate(int activeQubit, double phi, std::vector<int> controlQubits) : RotationXGate(activeQubit, phi)
{
    m_controlQubits = controlQubits;
}

RotationYGate::RotationYGate() {};
RotationYGate::RotationYGate(int activeQubit, double phi) 
{
    m_activeQubit = activeQubit; 
    m_theta = phi;
    m_matrix = {c(std::cos(phi/2), 0.0), c(-std::sin(phi/2), 0.0), c(std::sin(phi/2), 0.0), c(std::cos(phi/2), 0.0)};
}
RotationYGate::RotationYGate(int activeQubit, double phi, std::vector<int> controlQubits) : RotationYGate(activeQubit, phi)
{
    m_controlQubits = controlQubits;
}

RotationZGate::RotationZGate() {};
RotationZGate::RotationZGate(int activeQubit, double phi) 
{
    m_activeQubit = activeQubit; 
    m_theta = phi;
    m_matrix = {c(std::polar(1.0,-phi/2)), c(0.0, 0.0), c(0.0, 0.0), c(std::polar(1.0,phi/2))};
}
RotationZGate::RotationZGate(int activeQubit, double phi, std::vector<int> controlQubits) : RotationZGate(activeQubit, phi)
{
    m_controlQubits = controlQubits;
}

SwapGate::SwapGate() {};
SwapGate::SwapGate(int activeQubit, int swapQubit) 
{
    m_activeQubit = activeQubit; 
    m_swapQubit = swapQubit;
}
SwapGate::SwapGate(int activeQubit, int swapQubit, std::vector<int> controlQubits) : SwapGate(activeQubit, swapQubit)
{
    m_controlQubits = controlQubits;
}
void SwapGate::act(std::vector<c> &qregister)
{
    bool controls_passed = true;
    std::vector<c> temp(qregister.size(), c(0.0, 0.0));
    for (int n=0; n<qregister.size(); ++n) 
    {
        for (auto cq : m_controlQubits) 
        {
            if (!(n % static_cast<int>(pow(2,cq)) >= pow(2,cq-1)))
            {
                controls_passed = false;
            }       
        }
        if (controls_passed)
        {
            int bit1 =  (n >> (m_activeQubit-1)) & 1;
            int bit2 =  (n >> (m_swapQubit-1)) & 1;
            int x = (bit1 ^ bit2);
            x = (x << (m_activeQubit-1)) | (x << (m_swapQubit-1));
            int result = n ^ x;
            temp[result] = qregister[n];              
        } else {
            temp[n] += qregister[n];    
        }
        controls_passed=true;
    }
    for (int n{}; n<qregister.size(); n++)
    {
        qregister[n] = temp[n];
    }
}