#include "DataStructures/CompileTime_String.hpp"

#include <iostream>
#include <string_view>

using namespace std;
using sview = string_view;


template<KozyLibrary::CONC_PROXY lhs, KozyLibrary::CONC_PROXY rhs>
consteval auto cct() {
    return KozyLibrary::concat_CT<lhs,rhs>();
}

/*

showcases three different ways on how to create a complex string at compiletime by combining strings that are known at compiletime.
this prints:

ABCdefg...xyz
foo!

ABCdefg...xyz
foo!

ABCdefg...xyz
foo!

*/
int main(int argc, const char** args) {
    static constexpr sview s1 = "ABC";
    static constexpr sview s2 = "defg";
    static constexpr sview s3 = "foo!";

    static constexpr auto str1 = KozyLibrary::concat_CT<s1,s2>();
    // Alternatives:
    //static constexpr KozyLibrary::CONC_PROXY str1 = KozyLibrary::concat_CT<s1,s2>();
    //static constexpr auto str1 = KozyLibrary::concat_CT<s1,s2>();
    //static constexpr sview str1 = KozyLibrary::concat_CT<s1,s2>();

    static constexpr auto str2 = KozyLibrary::concat_CT<str1,"...xyz\n">();
    static constexpr auto str3 = KozyLibrary::concat_CT<str2,s3>();

    cout << str3 << '\n' << endl;


    static constexpr auto str4 = 
    cct<cct<cct<
        s1,s2>(), "...xyz\n">(), s3>()
    ;

    cout << str4 << '\n' << endl;
    

    cout << cct<cct<cct<s1,s2>(), "...xyz\n">(), s3>() << endl;

    return 0;
}


