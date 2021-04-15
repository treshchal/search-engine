#pragma once

#include <iostream>

struct Document {
    int id = 0;
    double relevance = 0;
    int rating = 0;
};

enum class DocumentStatus {
    kActual,
    kIrrelevant,
    kBanned,
    kRemoved,
};

void PrintDocument(const Document& document);

std::ostream& operator<<(std::ostream& out, const Document& document);
