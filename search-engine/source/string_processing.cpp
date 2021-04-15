#include <set>
#include <string>

#include "string_processing.h"

std::vector<std::string> string_processing::SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;

    std::string word;

    for (const char character : text) {
        if (character == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += character;
        }
    }
    words.push_back(word);

    return words;
}
