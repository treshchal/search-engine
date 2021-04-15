#include <iostream>

#include "read_input_functions.h"

std::string ReadLine() {
    std::string line;
    std::getline(std::cin, line);

    return line;
}

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();

    return result;
}
