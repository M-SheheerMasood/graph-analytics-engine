#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "../headers/SocialMediaSystem.h"

using namespace std;

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

void testBuildFeed() {
    printTestHeader("Test 1: Build Undiscovered Feed");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75, 150};
    
    cout << BLUE << "Building feed with likes: ";
    for (int l : likes) cout << l << " ";
    cout << RESET << endl;
    
    sms.buildUndiscoveredFeed(likes);
    
    vector<int> allPosts = sms.getTopNPosts(10);
    
    bool sizeTest = (allPosts.size() == 5);
    printResult(sizeTest, "All posts added to feed");
    
    bool minTest = (allPosts[0] == 50);
    printResult(minTest, "Minimum engagement post identified");
}

void testBuildEmptyFeed() {
    printTestHeader("Test 2: Build Empty Feed");
    SocialMediaSystem sms;
    vector<int> likes = {};
    
    sms.buildUndiscoveredFeed(likes);
    
    vector<int> allPosts = sms.getTopNPosts(1);
    bool emptyTest = (allPosts.size() == 0);
    printResult(emptyTest, "Empty feed handled correctly");
}

void testBuildLargeFeed() {
    printTestHeader("Test 3: Build Large Feed");
    SocialMediaSystem sms;
    vector<int> likes;
    
    //50 posts
    for (int i = 1; i <= 50; i++) {
        likes.push_back(i * 10);
    }
    
    cout << BLUE << "Building feed with 50 posts..." << RESET << endl;
    sms.buildUndiscoveredFeed(likes);
    
    vector<int> allPosts = sms.getTopNPosts(100);
    
    bool sizeTest = (allPosts.size() == 50);
    printResult(sizeTest, "Large feed built correctly");
    
    bool minTest = (allPosts[0] == 10);
    printResult(minTest, "Minimum post correct in large feed");
}

void testFindPostIndex() {
    printTestHeader("Test 4: Find Post Index");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75, 150};
    
    sms.buildUndiscoveredFeed(likes);
    
    int idx1 = sms.findPostIndex(50);
    bool foundTest = (idx1 != -1);
    printResult(foundTest, "Existing post found");
    
    int idx2 = sms.findPostIndex(999);
    bool notFoundTest = (idx2 == -1);
    printResult(notFoundTest, "Non-existent post returns -1");
    
    int idx3 = sms.findPostIndex(200);
    bool found200 = (idx3 != -1);
    printResult(found200, "Another existing post found");
}

void testUpdatePostLikes() {
    printTestHeader("Test 5: Update Post Likes (Increase)");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Before update - Top posts: ";
    vector<int> before = sms.getTopNPosts(4);
    for (int l : before) cout << l << " ";
    cout << RESET << endl;
    
    cout << BLUE << "Updating post with 50 likes to 250 likes" << RESET << endl;
    sms.updatePostLikes(50, 250);
    
    cout << BLUE << "After update - Top posts: ";
    vector<int> after = sms.getTopNPosts(4);
    for (int l : after) cout << l << " ";
    cout << RESET << endl;
    
    bool minTest = (after[0] == 75);
    printResult(minTest, "Minimum updated correctly");
    
    bool updatedInFeed = (find(after.begin(), after.end(), 250) != after.end());
    printResult(updatedInFeed, "Updated value present in feed");
    
    bool oldValueGone = (find(after.begin(), after.end(), 50) == after.end());
    printResult(oldValueGone, "Old value removed from feed");
}

void testUpdateNonExistentPost() {
    printTestHeader("Test 6: Update Non-Existent Post");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200};
    
    sms.buildUndiscoveredFeed(likes);
    
    sms.updatePostLikes(999, 1000); 

    vector<int> after = sms.getTopNPosts(5);
    bool sizeUnchanged = (after.size() == 3);
    printResult(sizeUnchanged, "Feed unchanged when updating non-existent post");
}

void testDecreasePostLikes() {
    printTestHeader("Test 7: Decrease Post Likes");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Before decrease - Top posts: ";
    vector<int> before = sms.getTopNPosts(4);
    for (int l : before) cout << l << " ";
    cout << RESET << endl;
    
    cout << BLUE << "Decreasing post with 200 likes to 40 likes" << RESET << endl;
    sms.decreasePostLikes(200, 40);
    
    cout << BLUE << "After decrease - Top posts: ";
    vector<int> after = sms.getTopNPosts(4);
    for (int l : after) cout << l << " ";
    cout << RESET << endl;
    
    bool minTest = (after[0] == 40);
    printResult(minTest, "New minimum correct after decrease");
    
    bool decreasedInFeed = (find(after.begin(), after.end(), 40) != after.end());
    printResult(decreasedInFeed, "Decreased value present in feed");
    
    bool oldValueGone = (find(after.begin(), after.end(), 200) == after.end());
    printResult(oldValueGone, "Old value removed from feed");
}

void testDecreaseNonExistentPost() {
    printTestHeader("Test 8: Decrease Non-Existent Post");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200};
    
    sms.buildUndiscoveredFeed(likes);
    
    sms.decreasePostLikes(999, 10);  
    
    vector<int> after = sms.getTopNPosts(5);
    bool sizeUnchanged = (after.size() == 3);
    printResult(sizeUnchanged, "Feed unchanged when decreasing non-existent post");
}

void testClearLowEngagement() {
    printTestHeader("Test 9: Clear Low Engagement Posts");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75, 30, 150, 20, 45};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Before clearing - All posts: ";
    vector<int> before = sms.getTopNPosts(10);
    for (int l : before) cout << l << " ";
    cout << RESET << endl;
    
    cout << BLUE << "Clearing posts with less than 60 likes..." << RESET << endl;
    sms.clearLowEngagementPosts(60);
    
    cout << BLUE << "After clearing - Remaining posts: ";
    vector<int> after = sms.getTopNPosts(10);
    for (int l : after) cout << l << " ";
    cout << RESET << endl;
    
    bool correctCount = (after.size() == 4);
    printResult(correctCount, "Correct number of posts remaining");
    
    bool allAboveThreshold = true;
    for (int likes : after) {
        if (likes < 60) {
            allAboveThreshold = false;
            break;
        }
    }
    printResult(allAboveThreshold, "All remaining posts above threshold");
    
    bool minCorrect = (after.size() > 0 && after[0] == 75);
    printResult(minCorrect, "New minimum correct");
}

void testClearAllPosts() {
    printTestHeader("Test 10: Clear All Posts");
    SocialMediaSystem sms;
    vector<int> likes = {10, 20, 30, 40};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Clearing all posts with threshold 100..." << RESET << endl;
    sms.clearLowEngagementPosts(100);
    
    vector<int> after = sms.getTopNPosts(5);
    bool allCleared = (after.size() == 0);
    printResult(allCleared, "All posts cleared when all below threshold");
}

void testClearNoPosts() {
    printTestHeader("Test 11: Clear No Posts");
    SocialMediaSystem sms;
    vector<int> likes = {100, 200, 300};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Clearing posts with threshold 50..." << RESET << endl;
    sms.clearLowEngagementPosts(50);
    
    vector<int> after = sms.getTopNPosts(5);
    bool noneCleared = (after.size() == 3);
    printResult(noneCleared, "No posts cleared when all above threshold");
}

void testGetTopNPosts() {
    printTestHeader("Test 12: Get Top N Posts");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200, 75, 150, 25, 300};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Getting top 3 posts..." << RESET << endl;
    vector<int> top3 = sms.getTopNPosts(3);
    
    cout << BLUE << "Top 3 posts: ";
    for (int l : top3) cout << l << " ";
    cout << RESET << endl;
    
    bool correctSize = (top3.size() == 3);
    printResult(correctSize, "Returns correct number of posts");
    
    bool sortedTest = is_sorted(top3.begin(), top3.end());
    printResult(sortedTest, "Posts returned in ascending order");
    
    bool firstIsMin = (top3[0] == 25);
    printResult(firstIsMin, "First post is minimum engagement");
    
    vector<int> afterCheck = sms.getTopNPosts(10);
    bool feedUnchanged = (afterCheck.size() == 7);
    printResult(feedUnchanged, "Original feed unchanged after getTopNPosts");
}

void testGetTopNMoreThanAvailable() {
    printTestHeader("Test 13: Get Top N (N > Available)");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Getting top 10 posts (only 3 available)..." << RESET << endl;
    vector<int> result = sms.getTopNPosts(10);
    
    bool correctSize = (result.size() == 3);
    printResult(correctSize, "Returns all available posts when N > size");
}

void testGetTopNZero() {
    printTestHeader("Test 14: Get Top N Posts (N = 0)");
    SocialMediaSystem sms;
    vector<int> likes = {100, 50, 200};
    
    sms.buildUndiscoveredFeed(likes);
    
    vector<int> result = sms.getTopNPosts(0);
    
    bool emptyResult = (result.size() == 0);
    printResult(emptyResult, "Returns empty vector when N = 0");
}

void testComplexScenario1() {
    printTestHeader("Test 15: Complex Scenario - Multiple Updates");
    SocialMediaSystem sms;
    vector<int> likes = {150, 200, 50, 100, 75};
    
    sms.buildUndiscoveredFeed(likes);
    
    cout << BLUE << "Initial feed" << RESET << endl;
    
    //multiple operations
    sms.updatePostLikes(50, 400);   
    sms.decreasePostLikes(200, 80);  
    sms.clearLowEngagementPosts(100); 
    
    vector<int> final = sms.getTopNPosts(10);
    
    cout << BLUE << "Final posts: ";
    for (int l : final) cout << l << " ";
    cout << RESET << endl;
    
    bool correctSize = (final.size() == 3);
    printResult(correctSize, "Correct number of posts after complex operations");
    
    bool hasViralPost = (find(final.begin(), final.end(), 400) != final.end());
    printResult(hasViralPost, "Viral post (updated to 400) present");
    
    bool lowEngagementRemoved = (find(final.begin(), final.end(), 75) == final.end());
    printResult(lowEngagementRemoved, "Low engagement posts removed");
}

void testComplexScenario2() {
    printTestHeader("Test 16: Complex Scenario - Build and Clear");
    SocialMediaSystem sms;
    
    vector<int> likes1 = {100, 50, 200};
    sms.buildUndiscoveredFeed(likes1);
    
    vector<int> likes2 = {75, 125, 25};
    sms.buildUndiscoveredFeed(likes2);
    
    vector<int> allPosts = sms.getTopNPosts(10);
    bool correctTotal = (allPosts.size() == 6);
    printResult(correctTotal, "Multiple builds accumulate posts");
    
    sms.clearLowEngagementPosts(1000);  
    
    vector<int> likes3 = {500, 600, 700};
    sms.buildUndiscoveredFeed(likes3);
    
    vector<int> newFeed = sms.getTopNPosts(10);
    bool rebuiltCorrectly = (newFeed.size() == 3);
    printResult(rebuiltCorrectly, "Feed rebuilt correctly after clearing");
}

void testSequentialOperations() {
    printTestHeader("Test 18: Sequential Operations Stress Test");
    SocialMediaSystem sms;
    
    vector<int> likes = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    sms.buildUndiscoveredFeed(likes);
    
    bool allPassed = true;
    
    //many operations in sequence
    sms.updatePostLikes(10, 110);
    sms.updatePostLikes(20, 120);
    sms.decreasePostLikes(100, 15);
    sms.decreasePostLikes(90, 25);
    sms.clearLowEngagementPosts(35);
    
    vector<int> result = sms.getTopNPosts(20);
    
    cout << BLUE << "Final posts after sequential operations: ";
    for (int l : result) cout << l << " ";
    cout << RESET << endl;
    
    for (int likes : result) {
        if (likes < 35) {
            allPassed = false;
            break;
        }
    }
    
    printResult(allPassed, "All operations executed correctly in sequence");
    
    bool has110 = (find(result.begin(), result.end(), 110) != result.end());
    bool has120 = (find(result.begin(), result.end(), 120) != result.end());
    printResult(has110 && has120, "Updated posts present in final feed");
}

void testRandomOperations() {
    printTestHeader("Test 19: Random Operations Stress Test");
    
    srand(time(0));
    
    SocialMediaSystem sms;
    bool allPassed = true;
    
    //initial feed with random likes
    vector<int> likes;
    int initialSize = 15 + rand() % 11; 
    cout << BLUE << "Building feed with " << initialSize << " posts..." << RESET << endl;
    
    for (int i = 0; i < initialSize; i++) {
        int likeCount = 50 + rand() % 200;  
        likes.push_back(likeCount);
    }
    sms.buildUndiscoveredFeed(likes);
    
    //random operations
    int numOps = 10 + rand() % 11;  
    cout << BLUE << "Performing " << numOps << " random operations..." << RESET << endl;
    
    for (int op = 0; op < numOps; op++) {
        int operation = rand() % 3;
        vector<int> currentPosts = sms.getTopNPosts(100);
        
        if (currentPosts.size() == 0) break;
        
        if (operation == 0) {
            int randomPost = currentPosts[rand() % currentPosts.size()];
            int newLikes = randomPost + rand() % 100 + 1;
            sms.updatePostLikes(randomPost, newLikes);
        }
        else if (operation == 1) {
            int randomPost = currentPosts[rand() % currentPosts.size()];
            if (randomPost > 10) {
                int newLikes = rand() % randomPost;
                sms.decreasePostLikes(randomPost, newLikes);
            }
        }
        else {
            int threshold = 50 + rand() % 50;  // 50-99
            sms.clearLowEngagementPosts(threshold);
        }
    }
    
    vector<int> finalPosts = sms.getTopNPosts(100);
    cout << BLUE << "Final feed size: " << finalPosts.size() << RESET << endl;
    
    bool stillValid = is_sorted(finalPosts.begin(), finalPosts.end());
    
    printResult(stillValid, "Feed maintains integrity through random operations");
}

int main() {
    cout << BOLD << MAGENTA << "\n";
    cout << "╔═══════════════════════════════════════════╗\n";
    cout << "║         SOCIAL MEDIA SYSTEM TESTS         ║\n";
    cout << "╚═══════════════════════════════════════════╝\n";
    cout << RESET << endl;
    
    testBuildFeed();
    testBuildEmptyFeed();
    testBuildLargeFeed();
    testFindPostIndex();
    testUpdatePostLikes();
    testUpdateNonExistentPost();
    testDecreasePostLikes();
    testDecreaseNonExistentPost();
    testClearLowEngagement();
    testClearAllPosts();
    testClearNoPosts();
    testGetTopNPosts();
    testGetTopNMoreThanAvailable();
    testGetTopNZero();
    testComplexScenario1();
    testComplexScenario2();
    testSequentialOperations();
    testRandomOperations();
    
    // Final Results
    cout << endl;
    cout << YELLOW << "======================================" << RESET << endl;
    cout << BOLD << CYAN << "FINAL RESULTS" << RESET << endl;
    cout << YELLOW << "======================================" << RESET << endl;
    cout << BOLD << "Total Tests: " << totalTests << RESET << endl;
    cout << GREEN << "Passed: " << passedTests << RESET << endl;
    cout << RED << "Failed: " << (totalTests - passedTests) << RESET << endl;
    
    double percentage = (totalTests > 0) ? (passedTests * 100 / totalTests) : 0;
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