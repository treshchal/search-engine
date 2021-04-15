#pragma once

#include <deque>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    void AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    void AddFindRequest(const std::string& raw_query, DocumentStatus status);

    void AddFindRequest(const std::string& raw_query);

    [[nodiscard]]int GetNoResultRequests() const;

private:
    struct QueryResult {
    	std::string query;
        bool is_empty_result = true;
        int64_t timestamp = 0;
    };

private:
    static const int kMinutesInDay = 1440;

private:
    void UpdateQueue(const std::string& raw_query, const std::vector<Document>& top_documents);

    void AddToQueue(QueryResult& query_result, const std::vector<Document>& top_documents);

    void RemoveFromQueue();

private:
    std::deque<QueryResult> requests_;
    const SearchServer& server_;
    int64_t timestamp_ = 0;
};
