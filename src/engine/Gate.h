#ifndef Gate_H
#define Gate_H

#include<vector>
#include<string>
#include<memory>
#include<complex>

typedef std::complex<double> c;

class Gate
{
public:
	virtual void act(std::vector<c> &qregister) = 0;
	std::string name() {return m_name;}
protected:
	std::string m_name;
};

#endif

