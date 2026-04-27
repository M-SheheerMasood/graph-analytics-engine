#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <memory>
#include <thread>
#include <type_traits>
#include "HashTables.h"
using namespace std;

bool timeOut;
promise<bool> done;

void timer(future<bool> done_future)
{
    chrono::seconds span(3);
    if (done_future.wait_for(span) == future_status::timeout)
    {
        timeOut = true;
    }
}

string typeToString(CollisionHandle type)
{
    switch (type)
    {
    case LINEAR_PROBING:
        return "Linear Probing";
    case QUADRATIC_PROBING:
        return "Quadratic Probing";
    case SEPARATE_CHAINING:
        return "Separate Chaining";
    default:
        return "Unknown";
    }
}

double runTestInt(vector<int> keys, vector<int> values, CollisionHandle type, promise<bool> done_future)
{
    using namespace chrono;

    HashTable<int> table(type);
    double score = 0;
    auto startTime = high_resolution_clock::now();

    cout << "\033[1;34mTesting Insertions (" << typeToString(type) << "): \033[0m";

    for (size_t i = 0; i < keys.size(); ++i)
    {
        table.insert(keys[i], values[i]);
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out!\033[0m" << endl;
            done_future.set_value(true);
            return score;
        }
    }

    cout << "\033[1;32mPassed!\033[0m" << endl;

    cout << "\033[1;34mTesting Search (" << typeToString(type) << "):    \033[0m";

    bool searchPassed = true;
    for (size_t i = 0; i < keys.size(); ++i)
    {
        int result = table.search(keys[i]);
        if (result != values[i])
        {
            cout << "\033[1;31mSearch failed for key " << keys[i] << "! Expected: " 
                 << values[i] << ", Got: " << result << "\033[0m" << endl;
            searchPassed = false;
            break;
        }
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out!\033[0m" << endl;
            searchPassed = false;
            break;
        }
    }
    if (searchPassed)
    {
        score += 1;
        cout << "\033[1;32mPassed!\033[0m" << endl;
    }

    cout << "\033[1;34mTesting Removal (" << typeToString(type) << "):   \033[0m";

    if (!searchPassed)
    {
        cout << "\033[1;33mSkipped (Search must pass first)\033[0m" << endl;
    }
    else
    {
        bool removalPassed = true;
        for (size_t i = 0; i < keys.size(); ++i)
        {
            table.remove(keys[i]);
            if (timeOut)
            {
                cout << "\033[1;31mTest timed out!\033[0m" << endl;
                removalPassed = false;
                break;
            }
        }

        if (removalPassed)
        {
            for (size_t i = 0; i < keys.size(); ++i)
            {
                if (table.search(keys[i]) != int())
                {
                    cout << "\033[1;31mRemoval failed! Key " << keys[i] << " still found.\033[0m" << endl;
                    removalPassed = false;
                    break;
                }
                if (timeOut)
                {
                    cout << "\033[1;31mTest timed out!\033[0m" << endl;
                    removalPassed = false;
                    break;
                }
            }
        }
        
        if (removalPassed)
        {
            score += 1;
            cout << "\033[1;32mPassed!\033[0m" << endl;
        }
    }

    // ====================== TESTING REHASHING ====================== //
    cout << "\033[1;34mTesting Rehashing (" << typeToString(type) << "): \033[0m";
    
    int initialCapacity = table.getTableSize();
    int numNewElements = initialCapacity * 1.5;

    vector<int> rehashKeys;
    vector<int> rehashValues;
    
    bool rehashInsertSuccess = true;
    for (int i = 0; i < numNewElements; ++i)
    {
        int newKey = 1000 + i;
        int newValue = newKey;

        rehashKeys.push_back(newKey);
        rehashValues.push_back(newValue);
        table.insert(newKey, newValue);
        
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out during rehashing!\033[0m" << endl;
            rehashInsertSuccess = false;
            break;
        }
    }

    bool rehashOccurred = false;
    if (rehashInsertSuccess && table.getTableSize() > initialCapacity)
    {
        cout << "\n";
        cout << "\033[1;32mRehashing occurred! Old size: " << initialCapacity 
             << ", New size: " << table.getTableSize() << "\033[0m" << endl;
        score += 1;
        rehashOccurred = true;
    }
    else
    {
        cout << "\033[1;31mRehashing did not occur as expected! Size remained: " 
             << table.getTableSize() << "\033[0m" << endl;
    }

    if (rehashOccurred)
    {
        cout << "\033[1;34mVerifying data integrity after rehashing...\033[0m" << endl;
        bool verificationPassed = true;
        for (size_t i = 0; i < rehashKeys.size(); ++i)
        {
            int result = table.search(rehashKeys[i]);
            if (result != rehashValues[i])
            {
                cout << "\033[1;31mRehashing verification failed! Key " << rehashKeys[i] 
                     << " Expected: " << rehashValues[i] << ", Got: " << result << "\033[0m" << endl;
                verificationPassed = false;
                break;
            }
        }
        
        if (verificationPassed)
        {
            score += 2;
            cout << "\033[1;32mRehashing Verification Passed!\033[0m" << endl;
        }
    }

    done_future.set_value(true);
    auto endTime = high_resolution_clock::now();
    duration<double> totalTime = duration_cast<duration<double>>(endTime - startTime);

    cout << "\033[1;36mTest completed in: " << totalTime.count() << " seconds.\033[0m" << endl;
    return score;
}

double runTestStr(vector<int> keys, vector<string> values, CollisionHandle type, promise<bool> done_future)
{
    using namespace chrono;

    HashTable<string> table(type);
    double score = 0;
    auto startTime = high_resolution_clock::now();

    cout << "\033[1;34mTesting Insertions (" << typeToString(type) << "): \033[0m";

    for (size_t i = 0; i < keys.size(); ++i)
    {
        table.insert(keys[i], values[i]);
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out!\033[0m" << endl;
            done_future.set_value(true);
            return score;
        }
    }

    cout << "\033[1;32mPassed!\033[0m" << endl;

    cout << "\033[1;34mTesting Search (" << typeToString(type) << "):    \033[0m";

    bool searchPassed = true;
    for (size_t i = 0; i < keys.size(); ++i)
    {
        string result = table.search(keys[i]);
        if (result != values[i])
        {
            cout << "\033[1;31mSearch failed for key " << keys[i] << "! Expected: " 
                 << values[i] << ", Got: " << result << "\033[0m" << endl;
            searchPassed = false;
            break;
        }
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out!\033[0m" << endl;
            searchPassed = false;
            break;
        }
    }
    if (searchPassed)
    {
        score += 1;
        cout << "\033[1;32mPassed!\033[0m" << endl;
    }

    cout << "\033[1;34mTesting Removal (" << typeToString(type) << "):   \033[0m";

    bool removalPassed = true;
    for (size_t i = 0; i < keys.size(); ++i)
    {
        table.remove(keys[i]);
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out!\033[0m" << endl;
            removalPassed = false;
            break;
        }
    }

    if (removalPassed)
    {
        for (size_t i = 0; i < keys.size(); ++i)
        {
            if (table.search(keys[i]) != string())
            {
                cout << "\033[1;31mRemoval failed! Key " << keys[i] << " still found.\033[0m" << endl;
                removalPassed = false;
                break;
            }
            if (timeOut)
            {
                cout << "\033[1;31mTest timed out!\033[0m" << endl;
                removalPassed = false;
                break;
            }
        }
    }
    
    if (removalPassed)
    {
        score += 1;
        cout << "\033[1;32mPassed!\033[0m" << endl;
    }

    // ====================== TESTING REHASHING ====================== //
    cout << "\033[1;34mTesting Rehashing (" << typeToString(type) << "): \033[0m";
    
    int initialCapacity = table.getTableSize();
    int numNewElements = initialCapacity * 1.5;

    vector<int> rehashKeys;
    vector<string> rehashValues;
    
    bool rehashInsertSuccess = true;
    for (int i = 0; i < numNewElements; ++i)
    {
        int newKey = 1000 + i;
        string newValue = "User" + to_string(newKey);

        rehashKeys.push_back(newKey);
        rehashValues.push_back(newValue);
        table.insert(newKey, newValue);
        
        if (timeOut)
        {
            cout << "\033[1;31mTest timed out during rehashing!\033[0m" << endl;
            rehashInsertSuccess = false;
            break;
        }
    }

    bool rehashOccurred = false;
    if (rehashInsertSuccess && table.getTableSize() > initialCapacity)
    {
        cout << "\n";
        cout << "\033[1;32mRehashing occurred! Old size: " << initialCapacity 
             << ", New size: " << table.getTableSize() << "\033[0m" << endl;
        score += 1;
        rehashOccurred = true;
    }
    else
    {
        cout << "\033[1;31mRehashing did not occur as expected! Size remained: " 
             << table.getTableSize() << "\033[0m" << endl;
    }

    if (rehashOccurred)
    {
        cout << "\033[1;34mVerifying data integrity after rehashing...\033[0m" << endl;
        bool verificationPassed = true;
        for (size_t i = 0; i < rehashKeys.size(); ++i)
        {
            string result = table.search(rehashKeys[i]);
            if (result != rehashValues[i])
            {
                cout << "\033[1;31mRehashing verification failed! Key " << rehashKeys[i] 
                     << " Expected: " << rehashValues[i] << ", Got: " << result << "\033[0m" << endl;
                verificationPassed = false;
                break;
            }
        }
        
        if (verificationPassed)
        {
            score += 2;
            cout << "\033[1;32mRehashing Verification Passed!\033[0m" << endl;
        }
    }

    done_future.set_value(true);
    auto endTime = high_resolution_clock::now();
    duration<double> totalTime = duration_cast<duration<double>>(endTime - startTime);

    cout << "\033[1;36mTest completed in: " << totalTime.count() << " seconds.\033[0m" << endl;
    return score;
}

int main()
{
    cout << "\033[1;35m========================================\033[0m" << endl;
    cout << "\033[1;35m   Hash Table Testing Suite \033[0m" << endl;
    cout << "\033[1;35m========================================\033[0m" << endl;
    cout << endl;

    vector<int> intKeys = {10, 21, 32, 43, 54, 65, 76, 87, 98, 109};
    vector<int> intValues = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

    vector<int> strKeys = {10, 21, 32, 43, 54, 65, 76, 87, 98, 109};
    vector<string> strValues = {"Alice", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Hannah", "Ivy", "John"};

    CollisionHandle collisionTypes[] = {LINEAR_PROBING, QUADRATIC_PROBING, SEPARATE_CHAINING};

    double totalScore = 0;

    for (auto type : collisionTypes)
    {
        timeOut = false;
        promise<bool> doneIntPromise;
        future<bool> doneIntFuture = doneIntPromise.get_future();
        thread intTimer(timer, move(doneIntFuture));

        cout << "\033[1;35m========================================\033[0m" << endl;
        cout << "\033[1;35mTesting " << typeToString(type) << " with integer values...\033[0m" << endl;
        cout << "\033[1;35m========================================\033[0m" << endl;
        double intScore = runTestInt(intKeys, intValues, type, move(doneIntPromise));
        intTimer.join();

        timeOut = false;
        promise<bool> doneStrPromise;
        future<bool> doneStrFuture = doneStrPromise.get_future();
        thread strTimer(timer, move(doneStrFuture));

        cout << "\033[1;33m----------------------------------------------\033[0m" << endl;
        cout << "\033[1;35mTesting " << typeToString(type) << " with string values...\033[0m" << endl;
        cout << "\033[1;33m----------------------------------------------\033[0m" << endl;
        double strScore = runTestStr(strKeys, strValues, type, move(doneStrPromise));
        strTimer.join();

        double collisionScore = intScore + strScore;
        totalScore += collisionScore;
        cout << "\033[1;33m----------------------------------------------\033[0m" << endl;
        cout << "\033[1;36mTest results for " << typeToString(type) << ":\033[0m" << endl;
        cout << "\033[1;32mInteger Test Score: " << intScore << "/5\033[0m" << endl;
        cout << "\033[1;32mString Test Score: " << strScore << "/5\033[0m" << endl;
        cout << "\033[1;32mCombined Test Score: " << collisionScore << "/10\033[0m" << endl;
        cout << "\033[1;33m----------------------------------------------\033[0m" << endl;
        cout << endl;
    }

    cout << "\033[1;35m========================================\033[0m" << endl;
    cout << "\033[1;36mTotal Score: " << totalScore << "/30\033[0m" << endl;
    cout << "\033[1;42;30m FINAL SCORE: " << totalScore << "/30 \033[0m" << endl;
    cout << "\033[1;35m========================================\033[0m" << endl;

    return static_cast<int>(totalScore);
}