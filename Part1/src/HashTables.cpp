#include "HashTables.h"

// =======================
// Constructor
// =======================
template <typename T>
HashTable<T>::HashTable(CollisionHandle strategy)
{
    collision_strategy = strategy;
    if (collision_strategy == SEPARATE_CHAINING)
    {
        chaining_table.resize(10);
    }
    else
    {
        probing_table = new KeyValuePair[10];
    }
    table_size = 10;
}

// =======================
// Destructor
// =======================
template <typename T>
HashTable<T>::~HashTable()
{
    delete[] probing_table;
}

// =======================
// Load Factor Calculation
// =======================
template <typename T>
void HashTable<T>::resizeAndRehash()
{
    int oldsize = table_size;
    table_size *= 2;
    if (collision_strategy == SEPARATE_CHAINING)
    {
        vector<vector<KeyValuePair>> newtable = vector<vector<KeyValuePair>>();
        newtable.resize(table_size);
        for (int i = 0; i < oldsize; i++)
        {
            for (int j = 0; j < chaining_table[i].size(); j++)
            {
                int idx = hashFunction1(chaining_table[i][j].key);
                newtable[idx].push_back(KeyValuePair(chaining_table[i][j].key, chaining_table[i][j].value, false));
            }
        }
        chaining_table = newtable;
    }
    else
    {
        KeyValuePair *newtable = new KeyValuePair[table_size];
        if (collision_strategy == LINEAR_PROBING)
        {
            for (int s = 0; s < oldsize; s++)
            {
                if (!probing_table[s].isEmpty)
                {
                    int idx = hashFunction1(probing_table[s].key);
                    for (int i = 0; i < table_size; i++)
                    {
                        int j = (idx + i) % table_size;
                        if (newtable[j].isEmpty)
                        {
                            newtable[j] = KeyValuePair(probing_table[s].key, probing_table[s].value, false);
                            break;
                        }
                    }
                }
            }
            delete[] probing_table;
            probing_table = newtable;
        }
        else if (collision_strategy == QUADRATIC_PROBING)
        {
            for (int s = 0; s < oldsize; s++)
            {
                if (!probing_table[s].isEmpty)
                {
                    int idx = hashFunction1(probing_table[s].key);
                    for (int i = 0; i < table_size; i++)
                    {
                        int j = (idx + (i * i)) % table_size;
                        if (newtable[j].isEmpty)
                        {
                            newtable[j] = KeyValuePair(probing_table[s].key, probing_table[s].value, false);
                            break;
                        }
                    }
                }
            }
            delete[] probing_table;
            probing_table = newtable;
        }
    }
    loadFactor = float(num_elements) / float(table_size);
}

template <typename T>
void HashTable<T>::calculateLoadFactor()
{
    loadFactor = float(num_elements) / float(table_size);
    if (loadFactor >= loadFactorThreshold)
    {
        resizeAndRehash();
    }
}

// =======================
// Linear Probing Methods
// =======================
template <typename T>
void HashTable<T>::insertLinearProbing(int key, T value)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + i) % table_size;
        if (probing_table[j].isEmpty || probing_table[j].isdeleted)
        {
            probing_table[j] = KeyValuePair(key, value, false);
            num_elements++;
            return;
        }
    }
}

template <typename T>
T HashTable<T>::searchLinearProbing(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + i) % table_size;
        if (probing_table[j].isEmpty && !probing_table[j].isdeleted)
        {
            break;
        }
        if (probing_table[j].key == key && !probing_table[j].isdeleted)
        {
            return probing_table[j].value;
        }
    }
    return T();
}

template <typename T>
void HashTable<T>::removeLinearProbing(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + i) % table_size;
        if (probing_table[j].isEmpty && !probing_table[j].isdeleted)
        {
            break;
        }
        if (probing_table[j].key == key && !probing_table[j].isEmpty)
        {
            probing_table[j].isdeleted = true;
            num_elements--;
            return;
        }
    }
}

// =======================
// Quadratic Probing Methods
// =======================
template <typename T>
void HashTable<T>::insertQuadraticProbing(int key, T value)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + (i * i)) % table_size;
        if (probing_table[j].isEmpty || probing_table[j].isdeleted)
        {
            probing_table[j] = KeyValuePair(key, value, false);
            num_elements++;
            return;
        }
    }
}

template <typename T>
T HashTable<T>::searchQuadraticProbing(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + (i * i)) % table_size;
        if (probing_table[j].isEmpty && !probing_table[j].isdeleted)
        {
            break;
        }
        if (probing_table[j].key == key && !probing_table[j].isdeleted)
        {
            return probing_table[j].value;
        }
    }
    return T();
}

template <typename T>
void HashTable<T>::removeQuadraticProbing(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < table_size; i++)
    {
        int j = (idx + (i * i)) % table_size;
        if (probing_table[j].isEmpty && !probing_table[j].isdeleted)
        {
            break;
        }
        if (probing_table[j].key == key && !probing_table[j].isEmpty)
        {
            probing_table[j].isdeleted = true;
            num_elements--;
            return;
        }
    }
}

// =======================
// Separate Chaining Methods
// =======================
template <typename T>
void HashTable<T>::insertSeparateChaining(int key, T value)
{
    int idx = hashFunction1(key);
    chaining_table[idx].push_back(KeyValuePair(key, value, false));
    num_elements++;
}

template <typename T>
T HashTable<T>::searchSeparateChaining(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < chaining_table[idx].size(); i++)
    {
        if (chaining_table[idx][i].key == key)
        {
            return chaining_table[idx][i].value;
        }
    }
    return T();
}

template <typename T>
void HashTable<T>::removeSeparateChaining(int key)
{
    int idx = hashFunction1(key);
    for (int i = 0; i < chaining_table[idx].size(); i++)
    {
        if (chaining_table[idx][i].key == key)
        {
            chaining_table[idx].erase(chaining_table[idx].begin() + i);
            num_elements--;
        }
    }
}

// =======================
// Insert
// =======================
template <typename T>
void HashTable<T>::insert(int key, T value)
{
    switch (collision_strategy)
    {
    case LINEAR_PROBING:
        insertLinearProbing(key, value);
        break;
    case QUADRATIC_PROBING:
        insertQuadraticProbing(key, value);
        break;
    case SEPARATE_CHAINING:
        insertSeparateChaining(key, value);
        break;
    }
    calculateLoadFactor();
}

// =======================
// Search
// =======================
template <typename T>
T HashTable<T>::search(int key)
{
    switch (collision_strategy)
    {
    case LINEAR_PROBING:
        return searchLinearProbing(key);
    case QUADRATIC_PROBING:
        return searchQuadraticProbing(key);
    case SEPARATE_CHAINING:
        return searchSeparateChaining(key);
    }
    return T(); // Default return
}

// =======================
// Remove
// =======================
template <typename T>
void HashTable<T>::remove(int key)
{
    switch (collision_strategy)
    {
    case LINEAR_PROBING:
        removeLinearProbing(key);
        break;
    case QUADRATIC_PROBING:
        removeQuadraticProbing(key);
        break;
    case SEPARATE_CHAINING:
        removeSeparateChaining(key);
        break;
    }
    calculateLoadFactor();
}

// Please use this for debugging help
template <typename T>
void HashTable<T>::displayProbingTable()
{
    cout << "Current Table (size: " << table_size << "):\n";
    for (int i = 0; i < table_size; i++)
    {
        if (!probing_table[i].isEmpty)
            cout << "[" << i << "] -> Key: " << probing_table[i].key << ", Value: " << probing_table[i].value << endl;
        else
            cout << "[" << i << "] -> EMPTY\n";
    }
}

template class HashTable<int>;
template class HashTable<string>;