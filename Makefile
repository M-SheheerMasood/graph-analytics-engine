

CXX = g++
CXXFLAGS_COMMON = -std=c++17 -Wall -Wextra -g -pthread
CXXFLAGS_P1 = $(CXXFLAGS_COMMON) -I./Part1/headers
CXXFLAGS_P2 = $(CXXFLAGS_COMMON) -I./Part2/include

BIN_DIR = bin

# --- Part 1 Files ---
P1_SRC_HEAP = Part1/src/Heap.cpp
P1_SRC_HASH = Part1/src/HashTables.cpp
P1_SRC_SOCIAL_SYS = Part1/src/SocialMediaSystem.cpp
P1_SRC_ALL = $(P1_SRC_HEAP) $(P1_SRC_HASH) $(P1_SRC_SOCIAL_SYS)
P1_TEST_HEAP = Part1/tests/test_heap.cpp
P1_TEST_HASH = Part1/tests/test_hash.cpp
P1_TEST_SOCIAL_SYS = Part1/tests/test_social_media.cpp

# --- Part 2 Files ---
P2_SRC_SOCIAL = Part2/src/social_graph.cpp
P2_SRC_GEO = Part2/src/geographic_network.cpp
P2_SRC_INTERACTION = Part2/src/interaction_graph.cpp
# UPDATED: Removed UserManager-specific components that aren't used elsewhere
P2_SRC_COMPONENTS = Part2/src/post_list.cpp Part2/src/post_pool.cpp Part2/src/user.cpp
P2_SRC_ALL = $(P2_SRC_SOCIAL) $(P2_SRC_GEO) $(P2_SRC_INTERACTION) $(P2_SRC_COMPONENTS)
P2_TEST_SOCIAL = Part2/test/test_social_class.cpp
P2_TEST_GEO = Part2/test/test_geographic_network.cpp
P2_TEST_INTERACTION = Part2/test/test_interaction_graph.cpp

# --- Runner Files ---
RUNNER_ALL_SRC = run_all_tests.cpp
RUNNER_P1_SRC = Part1/tests/run_part1_tests.cpp
RUNNER_P2_SRC = Part2/test/run_part2_tests.cpp

# --- Main Targets ---
.PHONY: all test part1 part2 clean
# UPDATED: Removed test-user-manager
.PHONY: test-heap test-hash test-social-media
.PHONY: test-social-graph test-geographic-network test-interaction-graph

all: test

test: $(BIN_DIR)/run_all_tests
	@cd $(BIN_DIR) && ./run_all_tests

part1: $(BIN_DIR)/run_part1_tests
	@cd $(BIN_DIR) && ./run_part1_tests

part2: $(BIN_DIR)/run_part2_tests
	@cd $(BIN_DIR) && ./run_part2_tests


# ==============================================================================
# Individual Test Run Rules
# ==============================================================================

test-heap: $(BIN_DIR)/test_heap
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running Heap Tests (Part 1)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_heap

test-hash: $(BIN_DIR)/test_hash
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running HashTable Tests (Part 1)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_hash

test-social-media: $(BIN_DIR)/test_social_media
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running Social Media System Tests (Part 1)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_social_media

test-social-graph: $(BIN_DIR)/test_social_graph
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running SocialGraph Tests (Part 2)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_social_graph

test-geographic-network: $(BIN_DIR)/test_geographic_network
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running GeographicNetwork Tests (Part 2)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_geographic_network

test-interaction-graph: $(BIN_DIR)/test_interaction_graph
	@echo ""
	@echo "$(shell tput bold)$(shell tput setaf 6)Running InteractionGraph Tests (Part 2)$(shell tput sgr0)"
	@-./$(BIN_DIR)/test_interaction_graph


# ==============================================================================
# Build Rules for ALL executables
# ==============================================================================
# UPDATED: Removed test_user_manager dependency
$(BIN_DIR)/run_all_tests: $(RUNNER_ALL_SRC) $(BIN_DIR)/test_heap $(BIN_DIR)/test_hash $(BIN_DIR)/test_social_media $(BIN_DIR)/test_social_graph $(BIN_DIR)/test_geographic_network $(BIN_DIR)/test_interaction_graph | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_COMMON) $< -o $@

$(BIN_DIR)/run_part1_tests: $(RUNNER_P1_SRC) $(BIN_DIR)/test_heap $(BIN_DIR)/test_hash $(BIN_DIR)/test_social_media | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_COMMON) $< -o $@

# UPDATED: Removed test_user_manager dependency
$(BIN_DIR)/run_part2_tests: $(RUNNER_P2_SRC) $(BIN_DIR)/test_social_graph $(BIN_DIR)/test_geographic_network $(BIN_DIR)/test_interaction_graph | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_P2) $< -o $@

# --- Part 1 Build Rules ---
$(BIN_DIR)/test_heap: $(P1_SRC_HEAP) $(P1_TEST_HEAP) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_P1) $^ -o $@

$(BIN_DIR)/test_hash: $(P1_SRC_HASH) $(P1_TEST_HASH) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_P1) $^ -o $@

$(BIN_DIR)/test_social_media: $(P1_SRC_ALL) $(P1_TEST_SOCIAL_SYS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_P1) $^ -o $@

# --- Part 2 Build Rules ---
# NOTE: The Social, Geo, and Interaction tests may need P2_SRC_COMPONENTS to build.
# If you get linking errors, uncomment the dependencies. For now, we assume they are self-contained.
$(BIN_DIR)/test_social_graph: $(P2_SRC_SOCIAL) $(P2_TEST_SOCIAL) | $(BIN_DIR) # Add $(P2_SRC_COMPONENTS) if needed
	$(CXX) $(CXXFLAGS_P2) $^ -o $@

$(BIN_DIR)/test_geographic_network: $(P2_SRC_GEO) $(P2_TEST_GEO) | $(BIN_DIR) # Add $(P2_SRC_COMPONENTS) if needed
	$(CXX) $(CXXFLAGS_P2) $^ -o $@

$(BIN_DIR)/test_interaction_graph: $(P2_SRC_INTERACTION) $(P2_TEST_INTERACTION) $(P2_SRC_COMPONENTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS_P2) $^ -o $@

# --- Utility Rules ---
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	@echo "Cleaning up all compiled files..."
	@rm -rf $(BIN_DIR)