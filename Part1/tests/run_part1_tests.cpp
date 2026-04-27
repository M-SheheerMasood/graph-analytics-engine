// In CS202_PA3_SOLUTION/run_part1_tests.cpp

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <sys/wait.h>


#define RESET   "\033[0m"
#define FAIL    "\033[1;31m"
#define PASS    "\033[1;32m"
#define SKIP    "\033[1;33m"
#define HEADER  "\033[1;35m"
#define BOLD    "\033[1m"
#define FINAL_SCORE "\033[1;42;30m"


struct TestSuite {
    std::string name;
    std::string executable_path;
    double max_raw_score;
    double final_weight;
    double earned_raw_score = 0.0;
    double earned_final_marks = 0.0;
};

void print_header() {
    std::cout << BOLD << HEADER;
    std::cout << "======================================================================\n";
    std::cout << "                      PART 1 - HEAPS & HASH TABLES                      \n";
    std::cout << "======================================================================\n";
    std::cout << RESET << std::endl;
}

void print_suite_header(const std::string& name) {
    std::cout << "\n" << BOLD << SKIP;
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << " Running Suite: " << name << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << RESET;
}

void print_final_summary(const std::vector<TestSuite>& suites) {
    double total_earned = 0.0;
    double total_possible = 0.0;

    std::cout << "\n" << BOLD << HEADER;
    std::cout << "======================================================================\n";
    std::cout << "                         PART 1 - FINAL MARK SUMMARY                    \n";
    std::cout << "======================================================================\n";
    std::cout << RESET << BOLD << std::fixed << std::setprecision(1);

    for (const auto& suite : suites) {
        total_earned += suite.earned_final_marks;
        total_possible += suite.final_weight;
        std::cout << "  " << std::left << std::setw(30) << suite.name << ": ";
        if (suite.earned_final_marks == suite.final_weight) std::cout << PASS;
        else std::cout << SKIP;
        std::cout << suite.earned_final_marks << " / " << suite.final_weight << " Marks" << RESET << "\n";
    }

    std::cout << SKIP << "----------------------------------------------------------------------" << RESET << "\n";
    
    std::cout << "  " << BOLD << std::left << std::setw(30) << "PART 1 TOTAL" << ": " << FINAL_SCORE << " "
              << total_earned << " / " << total_possible << " Marks " << RESET << "\n";
    
    std::cout << HEADER << "======================================================================\n";
    std::cout << RESET;
}

int main() {
    print_header();

    std::vector<TestSuite> part1_suites = {
        {"Hash Tables",     "./test_hash",          30.0, 30.0},
        {"Min Heap",          "./test_heap",          100.0, 30.0},
        {"Social Media System", "./test_social_media",  100.0, 40.0}
    };

    for (auto& suite : part1_suites) {
        print_suite_header(suite.name);
        int system_result = std::system(suite.executable_path.c_str());

        if (WIFEXITED(system_result)) {
            suite.earned_raw_score = WEXITSTATUS(system_result);
        } else {
            suite.earned_raw_score = -1;
        }
        
        if (suite.earned_raw_score < 0 || suite.earned_raw_score > suite.max_raw_score) {
            std::cout << FAIL << "\n  Error: Test executable returned an invalid score: " << suite.earned_raw_score 
                      << ". Awarding 0 points.\n" << RESET;
            suite.earned_raw_score = 0;
        }

        suite.earned_final_marks = (suite.earned_raw_score / suite.max_raw_score) * suite.final_weight;
    }

    print_final_summary(part1_suites);

    return 0;
}