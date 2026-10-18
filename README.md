# awPass

A tiny password manager

---
## Requirements
The code uses `wl-copy`, as well as `pipe`. So, the requirements are basically a primary use of these systems.
* A linux machine with an active Wayland Compositor
* `wl-clipboard` as the clipboard handler
## Usage
`awPass` creates a random string based on an input string. So, passwords are never saved but always regenerated from scratch.
* Use `./setKey <Key>` to set a key to the password manager.
* Rebuild/Install awPass to set the key you entered as the default key
* Use `awPass <Key> <Name of Service>` to generate a password for your service.
> Example
`awPass 69-420 webflyx` generates a password for `webflyx` based on `69-420`.
`setKey 420-69` changes the key from `69-420` to `420-69`.
Then Rebuild
To get the old password run `awPass 420-69 69-420 webflyx`.
Then go to `webflyx`, change your password.
Use `awPass 420-60 webflyx` to get the new password.

## Installation
1. Run `make set_key` to generate the setKey binary.
2. Run `.setKey <Key>` to set a key.
3. Run `make` to create a binary in the current directory or `make install` to install awPass to your system.
> Note `./awPass` if you used `make` or `awPass` of you used `make install`.

4. Run `awPass <Key> <Name of Service>` to generate a password for `Service`.
## High-Level Overview
1. `setKey` simply creates a file `key.h` which is imported as a module in `main.cpp`.
`key.h` only has one line.
```
std::string key = "";
```
`setKey <Key>` runs `hash(Key)`; and stores the hash in `key.h`.
2. In `main.cpp` (the code which runs upon running `awPass <Key> <Service>`), first `hash(Key)` is calculated and checked against `key` from `key.h`.
3. If you ran `awPass <Key> <Service>` then `hash(Key+Service)` is copied to clipboard using `wl-copy < `.
4. If you ran `awPass <Key> <OldKey> <Service>` then `hash(OldKey+Service)` is copied to the clipboard.
## Low Level Overview
### The Pipe
A basic function what simply copies a string into the clipboard using `wl-copy`

*clipboard.h*
```
void string2clipboard(std::string s) {
    FILE* p = popen("wl-copy", "w");
    fwrite(s.data(), 1, s.size(), p);
    pclose(p);
}
```
### Setting the key
Loads the `key.h` file.
*setKey.cpp*
```
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
```
Clearly `hash(std::string str)` does all the heavy-lifting here.
### The Password Manager
Really more of a consistent password generator, but who cares.

*main.cpp*
```
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
```
### The Character set
The way the hash works is not by first generating an array of 64 numbers each between 0-63. Then a predefined character set is taken and then each character is swapped with the random number generated from the hash. This way, one can always keep track of exactly which symbols are being used in their password as well as ensure all symbols are being used in the password.
> All passwords generated are simply a different purmutation of the original character set.

The character set I am using here is given by,
```
std::array<char, 64> characters = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '@', '#'
};
```
### The hash
Really, the whole reason why I made this project in the first place.
```
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
```

1. Converting a string into numbers based on the character set.
```
std::vector<int> n_service(const std::string& service) {
    int l = service.length();
    std::vector<int> n(l, -1);

    for (int j = 0; j < l; j++)
        for (int i = 0; i < 64; i++)
            if (service[j] == characters[i])
                n[j] = i;

    return n;
}
```
2. Since 6-bit ints are not an existing datatype, bitwise operators do not apply and we will wave to make our own.
*Converting into Binary*
```
std::array<bool, 6> toBinary(int number) {
    std::array<bool,6> bin;
    for (int i =5; i >=0; i--) {
        bin[i] = number % 2;
        number = number / 2;
    }
    return bin;
}
```
*The Bitwise Left and Right Shifts*
```
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
```
3. Since the shifting is incremental, there will be repeating patterns no matter what. Even if we reverse and shift again. So, I mix each bit with its neighbouring bits both before and after reversing. This removes any patters and adds that sweet effect of changing the key slightly to change the hash significantly.
```
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
```
4. The reverse and swap logics are fairly straight-forward and given in `awPass.h`.
