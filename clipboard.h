#include <cstdio>

void string2clipboard(std::string s) {
    FILE* p = popen("wl-copy", "w");
    fwrite(s.data(), 1, s.size(), p);
    pclose(p);
}
