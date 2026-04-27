#include "Heap.h"
#include <iostream>
#include <climits>

using namespace std;

MinHeap::MinHeap(int cap)
{
    capacity = cap;
    if (capacity > 0)
    {
        harr = new int[capacity];
    }
    heap_size = 0;
}

MinHeap::~MinHeap()
{
    delete[] harr;
}

MinHeap::MinHeap(const MinHeap &other)
{
    capacity = other.capacity;
    heap_size = other.heap_size;
    if (capacity > 0)
    {
        harr = new int[capacity];
    }
    for (int i = 0; i < heap_size; i++)
    {
        harr[i] = other.harr[i];
    }
}

MinHeap &MinHeap::operator=(const MinHeap &other)
{
    if (this != &other)
    {
        delete[] harr;
        capacity = other.capacity;
        heap_size = other.heap_size;
        if (capacity > 0)
        {
            harr = new int[capacity];
        }
        for (int i = 0; i < heap_size; i++)
        {
            harr[i] = other.harr[i];
        }
    }
    return *this;
}

int MinHeap::extractMin()
{
    if (heap_size > 0)
    {
        int ret = harr[0];
        swap(&harr[0], &harr[heap_size - 1]);
        heap_size--;
        minHeapify(0);
        return ret;
    }
    return INT_MAX;
}

void MinHeap::decreaseKey(int i, int new_val)
{
    if (i < heap_size && i >= 0)
    {
        if (harr[i] > new_val && new_val >= 0)
        {
            harr[i] = new_val;
            minHeapify(i);
        }
    }
}

void MinHeap::increaseKey(int i, int new_val)
{
    if (i < heap_size && i >= 0)
    {
        if (harr[i] < new_val)
        {
            harr[i] = new_val;
            minHeapify(i);
        }
    }
}

int MinHeap::getMin() const
{
    if (heap_size > 0)
    {
        return harr[0];
    }
    return INT_MAX;
}

void MinHeap::deleteKey(int i)
{
    if (i < heap_size && i >= 0)
    {
        harr[i] = INT_MIN;
        minHeapify(i);
        extractMin();
    }
}

void MinHeap::insertKey(int k)
{
    if (k >= 0)
    {
        if (heap_size == capacity)
        {
            capacity *= 2;
            int *newarr = new int[capacity];
            for (int i = 0; i < heap_size; i++)
            {
                newarr[i] = harr[i];
            }
            delete[] harr;
            harr = newarr;
        }
        harr[heap_size] = k;
        heap_size++;
        minHeapify(heap_size - 1);
    }
}

void MinHeap::minHeapify(int i)
{
    if (i < heap_size)
    {
        int p = parent(i);
        if (i > 0 && harr[p] > harr[i])
        {
            swap(&harr[i], &harr[p]);
            minHeapify(p);
            return;
        }
        int l = left(i);
        int r = right(i);
        int smol = i;
        if (l < heap_size && harr[l] < harr[smol])
        {
            smol = l;
        }
        if (r < heap_size && harr[r] < harr[smol])
        {
            smol = r;
        }
        if (smol != i)
        {
            swap(&harr[i], &harr[smol]);
            minHeapify(smol);
        }
    }
}