#include <iostream>

#include "exception_catch.h"
#include "paginator.h"
#include "remove_duplicates.h"
#include "request_queue.h"
#include "search_server.h"
#include "test_run.h"

void RunExceptions() {
    using namespace std::literals::string_literals;

    SearchServer search_server;
    try {
    	search_server = SearchServer("and in on"s);
    } catch (const std::exception& e) {
    	std::cout << "Ошибка созданий поискового сервера со стоп-словами: and in on"s
    	 << ": "s << e.what() << std::endl;
    	abort();
    }

    exception_catch::AddDocument(search_server, 1, "big cat fluffy tail"s, DocumentStatus::kActual, {7, 2, 7});
    exception_catch::AddDocument(search_server, 1, "fluffy dog and stylish collar"s, DocumentStatus::kActual, {1, 2});
    exception_catch::AddDocument(search_server, -1, "fluffy dog and stylish collar"s, DocumentStatus::kActual, {1, 2});
    exception_catch::AddDocument(search_server, 3, "big dog and sta\x12rling evgeniy"s, DocumentStatus::kActual, {1, 3, 2});
    exception_catch::AddDocument(search_server, 4, "big dog starleng evgeniy"s, DocumentStatus::kActual, {1, 1, 1});

    exception_catch::FindTopDocuments(search_server, "fluffy -dog"s);
    exception_catch::FindTopDocuments(search_server, "fluffy --cat"s);
    exception_catch::FindTopDocuments(search_server, "fluffy -"s);

    exception_catch::MatchDocuments(search_server, "fluffy dog"s);
    exception_catch::MatchDocuments(search_server, "stylish -cat"s);
    exception_catch::MatchDocuments(search_server, "stylish --dog"s);
    exception_catch::MatchDocuments(search_server, "fluffy - tail"s);
}

void RunPaginator() {
    using namespace std::literals::string_literals;

    SearchServer search_server;
    try {
    	search_server = SearchServer("and with"s);
    } catch (const std::exception& e) {
    	std::cout << "Ошибка созданий поискового сервера со стоп-словами: and with"s
    	 << ": "s << e.what() << std::endl;
    	abort();
    }

    search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::kActual, {7, 2, 7});
    search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::kActual, {1, 2, 3});
    search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::kActual, {1, 2, 8});
    search_server.AddDocument(4, "big dog cat Vladislav"s, DocumentStatus::kActual, {1, 3, 2});
    search_server.AddDocument(5, "big dog hamster Borya"s, DocumentStatus::kActual, {1, 1, 1});

    const auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 2;
    const auto pages = Paginate(search_results, page_size);

    for (auto page = pages.begin(); page != pages.end(); ++page) {
        std::cout << *page;
        std::cout << "Page break"s << std::endl;
    }
}

void RunEmptyRequests() {
    using namespace std::literals::string_literals;

    SearchServer search_server;
    try {
        search_server = SearchServer("and in at"s);
    } catch (const std::exception& e) {
    	std::cout << "Ошибка созданий поискового сервера со стоп-словами: and in at"s
    	 << ": "s << e.what() << std::endl;
    	abort();
    }

    RequestQueue request_queue(search_server);

    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::kActual, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::kActual, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::kActual, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::kActual, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::kActual, {1, 1, 1});

    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }

    request_queue.AddFindRequest("curly dog"s);
    request_queue.AddFindRequest("big collar"s);
    request_queue.AddFindRequest("sparrow"s);

    std::cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << std::endl;
}

void RunRemoveDuplicates() {
	using namespace std::literals::string_literals;

	SearchServer search_server;
	try {
	    search_server = SearchServer("and with"s);
	} catch (const std::exception& e) {
		std::cout << "Ошибка созданий поискового сервера со стоп-словами: and with"s
		 << ": "s << e.what() << std::endl;
		abort();
	}

	exception_catch::AddDocument(search_server, 1, "funny pet and nasty rat"s, DocumentStatus::kActual, {7, 2, 7});
	exception_catch::AddDocument(search_server, 2, "funny pet with curly hair"s, DocumentStatus::kActual, {1, 2});

	// дубликат документа 2, будет удалён
	exception_catch::AddDocument(search_server, 3, "funny pet with curly hair"s, DocumentStatus::kActual, {1, 2});

	// отличие только в стоп-словах, считаем дубликатом
	exception_catch::AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::kActual, {1, 2});

	// множество слов такое же, считаем дубликатом документа 1
	exception_catch::AddDocument(search_server, 5, "funny funny pet and nasty nasty rat"s, DocumentStatus::kActual, {1, 2});

	// добавились новые слова, дубликатом не является
	exception_catch::AddDocument(search_server, 6, "funny pet and not very nasty rat"s, DocumentStatus::kActual, {1, 2});

	// множество слов такое же, как в id 6, несмотря на другой порядок, считаем дубликатом
	exception_catch::AddDocument(search_server, 7, "very nasty rat and not very funny pet"s, DocumentStatus::kActual, {1, 2});

	// есть не все слова, не является дубликатом
	exception_catch::AddDocument(search_server, 8, "pet with rat and rat and rat"s, DocumentStatus::kActual, {1, 2});

	// слова из разных документов, не является дубликатом
	exception_catch::AddDocument(search_server, 9, "nasty rat with curly hair"s, DocumentStatus::kActual, {1, 2});

	std::cout << "Before duplicates removed: "s << search_server.GetDocumentCount() << std::endl;
	RemoveDuplicates(search_server);
	std::cout << "After duplicates removed: "s << search_server.GetDocumentCount() << std::endl;

}
