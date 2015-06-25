#ifndef BINOMIAL_HEAP_HPP
#define BINOMIAL_HEAP_HPP

#include <vector>

using namespace std;
// the root trees in the pyramid are in decreasing order
// inside the root tree the items are in increasing order

template<class T>
struct Node
{
    Node *right;
    Node *parent;
    Node *leftmostChild;
    T    key;
};

template<class T>
class BinomialHeap
{
private:
    // The root of the heap. Should be kept NULL all the time the heap is empty
    Node<T> *root;
    // The number of elements currently stored in the heap.
    int size;

protected:
        /**
        * Auxiliary function to be used when uniting two heaps.
        *
        * This function should merge the two given root lists in the root list of a
        * single binomial heap - the result of the merging.
        *
        * This method is virtual for ease of testing. Still you need to implement it.
        *
        * |NOTE| This method could have been static as long as it will not access any
        * |NOTE| of the fields of the class. Still it is not static, because in some
        * |NOTE| of the tests it will be overridden.
        *
        * @param leftRoot - the first node of the root list of the first heap
        * @param leftSize - the number of the elements in the left heap
        * @param rightRoot - the first node of the root list of the second heap
        * @param rightSize - the number of the elements in the right heap
        * @return the first node of the root list of the binomial heap result of
        *		the merging of the two given heaps
        */
        typedef unsigned int Degree;
        virtual Node<T>* consolidate( Node<T>* leftRoot, int leftSize,
                                      Node<T>* rightRoot, int rightSize)
        {
            // here we go
            vector< Degree > leftTreeDegrees;
            degreesOfTree(leftSize, leftTreeDegrees);
            vector< Degree> rightTreeDegrees;
            degreesOfTree(rightSize, rightTreeDegrees);

            vector< Degree > mergedDegrees; // todo: reserve space for leftTreeDegrees.size() + rightTreeDegrees.size()
            Node<T>* mergedPathHead = nullptr;
            Node<T>** nextInsertionPos = &mergedPathHead;

            int leftPos = 0;
            int rightPos = 0;

            // merge the root lists maintaining ascending order of degrees
            while (leftRoot != 0
                    && rightRoot != 0)
            {
                const Degree leftDegree = leftTreeDegrees[ leftPos ];
                const Degree rightDegree = rightTreeDegrees[ rightPos ];
                if ( leftDegree < rightDegree )
                {
                    *nextInsertionPos = leftRoot;
                    leftRoot = leftRoot->right;
                    mergedDegrees.push_back(leftTreeDegrees[ leftPos ]);
                    leftPos += 1;
                }
                else
                {
                    *nextInsertionPos = rightRoot;
                    mergedDegrees.push_back(rightTreeDegrees[ rightPos ]);
                    rightRoot = rightRoot->right;
                    rightPos += 1;
                }

                nextInsertionPos = &((*nextInsertionPos)->right);
            }
            // if we are here one of the list is empty -> check which one

            const bool shouldUseLeftRoot = leftRoot != nullptr;

            *nextInsertionPos = shouldUseLeftRoot
                                ? leftRoot
                                : rightRoot;

            int degreesPos = shouldUseLeftRoot
                             ? leftPos
                             : rightPos;

            vector< Degree >& degreesToCopy = shouldUseLeftRoot
                                              ? leftTreeDegrees
                                              : rightTreeDegrees;
            while( *nextInsertionPos )
            {
                mergedDegrees.push_back( degreesToCopy[ degreesPos ] );
                degreesPos += 1;
                nextInsertionPos = &((*nextInsertionPos)->right);
            }

            // remove duplicates
            Node<T>* current = mergedPathHead;
            Node<T>* prev = nullptr; // sometimes we need the element before the current, to reroute the links between the roots
            Node<T>* next = current->right;
            int pos = 0;
            while (next != nullptr)
            {
                if ( mergedDegrees[ pos ] != mergedDegrees[ pos + 1 ]
                     || ( next->right != nullptr && mergedDegrees[ pos ] == mergedDegrees[ pos + 2 ] ) )
                {
                    // simply advance
                    prev    = current;
                    current = next;
                }
				else
				{
					// we have to merge trees head and next
					if ( current->key < next->key )
					{
						// next should become child of head
						current->right = next->right;
						this->addLeftmostChild( current, next );
						mergedDegrees[ pos + 1 ] += 1; // merging will increase the degree of head
					}
					else
					{
						// current should become child of next
						if ( prev != nullptr )
						{
							prev->right = next;
						}
						else
						{
							// if we don't have elements before the current and we merge the first with the second, the head now
							// must point the second element( took me several minutes to grasp this shit :D)
							mergedPathHead = next;
						}
						this->addLeftmostChild( next, current );
						mergedDegrees[ pos + 1 ] += 1; // merging will increase the degree of head
						current = next;
					}
				}

                pos += 1;
                next  = current->right;
            }

            return mergedPathHead;
        }

public:
    /**
    * Construct empty binomial heap
    */
    BinomialHeap()
        : root(nullptr)
        , size(0)
    {}
    /**
    * Construct heap with the given elements.
    * Expected runneing time O(elementsSize)
    * @param elements - array containing the elements
    * @param elementsSize - the number of elements in the array
    */
    BinomialHeap(T elements[], int elementsSize)
    {
        for (int i = 0; i < elementsSize; ++ i)
        {
            this->push(elements[i]);
        }
    }

    /**
    * @return - the key of the minimal element currently stored in the heap
    */
    T getMin() const
    {
        int size;// unused
        Node<T>* minNode = this->getMinNode(size);
        if ( minNode == nullptr )
        {
            if ( root != nullptr )
            {
                cout << "Error finding min occured" << endl;
            }
            else
            {
                cout << "Could not found min: Heap is empty" << endl;
            }
            return T();
        }

        return minNode->key;
    }

    /**
    * @return - the root of the heap. Will be used for testing purposes.
    */
    Node<T>* getRoot() const
    {
        return root;
    }

    /**
    * @return - the number of the elements currently stored in the heap.
    */
    int getSize() const
    {
        return size;
    }

    /**
    * Extracts (removes) the element with minimal key from the heap
    * @return - the key of the minimal element currently stored in the heap
    */
    T extractMin()
    {
        // the last one. oh boy this is gonna be hard:D
        unsigned int sizeOfNode = 0; // the number of nodes covered by min node(including the node)
        Node<T>* minNode = this->getMinNode( sizeOfNode );
        if ( minNode == nullptr )
        {
            // report error;
            return T();
        }

        Node<T>* firstChild = minNode->leftmostChild;
        Node<T>* rootToBeMerged = this->reverseRootList( firstChild );

        this->consolidate( root, size, rootToBeMerged, sizeOfNode - 1 );

        return minNode->key;
    }

    /**
    * Unites the current heap with the one given.
    * It is expected that the structure of otherHeap will be destroyed in this method.
    *
    * @param otherHeap - the heap with which we will unite. It will be destroyed in this method
    */
    void unite(BinomialHeap<T>& otherHeap)
    {
        root = this->consolidate(root, size, otherHeap.root, otherHeap.size);
        size += otherHeap.size; // for now we don't delete elements with the same key
    }

    /**
    * Inserts element with the new given key in the heap.
    * @param newKey - the key of the element to be inserted
    * @return a pointer to the newly created element in the heap
    */
    Node<T>* push(const T& newKey)
    {
       if ( root == nullptr )
       {
           // node creation
           Node<T>* newNode = new Node<T>;
           newNode->key           = newKey;
           newNode->leftmostChild = nullptr;
           newNode->parent        = nullptr;
           newNode->right         = nullptr;

           size = 1;
           root = newNode;

           return newNode;
       }
       else
       {
           BinomialHeap temp;
           Node<T>* newNode = temp.push( newKey );

           this->unite( temp );
           return newNode;
       }
    }

    /**
    * Decreases the key of the given element to the given amount.
    * It is guaranteed the ptr is real pointer to element of the heap.
    * WARNING!!!
    *       No changes should be made if the newly given key is larger than
    *       the current value of the key.
    * WARNING!!!
    *
    * @param ptr - a pointer to the element which key is to be modified
    * @param newKey - the new value of the key to be used for element pointed by ptr
    */
    void decreaseKey(Node<T>* ptr, const T& newKey)
    {
        const T& ptrKey = ptr->key;
        if ( ptrKey < newKey )
        {
            return;
        }

        ptr->key = newKey;

        while( ptr->parent != nullptr
               && ptr->parent->key > newKey )
        {
            // 11:12 - we have simpler version now
            T temp = ptr->parent->key;
            ptr->parent->key = ptr->key;
            ptr->key = temp;

            ptr = ptr->parent;
            // rerouting the links
//            Node<T>* oldParent = ptr->parent;

//            // reroute sibling links from previous sibling of oldParent
//            Node<T>* grandparent = oldParent->parent;
//            if ( grandparent != nullptr )
//            {
//                Node<T>* uncle = grandparent->leftmostChild;
//                if ( uncle != oldParent )
//                {
//                    while( uncle->right != oldParent )
//                    {
//                        uncle = uncle->right;
//                    }

//                    uncle->right = ptr;
//                }
//            }

//            // reroute all the children of ptr to link to oldParent
//            Node<T>* firstChild = ptr->leftmostChild;
//            while( firstChild )
//            {
//                firstChild->parent = oldParent;
//                firstChild = firstChild->right;
//            }

//            // reroute siblings of ptr to pointer to oldParent now
//            Node<T>* brother = oldParent->leftmostChild;
//            if ( brother != ptr )
//            {
//                while( brother->right != ptr )
//                {
//                    brother = brother->right;
//                }

//                brother->right = oldParent;
//            }

//            // reroute ptr with his parent
//            Node<T>* ptrParentTemp = ptr->parent;
//            Node<T>* ptrRightTemp = ptr->right;
//            Node<T>* ptrLeftmostChild = ptr->leftmostChild;

//            ptr->parent = oldParent->parent;
//            ptr->right = oldParent->right;
//            ptr->leftmostChild = oldParent->leftmostChild;

//            oldParent->right         = ptrRightTemp;
//            oldParent->parent        = ptrParentTemp;
//            oldParent->leftmostChild = ptrLeftmostChild;
        }
    }

private:

    Node<T>* getMinNode( int& sizeOfMinNode ) const
    {
        if ( root == nullptr )
        {
            return nullptr;
        }


        vector<Degree> degrees;
        this->degreesOfTree( size, degrees );
        Node<T>* min = root;
        int degreeOfMinNode = degrees[ 0 ];
        Node<T>* current = root->right;
        int pos = 1;
        while( current )
        {
            if ( current->key < min->key )
            {
                min = current;
                degreeOfMinNode = degrees[ pos ];
            }

            current = current->right;
        }

        sizeOfMinNode = 1 << degreeOfMinNode; // size
        return min;
    }
    /**
     * @brief reverseRootList - Reverses linked list of roots( used because subtrees in Binomial Tree are aligned in descending order
     * but we need them in ascending to merge them with other heaps(this is used in extract min))
     * @param rootList
     * @return
     */
    Node<T>* reverseRootList( Node<T>* rootList )
    {
        if ( rootList->right == nullptr )
        {
            return rootList;
        }

        Node<T>* reversed = this->reverseRootList( rootList->right );
        reversed->right = rootList;
        return reversed;
    }

    /**
     * @brief mergeTrees: Merge two trees. Trees must be of equal degrees. Inside a tree its subtrees are in acsending order
     * @param firstTree
     * @param firstDegree
     * @param secondTree
     * @param secondDegree
     * @return
     */
    Node<T>* mergeTrees(Node<T>* firstTree, int firstDegree, Node<T>* secondTree, int secondDegree)
    {
        if (firstDegree != secondDegree)
        {
            return nullptr;
        }

        if (firstTree->key < secondDegree->key)
        {
            // add second tree as a first child of first
            this->addLeftmostChild(firstTree, secondTree);
        }
        else
        {
            this->addLeftmostChild(secondTree, firstTree);
        }
    }

    void addLeftmostChild(Node<T>* root, Node<T>* child)
    {
        child->right         = root->leftmostChild;
        root->leftmostChild  = child;
        child->parent        = root;     
    }

    static void degreesOfTree( unsigned int treeSize, vector<Degree>& degrees )
    {
        Degree degree = 0;
        while( treeSize != 0 )
        {
            const bool lastBit = treeSize % 2 != 0;
            if ( lastBit )
            {
                degrees.push_back( degree );
            }

            degree += 1;
            treeSize /= 2;
        }
    }

    static unsigned int degreeOfTree(unsigned int treeSize)
    {
        // we need basically to find log_2( treeRoot->size() )
        unsigned int degree = 0;
        while (((treeSize & 1) == 0) && treeSize > 1) /* While x is even and > 1 */
        {
            treeSize >>= 1;
            degree += 1;
        }

        return degree;
    }

};

#endif //BINOMIAL_HEAP_HPP
