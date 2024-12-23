#ifndef OPTIONALMEMBER_HPP
#define OPTIONALMEMBER_HPP


#include <type_traits>
#include <concepts>
#include <stdexcept>
#include <string_view>

namespace KozyLibrary {

/*
    Enables a field only if a condition is evaluated true at compile time. 

    Depending on the compiler, sizeof(Optional_Member<T, false>) is at most 1. 
    Note:   This might not be guaranteed behaviour. As far as I know, any Type has to be at least as big as char, which is the designated smallest type.
            In C++, a raw compile-time zero-length array, is sort of a type, thus I misuse that knowledge and an inactive Optional_Member becomes a wrapper of a zero-length array, which will likely get removed by an optimizing compiler.
            
    Accessing the underlying value in an inactive state throws an exception. 
    Note:   It throws instead of causing a compiler error, so that the user can create control flows in which the value is only accessed in an activated state.

*/
template<typename T, bool activation_condition, typename T2 = typename std::conditional< (activation_condition == true), T, void*[0]>::type >
struct Optional_Member{
    
    inline static constexpr bool is_Active{activation_condition};

 

    template<typename TT>
    constexpr Optional_Member(TT&& val) requires (is_Active) :
         m_member(std::forward<TT>(val))
    {
 
    }

    constexpr Optional_Member() requires (is_Active) :
        m_member()
    {

    }

    template<typename TT>
    constexpr Optional_Member(TT&& val) requires (!is_Active)
    {
        
    }
    constexpr Optional_Member() requires (!is_Active) 
    {

    }

    constexpr ~Optional_Member(){}

   
    constexpr operator const T&()const noexcept requires (is_Active) {
        return m_member;
    }
    operator T&() noexcept requires (is_Active) {
        return m_member;
    }


    constexpr operator const T&()const requires (!is_Active) {
        throw std::logic_error(
            "Error: OptionalMember.\n"
            "Tried to operate on a not activated object!\n"
            "The activated Type would be: " +
            std::string(typeid(T).name())
        );
        return reinterpret_cast<const T&>(m_member);
    }
    
    operator T&() requires (!is_Active) {
        throw std::logic_error(
            "Error: OptionalMember.\n"
            "Tried to operate on a not activated object!\n"
            "The activated Type would be: " +
            std::string(typeid(T).name())
        );
        return reinterpret_cast<T&>(m_member);
    }

/*
    The (member) field only gets initialized, if it is active.
*/
#ifdef _MSC_VER
    [[msvc::no_unique_address]] T2 m_member;
#else
    [[no_unique_address]] T2 m_member;  
#endif
	
};

/**
//Example
template<int i>
struct A {
    
    Optional_Member<int, (i == 2)> a;
    
    inline static constexpr Optional_Member<int, i == 3> b{42}; // gets the value 42 only if i == 3, otherwise the value is ignored
    
};
*/

}


#endif