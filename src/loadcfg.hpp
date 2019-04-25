#ifndef LOADCFG_H
#define LOADCFG_H

typedef  std::map <std::string, std::string> cfgType;

void shit (const char* mens);
void loadCfg (const char* filename, cfgType* pcfg);
std::string getmac (const char* name);

#endif