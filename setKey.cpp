#include "awPass.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

void wrongInput() {
    std::cout << "Usage: setKey <Key>";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) wrongInput();
    std::string input = argv[1];
    std::ofstream file("key.h");
    std::string file_text = "std::string key = \"" + hash(input) + "\";";
    file << file_text;
    file.close();
    std::cout << "Key set to " + input + ". Run\x1B[3m sudo make install\x1B[23m to implement.";
    return 0;
}
