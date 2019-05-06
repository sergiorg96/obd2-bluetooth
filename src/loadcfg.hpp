#ifndef LOADCFG_HPP
#define LOADCFG_HPP

typedef  std::map <std::string, std::string> cfgType;

void shit (const char* mens);
void loadCfg (const char* filename, cfgType* pcfg);
std::string getmac (const char* name);

#endif