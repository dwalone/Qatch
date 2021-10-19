#ifndef Parser_H
#define Parser_H

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <numeric>
#include "Gate.h"
#include "CustomGate.h"
#include "DefaultGate.h"

struct DefinitionData
{
    int def_line;
    int endef_line;
    std::vector<std::string> variables;
};

struct LoopData
{
    int loop_line;
    int endloop_line;
    std::string var;
    std::vector<int> loop_counter;
};

class Parser
{
public:
    Parser();
    void parse(std::string filename, std::vector<std::unique_ptr<Gate>> &m_gateList, int &nQ, std::vector<c> &qR);
    std::string ltrim(const std::string &s);
    std::string rtrim(const std::string &s);
    std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    void parseControlQubits(int &line_number, std::vector<int> &cqs, std::istringstream &iss);
    void parseQubit(int &line_number, int &q, std::istringstream &iss);
    void parseAngle(int &line_number, double &phi, std::istringstream &iss);
    void checkValidName(int &line_number, std::string name);
    void parseLine(int &line_number, std::string &line, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR);
    void checkInit(int &line_number, std::istringstream &iss, std::string &sym, int &nQ, std::vector<c> &qR);
    void checkEndef(int &line_number, std::istringstream &iss, std::string &sym);
    void checkDef(int &line_number, std::istringstream &iss, std::string &sym);
    void checkFor(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR);
    void checkEndfor(int &line_number, std::istringstream &iss, std::string &sym);
    void checkNormalGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ);
    void checkCustomGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR);
    void checkComment(int &line_number, std::string &sym);
    void checkEmpty(int &line_number, std::string &sym);
    double eval(std::string expr);
    void pAssert(bool condition, std::string statement, int line_number);
    ~Parser(){};

private:
    std::string m_filename;
    std::vector<std::string> m_keywordTextLib;
    std::vector<std::string> m_gateTextLib;
    std::vector<std::string> m_lines;
    std::map<std::string, DefinitionData> m_defs;
    std::vector<LoopData> m_loops;
    bool m_isInitialised;
    bool m_inDef;
    bool m_inLoop;
    bool m_lineExecuted;
    bool m_assert;
    std::string m_currentDefName;
};

#endif