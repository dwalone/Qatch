#ifndef DefaultGate_H
#define DefaultGate_H

#include "Gate.h"
#include <cmath>

class DefaultGate : public Gate
{
public:
	void setActive(int activeQubit);
	void setControl(std::vector<int> contolQubits);
protected:
	std::vector<int> m_controlQubits;
	int m_activeQubit;
};


class MatrixGate : public DefaultGate
{
public:
    void act(std::vector<c> &qregister);
protected:
    std::vector<std::complex<double>> m_matrix;
};

class IdentityGate : public MatrixGate
{
public:
	IdentityGate();
	IdentityGate(int activeQubit);
};

class HadamardGate : public MatrixGate
{
public:
	HadamardGate();
	HadamardGate(int activeQubit);
	HadamardGate(int activeQubit, std::vector<int> controlQubits);
};


class XGate : public MatrixGate
{
public:
    XGate();
	XGate(int activeQubit);
	XGate(int activeQubit, std::vector<int> controlQubits);
};


class YGate : public MatrixGate
{
public:
	YGate();
	YGate(int activeQubit);
	YGate(int activeQubit, std::vector<int> controlQubits);
};

class ZGate : public MatrixGate
{
public:
	ZGate();
	ZGate(int activeQubit);
	ZGate(int activeQubit, std::vector<int> controlQubits);
};

class PhaseShiftGate : public MatrixGate
{
public:
	PhaseShiftGate();
	PhaseShiftGate(int activeQubit, double phase);
	PhaseShiftGate(int activeQubit, double phase, std::vector<int> controlQubits);
protected:
	double m_phase = 0;
};

class RotationXGate : public MatrixGate
{
public:
	RotationXGate();
	RotationXGate(int activeQubit, double theta);
	RotationXGate(int activeQubit, double theta, std::vector<int> controlQubits);
protected:
	double m_theta = 0;
};

class RotationYGate : public MatrixGate
{
public:
	RotationYGate();
	RotationYGate(int activeQubit, double theta);
	RotationYGate(int activeQubit, double theta, std::vector<int> controlQubits);
protected:
	double m_theta = 0;
};

class RotationZGate : public MatrixGate
{
public:
	RotationZGate();
	RotationZGate(int activeQubit, double theta);
	RotationZGate(int activeQubit, double theta, std::vector<int> controlQubits);
protected:
	double m_theta = 0;
};

class SwapGate : public DefaultGate
{
public:
	SwapGate();
	SwapGate(int activeQubit, int swapQubit);
	SwapGate(int activeQubit, int swapQubit, std::vector<int> controlQubits);
	void act(std::vector<c> &qregister);
protected:
	int m_swapQubit = 0;
};

#endif