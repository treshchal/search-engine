#include <vector>
#include <set>
#include <string>

#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::set<std::set<std::string>> unique_documents;
    std::vector<int> duplicate_documents_ids;

    for(const int& document_id : search_server) {
        auto word_frequencies = search_server.GetWordFrequencies(document_id);

        std::set<std::string> unique_document_contents;

        for(const auto& word_frequency : word_frequencies) {
            unique_document_contents.insert(word_frequency.first);
        }

        if(!(unique_documents.emplace(unique_document_contents)).second) {
            duplicate_documents_ids.push_back(document_id);
        }
    }

    for(const int& document_id : duplicate_documents_ids) {
        search_server.RemoveDocument(document_id);
    }
}
