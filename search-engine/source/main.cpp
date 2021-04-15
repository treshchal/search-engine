#include <iostream>

#include "test_run.h"

int main() {
    std::cout << "SAMPLE EXCEPTIONS CATCHING" << std::endl << std::endl;
    RunExceptions();

    std::cout << std::endl << "SAMPLE PAGE BREAK" << std::endl << std::endl;
    RunPaginator();

    std::cout << std::endl << "SAMPLE EMPTY REQUESTS CHECK" << std::endl << std::endl;
    RunEmptyRequests();

    std::cout << std::endl << "SAMPLE REMOVE DUPLICATES" << std::endl << std::endl;
    RunRemoveDuplicates();

    return 0;
}
