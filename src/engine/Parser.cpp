#include "Parser.h"

const double pi = acos(-1.0);
const std::string pi_str = "3.14159265358979";



Parser::Parser()
{
    m_symbol_map["init"]    = INITIALISE;
    m_symbol_map["def"]     = DEFINITION;
    m_symbol_map["endef"]   = END_DEFINITION;
    m_symbol_map["for"]     = FOR_LOOP;
    m_symbol_map["endfor"]  = END_FOR_LOOP;
    m_symbol_map["H"]       = HADAMARD;
    m_symbol_map["CH"]      = CONTROLLED_HADAMARD;
    m_symbol_map["X"]       = X;
    m_symbol_map["CX"]      = CONTROLLED_X;
    m_symbol_map["Y"]       = Y;
    m_symbol_map["CY"]      = CONTROLLED_Y;
    m_symbol_map["Z"]       = Z;
    m_symbol_map["CZ"]      = CONTROLLED_Z;
    m_symbol_map["P"]       = PHASE_SHIFT;
    m_symbol_map["CP"]      = CONTROLLED_PHASE_SHIFT;
    m_symbol_map["RX"]      = ROTATION_X;
    m_symbol_map["CRX"]     = CONTROLLED_ROTATION_X;
    m_symbol_map["RY"]      = ROTATION_Y;
    m_symbol_map["CRY"]     = CONTROLLED_ROTATION_Y;
    m_symbol_map["RZ"]      = ROTATION_Z;
    m_symbol_map["CRZ"]     = CONTROLLED_ROTATION_Z;
    m_symbol_map["SWAP"]    = SWAP;
    m_symbol_map["CSWAP"]   = CONTROLLED_SWAP;

    m_isInitialised = false;
    m_inDef = false;
    m_inLoop = false;
}

void Parser::parse(std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{
    int line_number = 0; 
    while (line_number<m_lines.size())  
    {
        ++line_number;
        parseLine(line_number, m_lines[line_number-1], gateList, nQ, qR); 
    } 
    pAssert(!m_inDef, "EOF - definition not closed", line_number);
    pAssert(!m_inLoop, "EOF - loop not closed", line_number);
}

void Parser::scanLines(std::string &filename)
{
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line))
    {
        m_lines.push_back(line);        
    }
}

void Parser::reset()
{
    m_isInitialised = false;
    m_symbol_map;
    for (auto it = m_defs.begin(); it != m_defs.end(); ++it)
    {
        m_symbol_map.erase(m_symbol_map.find(it->first));
    }
    m_defs.clear();
}

void Parser::parseLine(int &line_number, std::string &line, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{
    formatLine(line_number, line);
    std::istringstream iss(line);
    Symbol symbol;
    std::string symbolstr;
    symHandler(line_number, iss, symbol, symbolstr);
    initialChecksHandler(line_number, symbol);
    commandHandler(line_number, iss, symbol, symbolstr, gateList, nQ, qR);
}

void Parser::formatLine(int &line_number,std::string &line)
{
    size_t start = line.find_first_not_of(" \n\r\t\f\v");
    line = (start == std::string::npos) ? "" : line.substr(start);
    size_t end = line.find_last_not_of(" \n\r\t\f\v");
    line = (end == std::string::npos) ? "" : line.substr(0, end + 1);
    std::reverse(m_vars.begin(), m_vars.end());
    for (const std::pair<std::string, std::string>& var : m_vars) 
    {
        if (!var.first.compare(NESTED_FUNC_SPLIT)) {break;}
        line = replaceVar(line, var.first, var.second);
    }
    std::reverse(m_vars.begin(), m_vars.end());
}

void Parser::symHandler(int &line_number, std::istringstream &iss, Symbol &symbol, std::string &symbolstr)
{
    iss >> symbolstr;
    if (symbolstr.rfind("//", 0) == 0 || symbolstr.empty()) {symbol = SKIP;}
    else 
    {
        pAssert(m_symbol_map.count(symbolstr) > 0, "Symbol not found - '"+symbolstr+"'", line_number);
        symbol = m_symbol_map[symbolstr];
    }
}

void Parser::initialChecksHandler(int &line_number, Symbol &symbol)
{
    if (symbol >= IDENTITY && symbol <= CUSTOM) {
        if (m_inDef) {symbol = SKIP;} else {pAssert(m_isInitialised, "Circuit must be initialised", line_number);}  

    } else if (symbol==INITIALISE) {
        pAssert(!m_inDef, "init cannot be declared in definition", line_number);
        pAssert(!m_inLoop, "init cannot be declared in loop", line_number);
        pAssert(!m_isInitialised, "Circuit already initialised", line_number);

    } else if (symbol == DEFINITION) {
        pAssert(!m_inDef, "def cannot be declared in definition", line_number);
        pAssert(!m_inLoop, "def cannot be declared in loop", line_number);

    } else if (symbol == END_DEFINITION) {
        pAssert(!m_inLoop, "endef cannot be declared in loop", line_number);
        pAssert(m_inDef, "No function defined", line_number);   

    } else if (symbol == FOR_LOOP) {
        if (m_inDef) {symbol = SKIP;} else {pAssert(m_isInitialised, "Circuit must be initialised", line_number);}   

    } else if (symbol == END_FOR_LOOP) {
        if (m_inDef) {symbol = SKIP;} else {pAssert(m_isInitialised, "Circuit must be initialised", line_number); 
                                            pAssert(m_inLoop, "No loop defined", line_number);}  

    } else if (symbol == SKIP) {
        ;
    } else {
        pAssert(false, "Unknown initial checks error", line_number);
    }
}

void Parser::commandHandler(int &line_number, std::istringstream &iss, Symbol &symbol, std::string &symbolstr, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{

    if (
        symbol == HADAMARD ||
        symbol == X ||
        symbol == Y ||
        symbol == Z ||
        symbol == CONTROLLED_HADAMARD ||
        symbol == CONTROLLED_X ||
        symbol == CONTROLLED_Y ||
        symbol == CONTROLLED_Z
        ) 
    {
        defaultGate(line_number, iss, symbol, gateList, nQ);

    } else if (
        symbol == PHASE_SHIFT ||
        symbol == ROTATION_X ||
        symbol == ROTATION_Y ||
        symbol == ROTATION_Z ||
        symbol == CONTROLLED_PHASE_SHIFT ||
        symbol == CONTROLLED_ROTATION_X ||
        symbol == CONTROLLED_ROTATION_Y ||
        symbol == CONTROLLED_ROTATION_Z
        ) 
    {
        defaultAngleGate(line_number, iss, symbol, gateList, nQ);

    } else if (
        symbol == SWAP ||
        symbol == CONTROLLED_SWAP
        )
    {
        defaultMultiQubitGate(line_number, iss, symbol, gateList, nQ);

    } else if (symbol==CUSTOM) {
        customGate(line_number, iss, symbolstr, gateList, nQ, qR);
    
    } else if (symbol==INITIALISE) {
        initialise(line_number, iss, nQ, qR);

    } else if (symbol == DEFINITION) {
        definition(line_number, iss);

    } else if (symbol == END_DEFINITION) {
        endDefinition(line_number, iss);

    } else if (symbol == FOR_LOOP) {
        forLoop(line_number, iss, gateList, nQ, qR);

    } else if (symbol == END_FOR_LOOP) {
        endForLoop(line_number, iss);

    } else if (symbol == SKIP) {
        ;
    } else {
        pAssert(false, "Unknown command error", line_number);  
    }
}

void Parser::defaultGate(int &line_number, std::istringstream &iss, Symbol symbol, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ)
{
    // Initalise command variables
    int aq;
    parseQubit(line_number, aq, iss, nQ);
    switch (symbol)
    {
        case IDENTITY : gateList.push_back(std::make_unique<IdentityGate>(aq)); return;
        case HADAMARD : gateList.push_back(std::make_unique<HadamardGate>(aq)); return;
        case X :        gateList.push_back(std::make_unique<XGate>(aq)); return;
        case Y :        gateList.push_back(std::make_unique<YGate>(aq)); return;
        case Z :        gateList.push_back(std::make_unique<ZGate>(aq)); return;
        return;
    }
    std::vector<int> cqs;
    parseControlQubits(line_number,cqs, iss, nQ);
    switch (symbol)
    {
        case CONTROLLED_HADAMARD :  gateList.push_back(std::make_unique<HadamardGate>(aq, cqs)); return;
        case CONTROLLED_X :         gateList.push_back(std::make_unique<XGate>(aq, cqs)); return;
        case CONTROLLED_Y :         gateList.push_back(std::make_unique<YGate>(aq, cqs)); return;
        case CONTROLLED_Z :         gateList.push_back(std::make_unique<ZGate>(aq, cqs)); return;
        return;
    }
}

void Parser::defaultAngleGate(int &line_number, std::istringstream &iss, Symbol symbol, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ)
{
    int aq;
    double ph;
    parseQubit(line_number, aq, iss, nQ);
    parseAngle(line_number, ph, iss);
    switch (symbol)
    {
        case PHASE_SHIFT :  gateList.push_back(std::make_unique<PhaseShiftGate>(aq, ph)); return;
        case ROTATION_X :   gateList.push_back(std::make_unique<RotationXGate>(aq, ph)); return;
        case ROTATION_Y :   gateList.push_back(std::make_unique<RotationYGate>(aq, ph)); return;
        case ROTATION_Z :   gateList.push_back(std::make_unique<RotationZGate>(aq, ph)); return;
    }
    std::vector<int> cqs;
    parseControlQubits(line_number, cqs, iss, nQ);
    switch (symbol)
    {
        case CONTROLLED_PHASE_SHIFT :   gateList.push_back(std::make_unique<PhaseShiftGate>(aq, ph, cqs)); return;
        case CONTROLLED_ROTATION_X :    gateList.push_back(std::make_unique<RotationXGate>(aq, ph, cqs)); return;
        case CONTROLLED_ROTATION_Y :    gateList.push_back(std::make_unique<RotationYGate>(aq, ph, cqs)); return;
        case CONTROLLED_ROTATION_Z :    gateList.push_back(std::make_unique<RotationZGate>(aq, ph, cqs)); return;
    }
}

void Parser::defaultMultiQubitGate(int &line_number, std::istringstream &iss, Symbol symbol, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ)
{
    int aq;
    int q2;
    parseQubit(line_number, aq, iss, nQ);
    parseQubit(line_number, q2, iss, nQ);
    switch (symbol)
    {
        case SWAP : gateList.push_back(std::make_unique<SwapGate>(aq, q2)); return;
    }
    std::vector<int> cqs;
    parseControlQubits(nQ, cqs, iss, nQ);
    switch (symbol)
    {
        case CONTROLLED_SWAP : gateList.push_back(std::make_unique<SwapGate>(aq, q2)); return;        
    }
}

void Parser::customGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{
    std::vector<std::unique_ptr<Gate>> gates;
    std::string var;
    std::vector<std::string> func_vars;
    while (iss>>var) 
    {
        func_vars.push_back(var);
    }
    pAssert(func_vars.size() == m_defs[sym].variables.size(), "invalid number of vars", line_number);
    m_vars.push_back({NESTED_FUNC_SPLIT, NESTED_FUNC_SPLIT});
    for (int i=0; i<func_vars.size(); ++i)
    {
        m_vars.push_back({m_defs[sym].variables[i], func_vars[i]});
    }
    for (int cg_line_number=m_defs[sym].def_line+1; cg_line_number<m_defs[sym].endef_line; ++cg_line_number)
    {
        parseLine(cg_line_number, m_lines[cg_line_number-1], gates, nQ, qR);
    }
    gateList.push_back(std::make_unique<CustomGate>(sym, std::move(gates)));
    for (int i=0; i<func_vars.size(); ++i)
    {
        m_vars.pop_back();
    }
    m_vars.pop_back();
}

void Parser::initialise(int &line_number, std::istringstream &iss, int &nQ, std::vector<c> &qR)
{
    int n;
    std::string check_extra;
    // Special Checks
    pAssert(!(!(iss>>n)), "no number of qubits given", line_number);
    pAssert(n>0, "number of qubits must be greater than zero", line_number);
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    // Command action
    nQ = n;
    qR.resize(pow(2,n));
    qR[0]=c(1.0, 0.0);
    // Set environment variables
    m_isInitialised = true;
}

void Parser::definition(int &line_number, std::istringstream &iss)
{
    std::vector<std::string> def_vars;
    std::string def_name;
    DefinitionData data;
    std::string var;
    pAssert(!(!(iss>>def_name)), "empty def name", line_number);
    pAssert(m_symbol_map.find(def_name) == m_symbol_map.end(), "Name already defined - '"+def_name+"'", line_number);
    pAssert(!def_name.empty() && def_name.find_first_not_of("0123456789") != std::string::npos, "Invalid name, name can't be a number - '"+def_name+"'", line_number);
    while (iss>>var)
        {
            pAssert(var.rfind("$", 0) == 0, "var doesnt start with $", line_number);
            def_vars.push_back(var);
        }
    data.def_line = line_number;
    data.variables = def_vars;
    m_defs.emplace(def_name, data);
    m_currentDefName = def_name; 
    m_symbol_map[def_name] = CUSTOM;
    m_inDef = true;
}

void Parser::endDefinition(int &line_number, std::istringstream &iss)
{
    std::string check_extra;
    iss>>check_extra;
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    m_defs[m_currentDefName].endef_line = line_number;
    m_inDef = false;
}

void Parser::forLoop(int &line_number, std::istringstream &iss, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR)
{
    m_inLoop = true;
    int loop_num = m_loops.size();
    std::string line;
    int lp_line_number;
    std::string var;
    int start_idx;
    int end_idx;
    char delimeter;
    pAssert(!(!(iss>>var)), "No loop variable given", line_number);
    pAssert(var.rfind("$", 0) == 0, "var doesnt start with $", line_number);
    pAssert(!(!(iss>>start_idx)), "Integer loop start must be given", line_number);
    pAssert(!(!(iss>>delimeter)), "Start and End must be separated by ':'", line_number);
    pAssert(delimeter == ':', "Start and End must be separated by ':'", line_number);
    pAssert(!(!(iss>>end_idx)), "Integer loop start must be given", line_number);
    LoopData data;
    data.loop_line = line_number;
    data.endloop_line = m_lines.size();
    std::vector<int> loopCounter(end_idx-start_idx+1);
    std::iota(std::begin(loopCounter), std::end(loopCounter), start_idx);
    data.loop_counter = loopCounter;
    m_loops.push_back(data);
    int loop_idx = m_vars.size();
    m_vars.push_back({var, std::to_string(m_loops[loop_num].loop_counter[0])});
    for (int i=0; i<loopCounter.size(); ++i)
    {
        m_vars[loop_idx].second = std::to_string(m_loops[loop_num].loop_counter[i]);
        for (lp_line_number=m_loops[loop_num].loop_line+1; lp_line_number<=m_loops[loop_num].endloop_line; ++lp_line_number)
        {
            line_number = lp_line_number;
            line = m_lines[line_number-1];        
            parseLine(line_number, line, gateList, nQ, qR);
        }
    }
    m_loops.pop_back();
    m_inLoop = (m_loops.size()!=0); 
    line_number = lp_line_number;
    m_vars.pop_back();   
}

void Parser::endForLoop(int &line_number, std::istringstream &iss)
{
    std::string check_extra;
    iss>>check_extra;
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    m_loops.back().endloop_line = line_number;
}

std::string Parser::replaceVar(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void Parser::parseControlQubits(int &line_number, std::vector<int> &cqs, std::istringstream &iss, int &nQ)
{
    double result;
    int cq;
    std::string cqstr;
    std::string cdstr;
    iss>>cdstr;
    pAssert(cdstr=="|", "delimeter needs to be |", line_number);
    while (iss>>cqstr)
    {
        result = eval(cqstr, line_number);
        pAssert(trunc(result)==result, "Control qubit number must be integer", line_number);
        cq = (int) result;
        pAssert(cq>0 && cq<=nQ, "Control qubit numbers must be between 1 and "+std::to_string(nQ), line_number); 
        pAssert(std::find(cqs.begin(), cqs.end(), cq) == cqs.end(), "Repeated control qubit - "+std::to_string(cq), line_number);   
        cqs.push_back((int) result);
    }
    pAssert(cqs.size()>0, "Requires control qubit(s)", line_number);
}

void Parser::parseQubit(int &line_number, int &q, std::istringstream &iss, int &nQ)
{
    double result;
    std::string qstr;
    pAssert(!(!(iss>>qstr)), "Requires active qubit", line_number);
    result = eval(qstr, line_number);
    pAssert(trunc(result)==result, "Active qubit number must be integer", line_number);
    q = (int) result; 
    pAssert(q>0 && q<=nQ, "Active qubit number must be between 1 and "+std::to_string(nQ), line_number);       
}

void Parser::parseAngle(int &line_number, double &phi, std::istringstream &iss)
{
    std::string astr;
    pAssert(!(!(iss>>astr)), "Requires angle to be given", line_number);
    phi = eval(astr, line_number);
}

double Parser::eval(std::string expr, int &line_number)
{
    pAssert(expr.find('$') == std::string::npos, "Undefined variable", line_number);
    std::string xxx;
    for (int i = 0; i < expr.length(); i++)
    {
        if (expr[i] != ' ')
        {
            xxx += expr[i];
        }
    }
    xxx = replaceVar(xxx, "pi", pi_str);
    std::string tok = "";
    for (int i = 0; i < xxx.length(); i++)
    {
        if (xxx[i] == '(')
        {
            int iter = 1;
            std::string token;
            i++;
            while (true)
            {
                if (xxx[i] == '(')
                {
                    iter++;
                } else if (xxx[i] == ')')
                {
                    iter--;
                    if (iter == 0)
                    {
                        i++;
                        break;
                    }
                }
                token += xxx[i];
                i++;
            }
            tok += std::to_string(eval(token, line_number));
        }
        tok += xxx[i];
    }

    for (int i = 0; i < tok.length(); i++)
    {
        if (tok[i] == '+')
        {
            return eval(tok.substr(0, i), line_number) + eval(tok.substr(i+1, tok.length()-i-1), line_number);
        } else if (tok[i] == '-')
        {
            if (tok.substr(0, i).length() != 0 && tok[i - 1] != '*' && tok[i - 1] != '/') 
            {
                return eval(tok.substr(0, i), line_number) + eval("-" + tok.substr(i + 1, tok.length() - i - 1), line_number); 
            }		
        }
    }

    for (int i = 0; i < tok.length(); i++)
    {
        if (tok[i] == '*')
        {
            return eval(tok.substr(0, i), line_number) * eval(tok.substr(i+1, tok.length()-i-1), line_number);
        } else if (tok[i] == '/')
        {
            return eval(tok.substr(0, i), line_number) / eval(tok.substr(i+1, tok.length()-i-1), line_number);
        }
    }
    return std::stod(tok.c_str());
}

void Parser::pAssert(bool condition, std::string statement, int line_number)
{
    if (!condition)
    {
        std::cerr<<statement<<" (line "<<line_number<<")"<<std::endl;
        exit(1);
    }
}