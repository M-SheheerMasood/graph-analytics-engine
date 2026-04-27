#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include "../headers/Heap.h"

using namespace std;

// ANSI color codes for output
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"

int totalTests = 0;
int passedTests = 0;

void printTestHeader(const string& testName) {
    cout << YELLOW << "======================================" << RESET << endl;
    cout << CYAN << testName << RESET << endl;
    cout << YELLOW << "======================================" << RESET << endl;
}

void printResult(bool passed, const string& message = "") {
    totalTests++;
    if (passed) {
        passedTests++;
        cout << GREEN << " PASSED" << RESET;
        if (!message.empty()) cout << " - " << message;
        cout << endl;
    } else {
        cout << RED << " FAILED" << RESET;
        if (!message.empty()) cout << " - " << message;
        cout << endl;
    }
}

void printHeap(MinHeap& heap) {
    cout << YELLOW << "Heap: [ ";
    for (int i = 0; i < heap.size(); i++) {
        cout << heap.elementAt(i) << " ";
    }
    cout << "]" << RESET << endl;
}

bool verifyMinHeapProperty(MinHeap& heap) {
    for (int i = 0; i < heap.size() / 2; i++) {
        int l = heap.left(i);
        int r = heap.right(i);
        
        if (l < heap.size() && heap.elementAt(i) > heap.elementAt(l)) {
            return false;
        }
        if (r < heap.size() && heap.elementAt(i) > heap.elementAt(r)) {
            return false;
        }
    }
    return true;
}

void testBasicInsertion() {
    printTestHeader("Test 1: Basic Insertion");
    MinHeap heap(15);
    vector<int> values = {10, 20, 5, 30, 40, 3, 50, 8, 15, 25};
    
    cout << BLUE << "Inserting values: ";
    for (int val : values) {
        cout << val << " ";
        heap.insertKey(val);
    }
    cout << RESET << endl;
    
    printHeap(heap);
    
    bool sizeTest = (heap.size() == 10);
    printResult(sizeTest, "Correct size after insertions");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained");
    
    bool minTest = (heap.getMin() == 3);
    printResult(minTest, "Minimum element is correct");
}

void testInsertionOrder() {
    printTestHeader("Test 2: Insertion with Different Orders");
    
    // Test ascending order
    MinHeap heap1(10);
    for (int i = 1; i <= 5; i++) {
        heap1.insertKey(i * 10);
    }
    bool ascendingTest = verifyMinHeapProperty(heap1);
    printResult(ascendingTest, "Insertion in ascending order");
    
    // Test descending order
    MinHeap heap2(10);
    for (int i = 5; i >= 1; i--) {
        heap2.insertKey(i * 10);
    }
    bool descendingTest = verifyMinHeapProperty(heap2);
    printResult(descendingTest, "Insertion in descending order");
    
    // Test with duplicates
    MinHeap heap3(10);
    heap3.insertKey(20);
    heap3.insertKey(20);
    heap3.insertKey(10);
    heap3.insertKey(10);
    bool duplicateTest = verifyMinHeapProperty(heap3);
    printResult(duplicateTest, "Insertion with duplicate values");
}

void testExtractMin() {
    printTestHeader("Test 3: extractMin()");
    MinHeap heap(10);
    vector<int> values = {15, 10, 20, 8, 25, 30, 12};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Initial heap:" << RESET << endl;
    printHeap(heap);
    
    int extracted = heap.extractMin();
    cout << BLUE << "Extracted: " << extracted << RESET << endl;
    printHeap(heap);
    
    bool extractTest = (extracted == 8);
    printResult(extractTest, "Extracted correct minimum");
    
    bool sizeTest = (heap.size() == 6);
    printResult(sizeTest, "Size decreased correctly");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained after extraction");
}

void testMultipleExtractions() {
    printTestHeader("Test 4: Multiple Extractions");
    MinHeap heap(10);
    vector<int> values = {50, 30, 20, 15, 10, 40, 25};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Initial heap:" << RESET << endl;
    printHeap(heap);
    
    // Extract all and verify order
    vector<int> extracted_vals;
    while (heap.size() > 0) {
        extracted_vals.push_back(heap.extractMin());
    }
    
    cout << BLUE << "Extracted in order: ";
    for (int val : extracted_vals) {
        cout << val << " ";
    }
    cout << RESET << endl;
    
    bool sorted = is_sorted(extracted_vals.begin(), extracted_vals.end());
    printResult(sorted, "Elements extracted in sorted order");
    
    bool emptyTest = (heap.size() == 0);
    printResult(emptyTest, "Heap is empty after all extractions");
}

void testDecreaseKey() {
    printTestHeader("Test 5: decreaseKey()");
    MinHeap heap(10);
    vector<int> values = {10, 20, 15, 30, 40, 25};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Before decreaseKey:" << RESET << endl;
    printHeap(heap);
    
    // Decrease element at index 4 (value 40) to 5
    cout << BLUE << "Decreasing element at index 4 to 5" << RESET << endl;
    heap.decreaseKey(4, 5);
    
    printHeap(heap);
    
    bool newMinTest = (heap.getMin() == 5);
    printResult(newMinTest, "New minimum is correct");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained");
    
    // Test decreasing middle element
    MinHeap heap2(10);
    heap2.insertKey(5);
    heap2.insertKey(10);
    heap2.insertKey(15);
    heap2.insertKey(20);
    
    heap2.decreaseKey(3, 7);  // Decrease 20 to 7
    bool midDecreaseTest = verifyMinHeapProperty(heap2);
    printResult(midDecreaseTest, "decreaseKey works for middle elements");
}

void testIncreaseKey() {
    printTestHeader("Test 6: increaseKey()");
    MinHeap heap(10);
    vector<int> values = {5, 10, 15, 20, 25, 30};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Before increaseKey:" << RESET << endl;
    printHeap(heap);
    
    // Increase root from 5 to 35
    cout << BLUE << "Increasing root to 35" << RESET << endl;
    heap.increaseKey(0, 35);
    
    printHeap(heap);
    
    bool newMinTest = (heap.getMin() == 10);
    printResult(newMinTest, "New minimum is correct");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained");
    
    // Test increasing a middle element
    MinHeap heap2(10);
    heap2.insertKey(5);
    heap2.insertKey(10);
    heap2.insertKey(15);
    heap2.insertKey(20);
    
    int sizeBefore = heap2.size();
    heap2.increaseKey(1, 25);
    bool midIncreaseTest = (heap2.size() == sizeBefore) && verifyMinHeapProperty(heap2);
    printResult(midIncreaseTest, "increaseKey works for middle elements");
}

void testDeleteKey() {
    printTestHeader("Test 7: deleteKey()");
    MinHeap heap(10);
    vector<int> values = {10, 20, 15, 30, 25, 40, 35};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Before deleteKey:" << RESET << endl;
    printHeap(heap);
    int initialSize = heap.size();
    int valueToDelete = heap.elementAt(2);
    
    cout << BLUE << "Deleting element at index 2 (value: " << valueToDelete << ")" << RESET << endl;
    heap.deleteKey(2);
    
    printHeap(heap);
    
    bool sizeTest = (heap.size() == initialSize - 1);
    printResult(sizeTest, "Size decreased correctly");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained");
    
    // Verify deleted element is not in heap
    bool notFound = true;
    for (int i = 0; i < heap.size(); i++) {
        if (heap.elementAt(i) == valueToDelete) {
            notFound = false;
            break;
        }
    }
    printResult(notFound, "Deleted element not found in heap");
}

void testDeleteRoot() {
    printTestHeader("Test 8: Delete Root Element");
    MinHeap heap(10);
    vector<int> values = {5, 10, 15, 20, 25};
    
    for (int val : values) {
        heap.insertKey(val);
    }
    
    cout << BLUE << "Before deleting root:" << RESET << endl;
    printHeap(heap);
    
    heap.deleteKey(0);
    
    cout << BLUE << "After deleting root:" << RESET << endl;
    printHeap(heap);
    
    bool newMinTest = (heap.getMin() == 10);
    printResult(newMinTest, "New minimum after root deletion");
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "MinHeap property maintained");
}

void testDynamicResize() {
    printTestHeader("Test 9: Dynamic Capacity Resize");
    
    MinHeap heap(3);
    cout << BLUE << "Initial capacity: 3" << RESET << endl;
    
    heap.insertKey(10);
    heap.insertKey(20);
    heap.insertKey(30);
    
    cout << BLUE << "Heap at initial capacity:" << RESET << endl;
    printHeap(heap);
    
    // Insert beyond initial capacity - should trigger resize
    cout << BLUE << "Inserting beyond capacity..." << RESET << endl;
    heap.insertKey(5);
    heap.insertKey(15);
    heap.insertKey(25);
    
    printHeap(heap);
    
    bool sizeTest = (heap.size() == 6);
    printResult(sizeTest, "Heap dynamically resized to accommodate more elements");
    
    bool contentTest = verifyMinHeapProperty(heap);
    printResult(contentTest, "Heap property maintained after resize");
    
    bool minTest = (heap.getMin() == 5);
    printResult(minTest, "Minimum element correct after resize");
}

void testEmptyHeapOperations() {
    printTestHeader("Test 10: Empty Heap Operations");
    
    MinHeap heap(5);
    
    // Test extractMin on empty heap
    bool emptyExtractTest = (heap.extractMin() == INT_MAX);
    printResult(emptyExtractTest, "extractMin on empty heap returns INT_MAX");
    
    // Test size on empty heap
    bool emptySizeTest = (heap.size() == 0);
    printResult(emptySizeTest, "size() returns 0 for empty heap");
}

void testInvalidOperations() {
    printTestHeader("Test 11: Invalid Index Operations");
    
    MinHeap heap(5);
    heap.insertKey(10);
    heap.insertKey(20);
    heap.insertKey(30);
    
    int sizeBefore = heap.size();
    
    // Test deleteKey with out of bounds index
    heap.deleteKey(10);
    bool invalidDeleteTest = (heap.size() == sizeBefore);
    printResult(invalidDeleteTest, "deleteKey with out of bounds index");
    
    // Test deleteKey with negative index
    heap.deleteKey(-1);
    bool negativeDeleteTest = (heap.size() == sizeBefore);
    printResult(negativeDeleteTest, "deleteKey with negative index");
    
    // Test increaseKey with invalid index
    heap.increaseKey(10, 100);
    bool invalidIncreaseTest = (heap.size() == sizeBefore);
    printResult(invalidIncreaseTest, "increaseKey with invalid index");
    
    bool heapIntact = verifyMinHeapProperty(heap);
    printResult(heapIntact, "Heap remains valid after invalid operations");
}

void testLargeHeap() {
    printTestHeader("Test 12: Large Heap Operations");
    
    MinHeap heap(100);
    
    // Insert 50 values in descending order
    for (int i = 50; i >= 1; i--) {
        heap.insertKey(i);
    }
    
    bool heapProperty = verifyMinHeapProperty(heap);
    printResult(heapProperty, "Large heap maintains min-heap property");
    
    bool minTest = (heap.getMin() == 1);
    printResult(minTest, "Minimum is correct in large heap");
    
    // Extract first 10 elements
    vector<int> extracted;
    for (int i = 0; i < 10; i++) {
        extracted.push_back(heap.extractMin());
    }
    
    bool sortedTest = is_sorted(extracted.begin(), extracted.end());
    printResult(sortedTest, "Extractions maintain sorted order");
}

void testMixedOperations() {
    printTestHeader("Test 13: Mixed Random Operations");
    
    srand(time(0)); 
    
    MinHeap heap(50);
    bool allPassed = true;
    int operationCount = 0;
    
    //random initial elements
    int initialSize = 10 + rand() % 11;
    cout << BLUE << "Inserting " << initialSize << " random elements..." << RESET << endl;
    for (int i = 0; i < initialSize; i++) {
        heap.insertKey(rand() % 100 + 1);
    }
    
    if (!verifyMinHeapProperty(heap)) allPassed = false;
    
    // 20-30 random operations
    int numOperations = 20 + rand() % 11;
    cout << BLUE << "Performing " << numOperations << " random operations..." << RESET << endl;
    
    for (int op = 0; op < numOperations; op++) {
        int operation = rand() % 5;  // 0-4 for different operations
        
        if (operation == 0 && heap.size() < 45) {
            // Insert random value
            int value = rand() % 100 + 1;
            heap.insertKey(value);
            operationCount++;
        }
        else if (operation == 1 && heap.size() > 0) {
            // Extract min
            heap.extractMin();
            operationCount++;
        }
        else if (operation == 2 && heap.size() > 0) {
            // Decrease random key
            int idx = rand() % heap.size();
            int currentVal = heap.elementAt(idx);
            if (currentVal > 1) {
                int newVal = rand() % currentVal;
                heap.decreaseKey(idx, newVal);
                operationCount++;
            }
        }
        else if (operation == 3 && heap.size() > 0) {
            // Increase random key
            int idx = rand() % heap.size();
            int currentVal = heap.elementAt(idx);
            int newVal = currentVal + rand() % 50 + 1;
            heap.increaseKey(idx, newVal);
            operationCount++;
        }
        else if (operation == 4 && heap.size() > 1) {
            // Delete random key
            int idx = rand() % heap.size();
            heap.deleteKey(idx);
            operationCount++;
        }
        
        // Verify heap property after each operation
        if (!verifyMinHeapProperty(heap)) {
            allPassed = false;
            cout << RED << "Heap property violated after operation " << operationCount << RESET << endl;
            break;
        }
    }
    
    cout << BLUE << "Final heap size: " << heap.size() << RESET << endl;
    cout << BLUE << "Operations completed: " << operationCount << RESET << endl;
    
    printResult(allPassed, "Heap property maintained through all random operations");
}


int main() {
    cout << BOLD << MAGENTA << "\n";
    cout << "╔═══════════════════════════════════════════╗\n";
    cout << "║             MIN HEAP TESTS                ║\n";
    cout << "╚═══════════════════════════════════════════╝\n";
    cout << RESET << endl;
    
    testBasicInsertion();
    testInsertionOrder();
    testExtractMin();
    testMultipleExtractions();
    testDecreaseKey();
    testIncreaseKey();
    testDeleteKey();
    testDeleteRoot();
    testDynamicResize();
    testEmptyHeapOperations();
    testInvalidOperations();
    testLargeHeap();
    testMixedOperations();
    
    // Final Results
    cout << endl;
    cout << YELLOW << "======================================" << RESET << endl;
    cout << BOLD << CYAN << "FINAL RESULTS" << RESET << endl;
    cout << YELLOW << "======================================" << RESET << endl;
    cout << BOLD << "Total Tests: " << totalTests << RESET << endl;
    cout << GREEN << "Passed: " << passedTests << RESET << endl;
    cout << RED << "Failed: " << (totalTests - passedTests) << RESET << endl;
    
    double percentage = (totalTests > 0) ? (passedTests * 100.0 / totalTests) : 0.0;
    cout << BOLD << "Score: " << fixed;
    cout.precision(0);
    cout << percentage << "%" << RESET << endl;
    cout << YELLOW << "======================================" << RESET << endl;
    
    if (passedTests == totalTests) {
        cout << GREEN << BOLD << " ALL TESTS PASSED! " << RESET << endl;
    } else {
        cout << YELLOW << "Review failed tests and fix implementation." << RESET << endl;
    }
    
   return static_cast<int>(percentage);
}