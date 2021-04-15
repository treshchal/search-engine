#include <cassert>
#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "search_server.h"
#include "string_processing.h"

using namespace std::literals::string_literals;

SearchServer::SearchServer(const std::string& stop_words_text)
	: SearchServer(string_processing::SplitIntoWords(stop_words_text)) {
}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    if (document_id < 0 || documents_.count(document_id)) {
	throw std::invalid_argument("ID of the document is negative or already linked to another document.");
    }

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);

    assert(words.size() != 0);

    const double inverted_word_count = 1.0 / words.size();

    for (const std::string& word : words) {
    	document_to_word_frequency_[word][document_id] += inverted_word_count;
    	id_to_word_frequency_[document_id][word] += inverted_word_count;
    }

    documents_.emplace(document_id,
    	DocumentData{
    		ComputeAverageRating(ratings),
            status
        }
    );

    document_ids_.insert(document_id);
}

void SearchServer::RemoveDocument(int document_id) {
    for (const auto word_to_frequency : id_to_word_frequency_.at(document_id)) {
    	document_to_word_frequency_.at(word_to_frequency.first).erase(document_id);
    }
    documents_.erase(document_id);
    document_ids_.erase(document_id);
    id_to_word_frequency_.erase(document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::kActual);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);

    std::vector<std::string> matched_words;

    for (const std::string& word : query.plus_words) {
        if (document_to_word_frequency_.count(word) == 0) {
            continue;
        }

        if (document_to_word_frequency_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    for (const std::string& word : query.minus_words) {
        if (document_to_word_frequency_.count(word) == 0) {
            continue;
        }

        if (document_to_word_frequency_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }

    return {matched_words, documents_.at(document_id).status};
}

int SearchServer::GetDocumentCount() const {
    return static_cast<int>(documents_.size());
}

std::set<int>::const_iterator SearchServer::begin() const{
    return document_ids_.cbegin();
}

std::set<int>::const_iterator SearchServer::end() const{
    return document_ids_.cend();
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
	static std::map<std::string, double> empty_map;
    if (!document_ids_.count(document_id)) {
        return empty_map;
    }

    return id_to_word_frequency_.at(document_id);
}

bool SearchServer::CheckForSpecialSymbols(const std::string& word) {
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c <= ' ';
    });
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;

    for (const std::string& word : string_processing::SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }

    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    int rating_sum = 0;

    for (const int rating : ratings) {
        rating_sum += rating;
    }

    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    if (text.empty()){
        return {};
    }

    bool is_minus = false;

    if (text[0] == '-') {
	is_minus = true;
	text = text.substr(1);
    }

    if (text.empty()) {
	throw std::invalid_argument("No text after \"minus\" character."s);
    } else if ((text[0] == '-') || (text[static_cast<int>(text.size() - 1)] == '-')) {
	throw std::invalid_argument("Minus in the end of the word or more than one minus in the start of the word."s);
    } else if (!CheckForSpecialSymbols(text)) {
	throw std::invalid_argument("The word contains invalid characters"s);
    }

    return {text, is_minus, IsStopWord(text)};
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;

    for (const std::string& word : string_processing::SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);

        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }

    return query;
}

double SearchServer::ComputeWordInverseDocumentFrequency(const std::string& word) const {
    if(document_to_word_frequency_.count(word)) {
	auto size_of_document_to_word_frequency = document_to_word_frequency_.at(word).size();

	if (size_of_document_to_word_frequency > 0){
            return log(GetDocumentCount() * 1.0 / size_of_document_to_word_frequency);
	}
    }

    return 0;

}

std::vector<Document> SearchServer::FindAllDocuments(const Query& query) const {
    std::map<int, double> document_to_relevance;

    for (const std::string& word : query.plus_words) {
        if (document_to_word_frequency_.count(word) == 0) {
            continue;
        }

        const double inverse_document_freq = ComputeWordInverseDocumentFrequency(word);

        for (const auto [document_id, term_freq] : document_to_word_frequency_.at(word)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }

    for (const std::string& word : query.minus_words) {
        if (document_to_word_frequency_.count(word) == 0) {
            continue;
        }

        for (const auto [document_id, _] : document_to_word_frequency_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    std::vector<Document> matched_documents;

    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({
            document_id,
            relevance,
	    documents_.at(document_id).rating
        });
    }

    return matched_documents;
}
