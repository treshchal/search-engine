#include "request_queue.h"
#include "search_server.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    :server_(search_server)
{
}

template <typename DocumentPredicate>
void RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    ++timestamp_;

    std::vector<Document> result = server_.FindTopDocuments(raw_query, document_predicate);

    UpdateQueue(raw_query, result);
}

void RequestQueue::UpdateQueue(const std::string& raw_query, const std::vector<Document>& top_documents) {
	QueryResult query_result = { .query = raw_query, .timestamp = timestamp_ };

    AddToQueue(query_result, top_documents);
    RemoveFromQueue();
}

void RequestQueue::AddToQueue(QueryResult& query_result, const std::vector<Document>& top_documents) {
	query_result.is_empty_result = top_documents.empty();

    requests_.push_front(query_result);
}

void RequestQueue::RemoveFromQueue() {
    for (const QueryResult& request : requests_) {
    	if (timestamp_ - request.timestamp >= kMinutesInDay) {
    	    requests_.pop_back();
    	}
    }
}

void RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

void RequestQueue::AddFindRequest(const std::string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::kActual);
}

int RequestQueue::GetNoResultRequests() const {
    int no_result_requests = 0;

    for (const QueryResult& request : requests_) {
        if (request.is_empty_result) {
            ++no_result_requests;
        }
    }

    return no_result_requests;
}
