# search-engine

This program in it's run creates multiple search servers and tests various things with them.

In "sample exception cathcing" it tests the ability to catch exceptions while adding documents to search server,
searching documents and matching documents.
In "sample page break" this program splits search results into multiple pages.
In "sample empty requests check" it shows you current amount of empty requests in queue.
In "sample remove duplicates" it removes copies of documents that were added into search server first.

Search server in it's constructor recieves stop words in the form of string literal
with words separeted with "space". Stop words do not count in query when matching documents.

Query can have minus words, that has "-" symbol before them. If such word is in document,
the document won't show up in the search results.

Time of each test run in main.cpp is being logged using macro from log_duration.h.
