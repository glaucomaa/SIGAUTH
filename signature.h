#ifndef hash_h
#define hash_h
#include <string>
void addDigitalSignature(const std::string& filename); 
bool containsDigitalSignature(const std::string& filename); 
void removeDigitalSignature(const std::string& filename); 
std::string ComputeFileHashMD5(const std::string &filepath); 
int hexStringToLL(const std::string &hexString); 
#endif
