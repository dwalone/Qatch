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

enum Symbol {
    // Keywords
    INITIALISE,
    DEFINITION,
    END_DEFINITION,
    FOR_LOOP,
    END_FOR_LOOP,
    // Gates
    IDENTITY,
    HADAMARD,
    CONTROLLED_HADAMARD,
    X,
    CONTROLLED_X,
    Y,
    CONTROLLED_Y,
    Z,
    CONTROLLED_Z,
    PHASE_SHIFT,
    CONTROLLED_PHASE_SHIFT,
    ROTATION_X,
    CONTROLLED_ROTATION_X,
    ROTATION_Y,
    CONTROLLED_ROTATION_Y,
    ROTATION_Z,
    CONTROLLED_ROTATION_Z,
    SWAP,
    CONTROLLED_SWAP,
    CUSTOM,
    // Other
    SKIP
};

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
    void readFile(std::string &filename);
    void parseLine(int &line_number, std::string &line, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR);
    void formatLine(int &line_number,std::string &line);
    void symHandler(int &line_number, std::istringstream &iss, Symbol &symbol, std::string &symbolstr);
    void initialChecksHandler(int &line_number, Symbol &symbol);
    void commandHandler(int &line_number, std::istringstream &iss, Symbol &symbol, std::string &symbolstr, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR);
    void initialise(int &line_number, std::istringstream &iss, int &nQ, std::vector<c> &qR);
    void definition(int &line_number, std::istringstream &iss);
    void endDefinition(int &line_number, std::istringstream &iss);
    void forLoop(int &line_number, std::istringstream &iss, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR);
    void endForLoop(int &line_number, std::istringstream &iss);
    void defaultGate(int &line_number, std::istringstream &iss, Symbol symbol, std::vector<std::unique_ptr<Gate>> &gateList, int nQ);
    void customGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR);
    std::string replaceVar(std::string str, const std::string& from, const std::string& to);
    void parseControlQubits(int &line_number, std::vector<int> &cqs, std::istringstream &iss);
    void parseQubit(int &line_number, int &q, std::istringstream &iss);
    void parseAngle(int &line_number, double &phi, std::istringstream &iss);
    
    double eval(std::string expr);
    void pAssert(bool condition, std::string statement, int line_number);
    ~Parser(){};

private:
    std::vector<std::string> m_lines;
    std::map<std::string, DefinitionData> m_defs;
    std::vector<LoopData> m_loops;
    std::vector<std::pair<std::string,std::string>> m_vars;
    bool m_isInitialised;
    bool m_inDef;
    bool m_inLoop;
    bool m_lineExecuted;
    std::string m_currentDefName;
    std::map<std::string, Symbol> m_symbol_map;
};

#endif