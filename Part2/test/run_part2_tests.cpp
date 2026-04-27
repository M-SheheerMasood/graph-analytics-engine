
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For std::system
#include <sstream>
#include <iomanip> // For std::setprecision
#include <sys/wait.h> // For WEXITSTATUS (This will work on WSL)

// ANSI color codes
#define RESET   "\033[0m"
#define FAIL    "\033[1;31m" // Bold Red
#define PASS    "\033[1;32m" // Bold Green
#define SKIP    "\033[1;33m" // Bold Yellow
#define TEST_NAME "\033[1;34m" // Bold Blue
#define HEADER  "\033[1;35m" // Bold Magenta
#define TIME    "\033[1;36m" // Bold Cyan
#define FINAL_SCORE "\033[1;42;30m" // Black on Green BG
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

// Struct to hold suite info
struct TestSuite {
    std::string name;
    std::string executable;
    double weight;          // Final marks (e.g., 30, 40, 50)
    double raw_total = 100.0; // All our tests are out of 100 raw points
    double earned_marks = 0.0;
};

void print_header() {
    std::cout << BOLD << HEADER;
    std::cout << "================================================================================\n";
    std::cout << "                      GRAPH IMPLEMENTATION TEST SUITE                             \n";
    std::cout << "================================================================================\n";
    std::cout << RESET << "\n";
}

void print_suite_header(const std::string& name) {
    std::cout << "\n" << BOLD << SKIP; // <-- FIXED (was YELLOW)
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << " Running: " << name << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << RESET;
}

void print_final_summary(const std::vector<TestSuite>& suites, double total_earned, double total_possible) {
    std::cout << "\n" << BOLD << HEADER;
    std::cout << "================================================================================\n";
    std::cout << "                            FINAL MARK SUMMARY                                  \n";
    std::cout << "================================================================================\n";
    std::cout << RESET << BOLD << std::fixed << std::setprecision(1);

    for (const auto& suite : suites) {
        std::cout << "  " << std::left << std::setw(25) << suite.name << ": ";
        if (suite.earned_marks == suite.weight) {
            std::cout << PASS;
        } else {
            std::cout << SKIP; // Use yellow if not full marks
        }
        std::cout << suite.earned_marks << " / " << suite.weight << " Marks" << RESET << "\n";
    }

    std::cout << SKIP << "--------------------------------------------------------------------------------" << RESET << "\n"; // <-- FIXED (was YELLOW)
    
    std::cout << "  " << BOLD << std::left << std::setw(25) << "TOTAL SCORE" << ": ";
    
    // Final score color
    if (total_earned == total_possible) std::cout << PASS;
    else if (total_earned > 0) std::cout << TIME;
    else std::cout << FAIL;
    
    std::cout << total_earned << " / " << total_possible << " Marks" << RESET << "\n";
    
    std::cout << HEADER << "================================================================================\n";
    std::cout << RESET;

    if (total_earned == total_possible) {
        std::cout << "\n" << PASS << BOLD << "All tests passed! Full Marks!" << RESET << "\n\n";
    } else {
        std::cout << "\n" << SKIP << BOLD << "Some tests failed. Please review the output above." << RESET << "\n\n";
    }
}

int main() {
    print_header();

    // UPDATED: UserManager suite has been removed.
    std::vector<TestSuite> suites = {
        {"SocialGraph Tests", "./test_social_graph", 30.0},
        {"GeographicNetwork Tests", "./test_geographic_network", 30.0},
        {"InteractionGraph Tests", "./test_interaction_graph", 40.0}
    };

    double total_earned = 0.0;
    double total_possible = 0.0;

    for (auto& suite : suites) {
        print_suite_header(suite.name);

        int system_result = std::system(suite.executable.c_str());

        int raw_score = 0;
        if (WIFEXITED(system_result)) {
            raw_score = WEXITSTATUS(system_result);
        } else {
            raw_score = -1;
        }
        
        if (raw_score < 0 || raw_score > 100) {
            std::cout << FAIL << "\n  Error: Test executable returned invalid score: " << raw_score 
                      << ". (Did it compile? Or did it crash?)\n" << RESET;
            raw_score = 0;
        }

        suite.earned_marks = (static_cast<double>(raw_score) / suite.raw_total) * suite.weight;
        
        total_earned += suite.earned_marks;
        total_possible += suite.weight;
    }

    print_final_summary(suites, total_earned, total_possible);

    return (total_earned == total_possible) ? 0 : 1;
}