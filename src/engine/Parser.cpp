#include "Parser.h"

const double pi = acos(-1.0);
const std::string pi_str = "3.141592653589793238462643383279502884197169399375";

Parser::Parser()
{
    m_keywordTextLib = {
        "init",
        "def",
        "endef",
        "for",
        "endfor"
    };
    m_gateTextLib = {
        "H",
        "CH",
        "X",
        "CX",
        "Y",
        "CY",
        "Z",
        "CZ",
        "P",
        "CP",
        "RX",
        "CRX",
        "RY",
        "CRY",
        "RZ",
        "CRZ",
        "SWAP",
        "CSWAP"
    };
    m_isInitialised = false;
    m_inDef = false;
    m_inLoop = false;
    m_lineExecuted = false;
}

void Parser::parse(std::string filename, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{
    m_filename = filename;
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line))
    {
        m_lines.push_back(line);        
    }
    int line_number = 0; 
    while (line_number<m_lines.size())  
    {
        ++line_number;
        parseLine(line_number, m_lines[line_number-1], gateList, nQ, qR); 
    } 

}

void Parser::parseLine(int &line_number, std::string &line, std::vector<std::unique_ptr<Gate>> &gateList, int &nQ, std::vector<c> &qR)
{
    m_lineExecuted = false;
    std::string sym;
    line = rtrim(ltrim(line));
    std::istringstream iss(line);
    iss>>sym;
    checkInit(line_number, iss, sym, nQ, qR);
    checkDef(line_number, iss, sym);
    checkEndef(line_number, iss, sym);
    checkFor(line_number, iss, sym, gateList, nQ, qR);
    checkEndfor(line_number, iss, sym);
    checkNormalGate(line_number, iss, sym, gateList, nQ);
    checkCustomGate(line_number, iss, sym, gateList, nQ, qR);
    checkComment(line_number, sym);
    checkEmpty(line_number, sym);
    pAssert(m_lineExecuted, "Unknown symbol - '"+sym+"'", line_number);   
}

void Parser::checkInit(int &line_number, std::istringstream &iss, std::string &sym, int &nQ, std::vector<c> &qR)
{
    if (sym != "init") {return;}
    int n;
    std::string check_extra;
    pAssert(!m_inDef, "init cannot be declared in definition", line_number);
    pAssert(!m_inLoop, "init cannot be declared in loop", line_number);
    pAssert(!m_isInitialised, "Circuit already initialised", line_number);
    pAssert(!(!(iss>>n)), "no number of qubits given", line_number);
    pAssert(n>0, "number of qubits must be greater than zero", line_number);
    iss>>check_extra;
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    nQ = n;
    qR.resize(pow(2,n));
    qR[0]=c(1.0, 0.0);
    m_isInitialised = true;
    m_lineExecuted = true;

}

void Parser::checkDef(int &line_number, std::istringstream &iss, std::string &sym)
{
    if (sym != "def") {return;}
    pAssert(!m_inDef, "def cannot be declared in definition", line_number);
    pAssert(!m_inLoop, "def cannot be declared in loop", line_number);
    std::vector<std::string> def_vars;
    std::string def_name;
    pAssert(!(!(iss>>def_name)), "empty def name", line_number);
    checkValidName(line_number, def_name);
    DefinitionData data;
    data.def_line = line_number;
    std::string var;
    while (iss>>var)
        {
            pAssert(var.rfind("$", 0) == 0, "var doesnt start with $", line_number);
            def_vars.push_back(var);
        }
    data.variables = def_vars;
    m_defs.emplace(def_name, data);
    m_currentDefName = def_name; 
    m_inDef = true;
    m_lineExecuted = true;
}

void Parser::checkEndef(int &line_number, std::istringstream &iss, std::string &sym)
{
    if (sym != "endef") {return;}
    pAssert(!m_inLoop, "endef cannot be declared in loop", line_number);
    pAssert(m_inDef, "No function defined", line_number);
    std::string check_extra;
    iss>>check_extra;
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    m_defs[m_currentDefName].endef_line = line_number;
    m_inDef = false;
    m_lineExecuted = true;
}

void Parser::checkNormalGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ)
{
    if (m_inDef) {m_lineExecuted=true; return;}
    if (std::find(m_gateTextLib.begin(), m_gateTextLib.end(), sym) == m_gateTextLib.end()) {return;}
    pAssert(m_isInitialised, "Circuit must be initialised", line_number);
    int aq;
    int sq;
    double ph;
    std::vector<int> cqs;
    if (sym=="H") {parseQubit(nQ, aq, iss); gateList.push_back(std::make_unique<HadamardGate>(aq));}
    else if (sym=="X") {parseQubit(nQ, aq, iss); gateList.push_back(std::make_unique<XGate>(aq));} 
    else if (sym=="Y") {parseQubit(nQ, aq, iss); gateList.push_back(std::make_unique<YGate>(aq));} 
    else if (sym=="Z") {parseQubit(nQ, aq, iss); gateList.push_back(std::make_unique<ZGate>(aq));}  
    else if (sym=="P") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); gateList.push_back(std::make_unique<PhaseShiftGate>(aq, ph));}
    else if (sym=="RX") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); gateList.push_back(std::make_unique<RotationXGate>(aq, ph));}
    else if (sym=="RY") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); gateList.push_back(std::make_unique<RotationYGate>(aq, ph));}
    else if (sym=="RZ") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); gateList.push_back(std::make_unique<RotationZGate>(aq, ph));}
    else if (sym=="SWAP") {parseQubit(nQ, aq, iss); parseQubit(nQ, sq, iss); gateList.push_back(std::make_unique<SwapGate>(aq, sq));}
    else if (sym=="CH") {parseQubit(nQ, aq, iss); parseControlQubits(nQ,cqs, iss); gateList.push_back(std::make_unique<HadamardGate>(aq, cqs));}
    else if (sym=="CX") {parseQubit(nQ, aq, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<XGate>(aq, cqs));} 
    else if (sym=="CY") {parseQubit(nQ, aq, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<YGate>(aq, cqs));} 
    else if (sym=="CZ") {parseQubit(nQ, aq, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<ZGate>(aq, cqs));} 
    else if (sym=="CP") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<PhaseShiftGate>(aq, ph, cqs));}
    else if (sym=="CRX") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<RotationXGate>(aq, ph, cqs));}
    else if (sym=="CRY") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<RotationYGate>(aq, ph, cqs));}
    else if (sym=="CRZ") {parseQubit(nQ, aq, iss); parseAngle(nQ, ph, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<RotationZGate>(aq, ph, cqs));}
    else if (sym=="CSWAP") {parseQubit(nQ, aq, iss); parseQubit(nQ, sq, iss); parseControlQubits(nQ, cqs, iss); gateList.push_back(std::make_unique<SwapGate>(aq, sq, cqs));} 
    else {return;}   
    m_lineExecuted = true;
}

void Parser::checkCustomGate(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR)
{
    if (m_inDef) {m_lineExecuted=true; return;}
    if (m_defs.count(sym) == 0) {return;}
    pAssert(m_isInitialised, "Circuit must be initialised", line_number);
    std::vector<std::unique_ptr<Gate>> gates;
    std::string line;
    int var;
    std::vector<int> func_vars;
    int i;
    while (iss>>var) 
    {
        func_vars.push_back(var);
    }
    pAssert(func_vars.size() == m_defs[sym].variables.size(), "invalid number of vars", line_number);
    for (int cg_line_number=m_defs[sym].def_line+1; cg_line_number<m_defs[sym].endef_line; ++cg_line_number)
    {
        line = m_lines[cg_line_number-1];
        i = 0;
        for (const std::string& tbr : m_defs[sym].variables) 
        {
            line = replaceAll(line, tbr, std::to_string(func_vars[i]));
            i = i + 1;
        }
        parseLine(cg_line_number, line, gates, nQ, qR);
    }
    gateList.push_back(std::make_unique<CustomGate>(sym, std::move(gates)));
    m_lineExecuted = true;
}

void Parser::checkComment(int &line_number, std::string &sym)
{
    if (sym.rfind("//", 0) == 0) {m_lineExecuted=true; return;}
}

void Parser::checkEmpty(int &line_number, std::string &sym)
{
    if (sym.empty()) {m_lineExecuted = true; return;}
}

void Parser::checkValidName(int &line_number, std::string name)
{
    pAssert(std::find(m_keywordTextLib.begin(), m_keywordTextLib.end(), name) == m_keywordTextLib.end(), "Invalid name, already a keyword - '"+name+"'", line_number);
    pAssert(std::find(m_gateTextLib.begin(), m_gateTextLib.end(), name) == m_gateTextLib.end(), "invalid name, already a gate - '"+name+"'", line_number);
    pAssert(!m_defs.count(name), "Definition name already exists - '"+name+"'", line_number);
    pAssert(!name.empty() && name.find_first_not_of("0123456789") != std::string::npos, "Invalid name, name can't be a number - '"+name+"'", line_number);
}

void Parser::checkFor(int &line_number, std::istringstream &iss, std::string &sym, std::vector<std::unique_ptr<Gate>> &gateList, int nQ, std::vector<c> &qR)
{
    if (m_inDef) {m_lineExecuted=true; return;}
    if (sym != "for") {return;}
    m_inLoop = true;
    int loop_num = m_loops.size();
    std::string line;
    int lp_line_number;
    pAssert(m_isInitialised, "Circuit must be initialised", line_number);
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
    std::reverse(std::begin(loopCounter), std::end(loopCounter));
    data.loop_counter = loopCounter;
    m_loops.push_back(data);
    for (int i=0; i<loopCounter.size(); ++i)
    {
        for (lp_line_number=m_loops[loop_num].loop_line+1; lp_line_number<m_loops[loop_num].endloop_line; ++lp_line_number)
        {
            line = m_lines[lp_line_number-1];        
            line = replaceAll(line, var, std::to_string(m_loops[loop_num].loop_counter[i]));
            parseLine(lp_line_number, line, gateList, nQ, qR);
        }

    }
    m_loops.pop_back();
    m_inLoop = (m_loops.size()!=0);
    line_number = lp_line_number;
    m_lineExecuted = true;   
}

void Parser::checkEndfor(int &line_number, std::istringstream &iss, std::string &sym)
{
    if (sym != "endfor") {return;}
    pAssert(m_inLoop, "No loop defined", line_number);
    std::string check_extra;
    iss>>check_extra;
    pAssert(!(iss>>check_extra), "invalid syntax", line_number);
    m_loops.back().endloop_line = line_number;
    m_lineExecuted = true;
}





std::string Parser::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string Parser::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Parser::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void Parser::parseControlQubits(int &line_number, std::vector<int> &cqs, std::istringstream &iss)
{
    int cq;
    char cd;
    iss>>cd;
    if (cd != '|')
    {
        std::cerr<<"delimeter needs to be |"<<std::endl;
        exit(1);
    } else {
        while (iss>>cq)
        {
            cqs.push_back(cq);
        }
    }
}

void Parser::parseQubit(int &line_number, int &q, std::istringstream &iss)
{
    double result;
    std::string qstr;
    pAssert(!(!(iss>>qstr)), "Requires qubit to be given", line_number);
    result = eval(qstr);
    pAssert(trunc(result)==result, "qubit number must be integer", line_number);
    q = (int) result;        
}

void Parser::parseAngle(int &line_number, double &phi, std::istringstream &iss)
{
    std::string astr;
    pAssert(!(!(iss>>astr)), "Requires angle to be given", line_number);
    phi = eval(astr);
}

double Parser::eval(std::string expr)
{
    std::string xxx;
    for (int i = 0; i < expr.length(); i++)
    {
        if (expr[i] != ' ')
        {
            xxx += expr[i];
        }
    }
    xxx = replaceAll(xxx, "pi", pi_str);
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
            tok += std::to_string(eval(token));
        }
        tok += xxx[i];
    }

    for (int i = 0; i < tok.length(); i++)
    {
        if (tok[i] == '+')
        {
            return eval(tok.substr(0, i)) + eval(tok.substr(i+1, tok.length()-i-1));
        } else if (tok[i] == '-')
        {
            if (tok.substr(0, i).length() != 0 && tok[i - 1] != '*' && tok[i - 1] != '/') 
            {
                return eval(tok.substr(0, i)) + eval("-" + tok.substr(i + 1, tok.length() - i - 1)); 
            }		
        }
    }

    for (int i = 0; i < tok.length(); i++)
    {
        if (tok[i] == '*')
        {
            return eval(tok.substr(0, i)) * eval(tok.substr(i+1, tok.length()-i-1));
        } else if (tok[i] == '/')
        {
            return eval(tok.substr(0, i)) / eval(tok.substr(i+1, tok.length()-i-1));
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