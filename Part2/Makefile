CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
SRC_DIR = src
TEST_DIR = test
INC_DIR = include

# Test executables
test_social_graph: $(SRC_DIR)/social_graph.cpp $(TEST_DIR)/test_social_class.cpp
	@echo "Compiling SocialGraph tests..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

test_geographic_network: $(SRC_DIR)/geographic_network.cpp $(TEST_DIR)/test_geographic_network.cpp
	@echo "Compiling GeographicNetwork tests..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

test_interaction_graph: $(SRC_DIR)/interaction_graph.cpp $(TEST_DIR)/test_interaction_graph.cpp
	@echo "Compiling InteractionGraph tests..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

test_user_manager: $(SRC_DIR)/user_manager.cpp $(SRC_DIR)/social_graph.cpp $(SRC_DIR)/geographic_network.cpp $(SRC_DIR)/interaction_graph.cpp $(SRC_DIR)/post_list.cpp $(SRC_DIR)/post_pool.cpp $(SRC_DIR)/user.cpp $(TEST_DIR)/test_user_manager.cpp
	@echo "Compiling UserManager tests..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

test_runner: $(TEST_DIR)/test_runner.cpp
	@echo "Compiling test runner..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

# Build all tests

build-tests: test_social_graph test_geographic_network test_interaction_graph test_user_manager
	@echo "All test executables compiled successfully."

# Run all tests with centralized runner
test: build-tests test_runner
	@echo ""
	@./test_runner

# Run individual test suites
test-social: test_social_graph
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running SocialGraph Tests$(shell tput sgr0)"
	@echo ""
	@./test_social_graph

test-geo: test_geographic_network
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running GeographicNetwork Tests$(shell tput sgr0)"
	@echo ""
	@./test_geographic_network

test-interaction: test_interaction_graph
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running InteractionGraph Tests$(shell tput sgr0)"
	@echo ""
	@./test_interaction_graph

test-user-manager: test_user_manager
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running UserManager Tests$(shell tput sgr0)"
	@echo ""
	@./test_user_manager

# Clean up

clean:
	@echo "Cleaning up test executables..."
	@rm -f test_social_graph test_geographic_network test_interaction_graph test_runner test_user_manager

.PHONY: test test-social test-geo test-interaction build-tests clean
