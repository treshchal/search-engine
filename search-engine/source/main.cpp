#include <iostream>

#include "log_duration.h"
#include "test_run.h"

int main() {
    std::cout << "SAMPLE EXCEPTIONS CATCHING" << std::endl << std::endl;
    {
    LOG_DURATION("exceptions");
    RunExceptions();
    }

    std::cout << std::endl << "SAMPLE PAGE BREAK" << std::endl << std::endl;
    {
    LOG_DURATION("pages");
    RunPaginator();
    }

    std::cout << std::endl << "SAMPLE EMPTY REQUESTS CHECK" << std::endl << std::endl;
    {
    LOG_DURATION("empty");
    RunEmptyRequests();
    }

    std::cout << std::endl << "SAMPLE REMOVE DUPLICATES" << std::endl << std::endl;

    {
    LOG_DURATION("duplicates");
    RunRemoveDuplicates();
    }
    return 0;
}
