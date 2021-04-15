#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "document.h"
#include "string_processing.h"

class SearchServer {
public:
    SearchServer() = default;
    SearchServer(const SearchServer& other) = default;
    SearchServer& operator=(const SearchServer& other) = default;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
    : stop_words_(string_processing::StringContainerToStringSet(stop_words)) {
	using namespace std::literals::string_literals;

	for(const std::string& word : stop_words) {
	    if (!CheckForSpecialSymbols(word)) {
		throw std::invalid_argument("The word contains invalid characters."s);
	    }
	}
    }

    explicit SearchServer(const std::string& stop_words_text);

public:
    template <typename Predicate>
    [[nodiscard]] std::vector<Document> FindTopDocuments(const std::string& raw_query, Predicate predicate) const {
        const Query query = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
            [&](const Document& left_hand_side, const Document& right_hand_side) {
                if (std::abs(left_hand_side.relevance - right_hand_side.relevance) < kCloseToZero) {
                    return left_hand_side.rating > right_hand_side.rating;
                } else {
                    return left_hand_side.relevance > right_hand_side.relevance;
                }
	    }
	);

        std::vector<Document> key_matched_documents;

        for (const Document& document : matched_documents) {
            if (predicate(document.id, documents_.at(document.id).status, document.rating)) {
                key_matched_documents.push_back(document);
            }
        }

        if (static_cast<int>(key_matched_documents.size()) > kMaxResultDocumentCount) {
            key_matched_documents.resize(static_cast<size_t>(kMaxResultDocumentCount));
        }

        return key_matched_documents;
    }

    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);

    void RemoveDocument(int document_id);

    [[nodiscard]] std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const;

    [[nodiscard]] std::vector<Document> FindTopDocuments(const std::string& raw_query) const;

    [[nodiscard]] std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

    [[nodiscard]] int GetDocumentCount() const;

    [[nodiscard]] std::set<int>::const_iterator begin() const;

    [[nodiscard]] std::set<int>::const_iterator end() const;

    [[nodiscard]] const std::map<std::string, double>& GetWordFrequencies(int document_id) const;

private:
    struct DocumentData {
        int rating = 0;
        DocumentStatus status = DocumentStatus::kActual;
    };

    struct QueryWord {
        std::string data;
        bool is_minus = false;
        bool is_stop = false;
    };

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

private:
    static const int kMaxResultDocumentCount = 5;
    static constexpr double kCloseToZero = 1e-6;

private:
    [[nodiscard]] static bool CheckForSpecialSymbols(const std::string& word);

    [[nodiscard]] bool IsStopWord(const std::string& word) const;

    [[nodiscard]] std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    [[nodiscard]] static int ComputeAverageRating(const std::vector<int>& ratings);

    [[nodiscard]] QueryWord ParseQueryWord(std::string text) const;

    [[nodiscard]] Query ParseQuery(const std::string& text) const;

    [[nodiscard]] double ComputeWordInverseDocumentFrequency(const std::string& word) const;

    [[nodiscard]] std::vector<Document> FindAllDocuments(const Query& query) const;

private:
    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> document_to_word_frequency_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;
    std::map<int, std::map<std::string, double>> id_to_word_frequency_;
};
