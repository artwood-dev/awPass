#include <string>
#include <array>
#include <vector>

std::array<char, 64> characters = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '@', '#'
};

std::vector<int> n_service(const std::string& service) {
    int l = service.length();
    std::vector<int> n(l, -1);

    for (int j = 0; j < l; j++)
        for (int i = 0; i < 64; i++)
            if (service[j] == characters[i])
                n[j] = i;

    return n;
}

std::array<bool, 6> toBinary(int number) {
    std::array<bool,6> bin;
    for (int i =5; i >=0; i--) {
        bin[i] = number % 2;
        number = number / 2;
    }
    return bin;
}

int toNumber(std::array<bool, 6> bin) {
    return
        (int)bin[0]*32 +
        (int)bin[1]*16 +
        (int)bin[2]*8 +
        (int)bin[3]*4 +
        (int)bin[4]*2 +
        (int)bin[5]*1;
}

namespace shift {
    std::array<bool,6> left(std::array<bool,6> bin, int count) {
        for (int x = 0; x < count; x++) {
            bool tmp = bin[0];
            for(int i = 0; i < 5; i++)
                bin[i] = bin[i+1];
            bin[5] = tmp;
        }
        return bin;
    }

    std::array<bool,6> right(std::array<bool,6> bin, int count) {
        for (int x = 0; x < count; x++) {
            bool tmp = bin[5];
            for(int i = 5; i > 0; i--)
                bin[i] = bin[i-1];
            bin[0] = tmp;
        }
        return bin;
    }
}

std::vector<std::array<bool, 6>> mix_neighbours(std::vector<std::array<bool, 6>> org) {
    std::vector<std::array<bool, 6>> result = org;
    int next;
    int prev;
    for (int i = 0; i < 64; i++) {
        switch (i) {
            case 0:
                prev = 63;
                next = 1;
                break;
            case 63:
                prev = 62;
                next = 0;
                break;
            default:
                next = i+1;
                prev = i-1;
                break;
        }
        for (int j = 0; j < 6; j++) {
            bool tmp;
            if (org[i][j] != org[next][j]) tmp = true;
            else tmp = false;
            if (tmp != org[prev][j]) tmp = true;
            else tmp = false;
            result[i][j] = tmp;
        }
    }
    return result;
}

std::vector<std::array<bool, 6>> reverse(std::vector<std::array<bool, 6>> org) {
    std::vector<std::array<bool, 6>> result;
    for(int i = org.size() - 1; i >= 0; i--)
        result.push_back(org[i]);
    return result;
}

std::string hash(std::string service) {
    //1 - Convert the Service to a bunch of numbers
    std::vector<int> numbers = n_service(service);

    //2 - Then into binary
    std::vector<std::array<bool,6>> numbers_binary;
    int l = numbers.size();
    for (int i = 0; i < l; i++)
        numbers_binary.push_back(toBinary(numbers[i]));

    //3 - Populate to 64 elements
    for (int i = 0; i < (64-l); i++)
        numbers_binary.push_back(numbers_binary[i % l]);

    //4 - Move bits around
    for (int i = 0; i < 64; i++)
        if(i % 2 == 0) numbers_binary[i] = shift::left(numbers_binary[i], i);
        else numbers_binary[i] = shift::right(numbers_binary[i], i);

    //5 - Mix Neighbours
    numbers_binary = mix_neighbours(numbers_binary);

    //6 - Reverse the result
    numbers_binary = reverse(numbers_binary);

    //7 - Move bits around again
    for (int i = 0; i < 64; i++)
        if(i % 2 == 0) numbers_binary[i] = shift::left(numbers_binary[i], i);
        else numbers_binary[i] = shift::right(numbers_binary[i], i);
    
    //8 - Mix Neighbours again
    numbers_binary = mix_neighbours(numbers_binary);

    //9 - Take the characters and use the resulting hash to swap indices
    for (int i = 0; i < 64; i++) {
        int n = toNumber(numbers_binary[i]);
        int tmp = characters[i];
        characters[i] = characters[n];
        characters[n] = tmp;
    }

    std::string result(characters.data(),characters.size());
    return result;
}
