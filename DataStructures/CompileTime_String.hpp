#ifndef COMPILETIME_STRING_HPP 
#define COMPILETIME_STRING_HPP


#include <cstdint>
#include <cstddef>
#include <array>
#include <string_view>
#include <string>

#include "../Math/Basic_Math.hpp"


namespace KozyLibrary {

/*
    returns the number of chars of a str

    expects a null-terminated String
*/
template<typename StringT> 
constexpr std::size_t get_Length(const StringT& str) {
    std::size_t sz = 0;
    for (const auto& e: str){
        if (e == '\0') break;
        ++sz;
    }
    return sz;
}


/*
    A proxy

*/
struct CONC_PROXY {

    template<typename T> 
    constexpr CONC_PROXY(const T& str) : 
        arr(fill_arr<T>(str))
    { 

    }

    constexpr ~CONC_PROXY() {

    }

    inline static constexpr std::size_t BUFFER_SIZE_MAX = KozyLibrary::Math::power(2,16);
    const std::array<char, BUFFER_SIZE_MAX> arr;

    template<typename T> 
    constexpr auto fill_arr(const T& str) {
        std::array<char, BUFFER_SIZE_MAX> arr{};
   
        const std::size_t sz = get_Length(str);
        for (std::size_t pos = 0; pos != sz ; ++pos){
            arr[pos] = str[pos];
        } 

        return arr;
    }

    constexpr operator std::string_view() {
        return std::string_view(arr.cbegin(),arr.cend());
    }

};
    template<CONC_PROXY lhs, CONC_PROXY rhs>
    consteval auto concat_CompileTimeString_Helper() {
     
      std::array<char, get_Length(lhs.arr) + get_Length(rhs.arr)> arr{};
      
      std::size_t pos = 0;
      for (std::size_t index = 0, cnt = get_Length(lhs.arr); index != cnt; ++index, ++pos)
          arr[pos] = lhs.arr[index];
         
      for (std::size_t index = 0, cnt = get_Length(rhs.arr); index != cnt; ++index, ++pos)
          arr[pos] = rhs.arr[index];

      return arr;
    } 

    /*
        A String type ( CONC_PROXY ) must not exceed the internal buffersize, which is at least 2^16. 
        It is advised to not have an output string that's longer than said value
    */
    template<CONC_PROXY lhs, CONC_PROXY rhs, auto arr = concat_CompileTimeString_Helper<lhs,rhs>()>
    consteval std::string_view concat_CT() {
        return std::string_view(arr.cbegin(),arr.cend());
    }

    constexpr auto get_digits(std::size_t num) {
        std::size_t d = 1;
        std::size_t num2 = num;
        while ((num2 = num2 / 10) != 0){
            ++d;
        }
        return d;
    }

    template<const std::size_t num>
    consteval auto number_toString_Helper() {
        std::array<char, get_digits(num) > arr{};
        std::size_t num2 = num;

        for (auto iter = arr.rbegin(), end = arr.rend(); iter != end; ++iter){
            *iter = num2 % 10 + 48;
            num2 = num2 / 10;
        }
        return arr;
    }

    template<std::size_t num, auto arr = number_toString_Helper<num>()>
    consteval std::string_view number_toString() {
        return std::string_view(arr.cbegin(), arr.cend());
    }

    
}

#endif