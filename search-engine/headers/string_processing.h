#pragma once

#include <set>
#include <string>
#include <vector>

namespace string_processing {

std::vector<std::string> SplitIntoWords(const std::string& text);

template <typename StringContainer>
std::set<std::string> StringContainerToStringSet(const StringContainer& string_container) {
	std::set<std::string> set_of_strings;
    for (const std::string& word : string_container) {
        if (!word.empty()) {
            set_of_strings.insert(word);
        }
    }

    return set_of_strings;
}

} //namespace string_processing
