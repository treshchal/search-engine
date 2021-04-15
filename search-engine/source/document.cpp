#include <iostream>

#include "document.h"

using namespace std::literals::string_literals;

void PrintDocument(const Document& document) {
    std::cout << "{ "s
     << "document_id = "s << document.id << ", "s
     << "relevance = "s << document.relevance << ", "s
     << "rating = "s << document.rating
     << " }"s << std::endl;
}

std::ostream& operator<<(std::ostream& out, const Document& document){
    std::cout << "{ "s
     << "document_id = "s << document.id << ", "s
     << "relevance = "s << document.relevance << ", "s
     << "rating = "s << document.rating
     << " }"s << std::endl;

    return out;
}
