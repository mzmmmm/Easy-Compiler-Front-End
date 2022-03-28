#include "Utils.h"
#include <string>
size_t countnum(std::string src, std::string str) {
    size_t cnt = 0, fnd = 0;
    while ((fnd = src.find(str, fnd)) != std::string::npos) {
        cnt++; fnd+=str.length();
    }
    return cnt;
}