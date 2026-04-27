

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>      // For std::system
#include <iomanip>      // For std::setprecision
#include <sys/wait.h>   // For WEXITSTATUS


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
    double max_raw_score; // e.g., 30 for Hash, 100 for SocialGraph
    double final_weight;  // The final marks this suite is worth
    
    // Results
    double earned_raw_score = 0.0;
    double earned_final_marks = 0.0;
};

void print_header() {
    std::cout << BOLD << HEADER;
    std::cout << "======================================================================\n";
    std::cout << "          CS202 PROGRAMMING ASSIGNMENT 3 - COMPLETE TEST SUITE          \n";
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
    std::cout << "                            FINAL MARK SUMMARY                          \n";
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
    
    std::cout << "  " << BOLD << std::left << std::setw(30) << "GRAND TOTAL" << ": " << FINAL_SCORE << " "
              << total_earned << " / " << total_possible << " Marks " << RESET << "\n";
    
    std::cout << HEADER << "======================================================================\n";
    std::cout << RESET;
}

int main() {
    print_header();

    // UPDATED: UserManager suite has been removed.
    std::vector<TestSuite> all_suites = {
        // --- Part 1 Suites ---
        {"Part 1: Hash Tables",     "./test_hash",          30.0, 30.0},
        {"Part 1: Min Heap",          "./test_heap",          100.0, 30.0},
        {"Part 1: Social Media System", "./test_social_media",  100.0, 40.0},
        
        // --- Part 2 Suites ---
        {"Part 2: SocialGraph",       "./test_social_graph",        100.0, 30.0},
        {"Part 2: GeographicNetwork",   "./test_geographic_network",    100.0, 30.0},
        {"Part 2: InteractionGraph",  "./test_interaction_graph",   100.0, 40.0}
    };

    for (auto& suite : all_suites) {
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

    print_final_summary(all_suites);

    return 0;
}