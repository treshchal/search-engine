#include <iostream>
#include <string>
#include <vector>

#include "exception_catch.h"
#include "search_server.h"

void exception_catch::PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    using namespace std::literals::string_literals;

    std::cout << "{ "s
     << "document_id = "s << document_id << ", "s
     << "status = "s << static_cast<int>(status) << ", "s
     << "words ="s;

    for (const std::string& word : words) {
    	std::cout << ' ' << word;
    }

    std::cout << "}"s << std::endl;
}

void exception_catch::AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
                 const std::vector<int>& ratings) {
    using namespace std::literals::string_literals;

    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const std::exception& e) {
    	std::cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << std::endl;
    }
}

void exception_catch::FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    using namespace std::literals::string_literals;

    std::cout << "Результаты поиска по запросу: "s << raw_query << std::endl;

    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const std::exception& e) {
    	std::cout << "Ошибка поиска: "s << e.what() << std::endl;
    }
}

void exception_catch::MatchDocuments(const SearchServer& search_server, const std::string& query) {
    using namespace std::literals::string_literals;

    try {
    	std::cout << "Матчинг документов по запросу: "s << query << std::endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = *std::next(search_server.begin(), index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            exception_catch::PrintMatchDocumentResult(document_id, words, status);
        }
    } catch (const std::exception& e) {
    	std::cout << "Ошибка матчинга документов на запрос "s << query << ": "s << e.what() << std::endl;
    }
}
