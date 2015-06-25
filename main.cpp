#include <iostream>

#include "BinomialHeap.h"

using namespace std;

template<typename T>
bool checkHeapProperty( const Node<T>* root )
{
    if ( root == nullptr )
    {
        return true;
    }

    const T key = root->key;

    Node<T>* child = root->leftmostChild;
    while( child )
    {
        if ( child->key < key )
        {
            cout << "Heap property constraint not satisfied between nodes: " << endl
                 << "Parent: " << root->key << endl
                 << "Child: " << child->key << endl;
            return false;
        }

        if ( ! checkHeapProperty( child ) )
        {
            return false;
        }

        child = child->right;
    }

    return true;
}

template<typename T>
bool checkHeapProperty( const BinomialHeap<T> heap )
{
    // check the properties of all the trees in the heap
    Node<T>* root = heap.getRoot();

    while( root )
    {
        if ( ! checkHeapProperty( root ) )
        {
            return false;
        }

        root = root->right;
    }

    return true;
}

int main()
{
    cout << "Hello World!" << endl;

    BinomialHeap<int> heap;

    heap.push( 4 );
    heap.push( 3 );
    heap.push( 5 );
    Node<int>* sixth = heap.push( 6 );
    heap.push( 7 );

    cout << heap.getRoot()->key << endl;
    cout << heap.getMin()       << endl;
    cout << checkHeapProperty( heap ) << endl;

    heap.decreaseKey( sixth, 0 );
    cout << checkHeapProperty( heap ) << endl;
    cout << heap.getMin() << endl;

    return 0;
}
