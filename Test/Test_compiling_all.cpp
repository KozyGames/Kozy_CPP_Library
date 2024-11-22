#include <iostream>
#include <string>
#include <string_view>

#include "../KozyLibrary_Complete.hpp"


using namespace std;
using sview = string_view;



inline static constexpr bool b = false;
struct A {
    KozyLibrary::Optional_Member<int, b> v{42};
    inline static constexpr KozyLibrary::Optional_Member<int, b> v2{101};
    KozyLibrary::Optional_Member<int, b> v3;
    KozyLibrary::Optional_Member<int&, b> v4{v};


};

template<typename T>
struct B {

    constexpr B(const T& arg):
        t(arg)
    {

    }
    constexpr ~B(){

    }

   operator T&(){
        return t;
    }

    
    constexpr operator const T&()const {
        return t;
    }
    T t;
};

struct BB {

};


int main(int argc, const char** args){

    
    A a;
    constexpr B bb(42);

    //cout << sizeof(A) << endl;
    //cout << sizeof(BB) << endl;


    if constexpr(b){

        try {
        cout << a.v << endl;
        a.v = 6;
   
        cout << a.v << endl;
        cout << a.v4 << endl;


        cout << a.v + 4 << endl;



        int c = move(a.v);
        cout << c << endl;
        cout << a.v << endl;


        } catch(const exception& e){
            cout << e.what() << endl;
        }
    }
    /**
    if constexpr(b){
        cout << a.v << endl;
    }*/
    return 0;
}