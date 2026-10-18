#include "awPass.h"
#include "key.h"
#include "clipboard.h"
#include <iostream>

void wrongInput() {
    std::cout << "Usage:\n awPass <Key> <Name of Service>\n"
        << "OR\n"
        <<"awPass <Key> <OldKey> <Name of Service>\n";
    std::exit(1);
}

void wrongKey() {
    std::cout << "Wrong Key.\n";
    wrongInput();
}

int main(int argc, char* argv[]) {

    if(argc == 3) {
        std::string Typedkey = argv[1];
        if(hash(Typedkey) != key) wrongKey();
        std::string service = argv[2];
        string2clipboard(hash(Typedkey + service));
        std::cout << "Password to " << service << " copied to clipboard";
    }
    else if (argc == 4) {
        std::string Typedkey = argv[1];
        if(hash(Typedkey) != key) wrongKey();
        std::string oldKey = argv[2];
        std::string service = argv[3];
        string2clipboard(hash(oldKey + service));
        std::cout << "Old password to " << service << " copied to clipboard";
    }

    else
        wrongInput();

    return 0;
}
