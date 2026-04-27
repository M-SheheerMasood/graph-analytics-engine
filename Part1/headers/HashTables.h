#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
#include <vector>
using namespace std;

enum CollisionHandle                                        // enumeration to help handle the collision handling strategies.
{
    LINEAR_PROBING,
    QUADRATIC_PROBING,
    SEPARATE_CHAINING
};

template <typename T>
class HashTable
{
    private:
        CollisionHandle collision_strategy;
    
        int table_size = 0;                                 // size of the hash table.
        int num_elements = 0;                               // number of elements in the hash table.
        float loadFactor = 0.0;
        float loadFactorThreshold = 0.85;

        struct KeyValuePair
        {
            int key;                                        // key used to hash the value.
            T value;                                        // value associated with the key.
            bool isEmpty;                                   // flag to indicate if the slot is empty.
            bool isdeleted;

            KeyValuePair() : key(0), value(T()), isEmpty(true), isdeleted(false) {}
            KeyValuePair(int k, T v, bool empty) : key(k), value(v), isEmpty(empty), isdeleted(false) {}

            // overload equality operator for ease.
            bool operator==(const KeyValuePair &other) const
            {
                return key == other.key && value == other.value && isEmpty == other.isEmpty;
            }
        };

        KeyValuePair* probing_table = nullptr;

        vector<vector<KeyValuePair>> chaining_table;       // hash table for separate chaining (vector of vectors).

        int hashFunction1(int key){return key%table_size;};                         // primary hash function (used for all methods).

        void calculateLoadFactor();

        // methods for Linear Probing.
        void insertLinearProbing(int key, T value);
        T searchLinearProbing(int key);
        void removeLinearProbing(int key);

        // methods for Quadratic Probing.
        void insertQuadraticProbing(int key, T value);
        T searchQuadraticProbing(int key);
        void removeQuadraticProbing(int key);

        // methods for Separate Chaining.
        void insertSeparateChaining(int key, T value);
        T searchSeparateChaining(int key);
        void removeSeparateChaining(int key);

        void resizeAndRehash();

    public:
        HashTable(CollisionHandle strategy = LINEAR_PROBING);       // constructor to initialize the hash table and choose collision handling strategy.

        ~HashTable();                                               // destructor.

        void insert(int key, T value);                              // insert a key-value pair based on the chosen collision handling strategy.

        T search(int key);                                          // search for a key and return its associated value based on the chosen strategy.

        void remove(int key);                                       // remove a key-value pair based on the chosen strategy.

        void displayProbingTable();                                             // Please use this for debugging help

        int getTableSize() {return table_size;}                     // DO NOT MODIFY.

        int getNumberElements() {return num_elements;}              // DO NOT MODIFY.
};


#endif