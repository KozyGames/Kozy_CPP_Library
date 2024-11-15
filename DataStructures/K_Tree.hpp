#ifndef K_TREE_HPP
#define K_TREE_HPP

/*

-- Part of KozyLibrary/DataStructures

*/

#include <type_traits>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <functional>



namespace KozyLibrary {

/*
* DESCRIPTION *

A Tree structure, that stores Elements in Nodes according to an array of comparison functions. 
A node has 2 to the power of k branches for which k is the count of dimensions/properties that elements are to be differentiated.
The tree does not take ownership of the elements.

Assumptions:
- Each Element is unique.
- an element is not deleted whilst there is a Node pointing to it.


* TEMPLATE PARAMETERS *

ElementT			: The underlying Element Type.

PROPERTIES_CNT		: How many distinguishable properties/dimensions/features an Element has. 
					For Example, if you store students only according to their points earned, then the Element has only one property to sort by.

compArr			: decides if a specific value of a property of the left object is semantically "bigger" than the rights respective one. 
                    left > right == true


* OTHER *

Node				: represents a pointer to an existing element. If the object is destroyed, then this node is in an invalid state.


*/
template<
	typename ElementT, 
	uint_fast8_t PROPERTIES_CNT, 
	bool (* const (&compArr)[PROPERTIES_CNT]) (const ElementT&, const ElementT&)
>
class K_Tree{
public:

inline static constexpr uint_fast8_t PROPERTIES				= PROPERTIES_CNT;
inline static constexpr auto COMPARE_FUNCTION_ARRAY		    = compArr;
inline static constexpr auto BUCKET_CNT 	                = (PROPERTIES_CNT <= 7) ? (static_cast<uint_fast8_t>(1) << PROPERTIES_CNT) : 
                                                            (PROPERTIES_CNT <= 15) ? (static_cast<uint_fast16_t>(1) << PROPERTIES_CNT): (static_cast<uint_fast32_t>(1) << PROPERTIES_CNT);

using BUCKET_TYPE = std::remove_cv_t<decltype(BUCKET_CNT)>;

struct Node;


    template<typename IteratorType>
    K_Tree(IteratorType arr, uint_fast32_t cnt)
    {
        root = nullptr;
        while (cnt-- != 0){
            push(*arr);
            ++arr;
        }
    }

    K_Tree(std::initializer_list<std::reference_wrapper<ElementT>> l){
        root = nullptr;
        for (auto& e : l){
            push(e);
        }
    }

    K_Tree():
        root(nullptr)
    {

    }

    ~K_Tree() {
       if (!is_empty()){
            for (Iterator iter = this->begin(), end = this->end(); iter != end; ){
                const Node& temp = *iter;
                ++iter;
                delete &temp;
            }
        }
    }

    Node& push(ElementT& obj) noexcept {
        return internal_push(obj, &root);
    }

    K_Tree& operator<<(ElementT& obj) noexcept {
        push(obj);
        return *this;
    }

    template<typename IteratorType>
    void push_array(IteratorType arr, uint_fast32_t cnt) {
        while (cnt-- != 0){
            push(*arr);
            ++arr;
        }
    }

    struct Node{

        Node(ElementT* v):
            value(v)
        {
            for (BUCKET_TYPE cnt = 0; cnt != BUCKET_CNT; ++cnt){
                children[cnt] = nullptr;
            }
        }

        inline ElementT& operator*() noexcept {
            return *value;
        }
        inline const ElementT& operator*() const noexcept {
            return *value;
        }

        inline ElementT& operator->() noexcept {
            return *value;
        }
        inline const ElementT& operator->() const noexcept {
            return *value;
        }

        ElementT* value;
        Node* children[BUCKET_CNT];


    };

    class Iterator{
    public:

        Iterator(Node* cur, Iterator* next = nullptr):
            current(cur),
            nextBranch(next)
        {

        }

        Iterator(const Iterator& cpy):
            current(cpy.current)
        {
            if (cpy.nextBranch){
                nextBranch = new Iterator(*cpy.nextBranch);
            } else {
                nextBranch = nullptr;
            }
        }

        ~Iterator() {
            delete nextBranch;
        }

        Iterator& operator=(const Iterator& rhs) noexcept {
            if (this != &rhs){
                current = rhs.current;
                delete nextBranch;
                if (rhs.nextBranch){
                    nextBranch = new Iterator(*rhs.nextBranch);
                } else {
                    nextBranch = nullptr;
                }
            }

            return *this;
        }

        /*
        User is responsible for valid Iterator. Undefined behavior, if the iterator points to a nullptr Node. 
        */
        inline Iterator& operator++() {
            return const_cast<Iterator&>(++static_cast<const Iterator&>(*this));
        }
        const Iterator& operator++() const {
            for (Node ** childIter = current->children, ** endIter = childIter + BUCKET_CNT; childIter != endIter; ++childIter){ // search for first valid child of current Node
                if (*childIter){ // true : valid child of current Node found
                    while (--endIter != childIter){ // look for new branches. go in reverse order from end to child
                        if (*endIter){ // true : new branch found. endIter is pointing to a sibling Node
                            nextBranch = new Iterator(*endIter, nextBranch); // update nextBranch
                        }
                    }
                    current = *childIter;
                    return *this;
                }
            } // current has no children

            if (nextBranch){ // jump to next branch
                const Iterator* const temp = nextBranch;
                current = temp->current;
                nextBranch = temp->nextBranch;
                (*temp).nextBranch = nullptr;
                delete temp; 

            } else { // there are no next nodes
                current = nullptr;
            }

            return *this;
        }

        Iterator operator++(int) const {
            Iterator old(*this);
            ++(*this);
            return old;
        }

        constexpr bool operator==(const Iterator& rhs) const noexcept {
            return current == rhs.current;
        }
        inline constexpr bool operator!=(const Iterator& rhs) const noexcept {
            return !((*this) == rhs);
        }

        Node& operator*() noexcept {
            return *current;
        }
        const Node& operator*() const noexcept {
            return *current;
        }

        ElementT& operator->() noexcept {
            return **current;
        }
        const ElementT& operator->() const noexcept { 
            return **current;
        }

        Iterator* get_nextBranch() noexcept {
            return nextBranch;
        }
        const Iterator* get_nextBranch() const noexcept {
            return nextBranch;
        }

        bool has_Node() const noexcept {
            return (current);
        }

    private:

        mutable Node* current;
        mutable Iterator* nextBranch;

    };

    Node* get_root() noexcept {
        return root;
    }
    const Node* get_root() const noexcept {
        return root;
    }

    bool is_empty() const noexcept {
        return !(root);
    }

    //ElementT& remove_Node(Node* node) {
        
    //}

    //ElementT& remove_element(const ElementT& obj) {
        
    //}

    /*
    gets the first node of an element, which is smaller than obj but bigger than all elements that are smaller than obj.

    case: there is an Element smaller than obj
        return a node to that

    case: there is no smaller Element
        return smallest

    case: there is no bigger
        return the biggest

    case: there are no elements 
        Undefined Behavior


    *//*
    static Node& find(const ElementT& obj, Node* current) {
        static constexpr BUCKET_TYPE smallerPivot = BUCKET_CNT/2;
        BUCKET_TYPE childPos = get_ComparisonIndex(**current, obj); 

        while(childPos < smallerPivot){ 
            Node* const child = (**current).children[childPos]; 
            if (child){
                current = child;
                childPos = get_ComparisonIndex(**current, obj);
            } else {
                break;
            } 
        }
        
        return *current;
    }
    inline Node& find(const ElementT& obj) {return find(obj, root);}
*/

//    Iterator get_range_shuffled(const ElementT& lhs, const ElementT& rhs) {

//    }

    Iterator begin(Node* node) {
        return Iterator(node);
    }
    inline Iterator begin() {return this->begin(root);}

    /*
    exclusive end
    */
    Iterator end(Node* node = nullptr) noexcept {
        return Iterator(node, nullptr);
    }



private:

    Node* root;


// ** Helper Functions **

    /*
    Example:
        compArr[0](left, right) return left[0] < right[0] 
        compArr[1](left, right) return left[1] < right[1] 
        
        struct XY{int x,y};
        left        right       out(binary) out(decimal)    meaning
        (0,0)       (1,1)       00          0               left <= right
        (2,0)       (1,1)       01          1               left <= right

        (0,2)       (1,1)       10          2               left >= right
        (2,2)       (1,1)       11          3               left >= right

        (0,1)       (1,1)       00          0               left <= right

        if (out <= PROPERTIES_CNT/2) then left is "smaller" than right

    */
    inline static BUCKET_TYPE get_ComparisonIndex(const ElementT& left, const ElementT& right) noexcept {
        BUCKET_TYPE childPos = 0;
        for(uint_fast8_t cnt = 0; cnt != PROPERTIES_CNT; ++cnt){
            childPos += (compArr[cnt](left, right) == true) ? (1 << cnt):(0);
        }
        return childPos;
    }

    static Node& internal_push(ElementT& obj, Node** node) noexcept {
        for (Node* n = *node; n != nullptr; n = *node){
            const BUCKET_TYPE childPos = get_ComparisonIndex(***node, obj);
            node = &(**node).children[childPos];
        }
        
        return *(*node = new Node(&obj));
    }


};

}
#endif

/*

k = 2

|(0,1)  |       |(1,1)
|       |       |
------ (1,1) ---------
|       |       |
|(0,0)  |       |(1,0)

*/