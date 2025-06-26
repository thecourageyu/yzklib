#include <iostream>
#include <string>
#include <random>
#include <ctime>

// Function to generate a random password
std::string generatePassword(int length) {
    const std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    const std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";
    const std::string symbols = "!@#$%^&*()-_=+[]{}<>?/";

    const std::string allChars = lowercase + uppercase + digits + symbols;

    std::random_device rd;                          // True random seed
    std::mt19937 gen(rd());                         // Mersenne Twister RNG
    std::uniform_int_distribution<> dist(0, allChars.size() - 1);

    std::string password;

    for (int i = 0; i < length; ++i) {
        password += allChars[dist(gen)];
    }

    return password;
}

int main() {
    int length;

    std::cout << "Enter desired password length: ";
    std::cin >> length;

    if (length <= 0) {
        std::cerr << "Invalid password length.\n";
        return 1;
    }

    std::string password = generatePassword(length);

    std::cout << "Generated password: " << password << "\n";

    return 0;
}
