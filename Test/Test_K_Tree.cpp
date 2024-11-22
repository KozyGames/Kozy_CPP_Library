/*
Test-main
 
*/

#include <iostream>
#include <string_view>

#include "../KozyLibrary_Complete.hpp"


using namespace std;
using namespace KozyLibrary;
using sview = std::string_view;

bool foo(const int& a, const int& b) {
    return a < b;
}
inline static constexpr decltype(&foo) fooArr2[] = {foo };

struct A{
    int x,y;
};

bool fooX(const A& lhs, const A& rhs) {
    return lhs.x < rhs.x;
}
bool fooY(const A& lhs, const A& rhs) {
    return lhs.y < rhs.y;
}
inline static constexpr decltype(&fooX) fooArrXY[] = {fooX, fooY};

ostream& operator<<(ostream& out, const A& rhs){
    return out << rhs.x << ' ' << rhs.y;
}

template<typename T, const T& t >
consteval auto test_strLength() {
    return t.length();
}

int main(int argc, const char** args) noexcept {
    cout << "Hello, World!" << endl;
    constinit static int arr[] = {4, 5, 6, 3, 2, 4, 5};
    //constexpr static int arr[] = {4, 5, 6, 3, 2, 4, 5};
    
    KozyLibrary::K_Tree<int, 1, fooArr2> tree1;
    //KozyLibrary::K_Tree<int, 1, fooArr> tree1 = {arr[0], arr[1], arr[2], arr[3]};

    static constexpr unsigned arrSize = sizeof(arr)/sizeof(int);
    //cout << arrSize <<endl; return 0;
    //cout << tree1.BUCKET_CNT << endl;

    tree1.push_array(arr, arrSize);
    cout << endl<<endl;
    for (auto iter = tree1.begin(), end = tree1.end(); iter != end; ++iter){
        cout << **iter << endl;
        cout << "next: ";
        if (iter.get_nextBranch()){
            cout << ***iter.get_nextBranch() << endl;
        } else {
            cout << "EMPTY" << endl;
        }
        cout << endl;
    }
    
    cout << endl;
    
    constinit static A arr2[] = { {10,10}, {5,5}, {12,6}, {8,14}, {15,16}, {7,16}, {18,20}, {5,21}, {7,7}, {6,6}};
    static constexpr unsigned arrSize2 = sizeof(arr2)/sizeof(A);

    KozyLibrary::K_Tree<A, 2, fooArrXY> tree2;
    tree2.push_array(arr2, arrSize2);

    cout << endl<<endl;
    for (auto iter = tree2.begin(), end = tree2.end(); iter != end; ++iter){
        cout << (**iter) << '\n';
        cout << "next: ";
        if (iter.get_nextBranch()){
            cout << ***iter.get_nextBranch() << endl;
        } else {
            cout << "EMPTY" << endl;
        }
        cout << endl;
    }
    
    cout << endl;

    //for (auto& e: tree2){

    //}
/**
    for(auto iter = tree2.begin(), end = tree2.end(); iter != end; ++iter){
        cout << (**iter) << endl;

        if ( (*iter).children[0]){
            cout << "Left: " << **(*iter).children[0] << endl;
        } else {
            cout << "Left: NULL" << endl;
        }

        if ( (*iter).children[1]){
            cout << "Right: " << **(*iter).children[1] << endl;
        } else {
            cout << "Right: NULL" << endl;
        }
        cout << endl;
        //auto next = iter.get_NextBranch();
        
    }
*/
    static constexpr sview astr = "ABC";
    static constexpr sview bstr = "DEFGH";
    //static constexpr sview cstr = "IJKLMNOP";

    static constexpr auto conc = concat_CT<astr, bstr>(); // TYPE : CONC_PROXY


    cout << conc << endl;
    static constexpr sview concStr = conc;
    cout << concStr << endl;


    cout << test_strLength<decltype(concStr), concStr>() << endl;
/*
    static constexpr Build_Config aaa{true};
    //static constexpr 
    //bool bbb = aaa.IS_DEBUG();
    static constexpr 
    //bool bbb = aaa.bbb;
    bool bbb = conf.bbb;
    
	if (bbb){
        cout << "AAA" << endl;
    } else {
        cout << "BBB" << endl;

    }*/

    return 0;
}