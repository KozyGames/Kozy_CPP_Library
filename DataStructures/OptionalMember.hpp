#ifndef OPTIONALMEMBER_HPP
#define OPTIONALMEMBER_HPP


#include <type_traits>
#include <concepts>



struct EMPTY{};


namespace KozyLibrary {

template<typename T, bool activation_condition, typename T2 = typename std::conditional< (activation_condition == true), T, EMPTY>::type >
struct Optional_Member{
    
    inline static constexpr bool is_Active{activation_condition};

    /*
        The member field only gets initialized, if it is active.
    */
    template<typename TT>
    constexpr Optional_Member(TT&& val)
    {
        if constexpr(is_Active){
            m_member = (std::forward<TT>(val));
        }
    }

    constexpr Optional_Member()
    {
        if constexpr(is_Active){
            m_member = T2{};
        }
    }
    constexpr ~Optional_Member(){}

    
    template<typename TT>
    T2& operator=(TT&& t) requires (is_Active) {
        return m_member = t;
    }

    template<typename TT>
    constexpr bool operator==(TT&& t)const requires (is_Active){
        return (m_member == t);
    }


    template<typename TT>
    constexpr bool operator!=(TT&& t)const requires (is_Active){
        return (m_member != t);
    }


    template<typename TT>
    auto operator+=(TT&& t) requires (is_Active){
        return (m_member += t);
    }


    template<typename TT>
    auto operator-=(TT&& t) requires (is_Active){
        return (m_member -= t);
    }


    template<typename TT>
    constexpr auto operator+(TT&& t)const requires (is_Active){
        return (m_member + t);
    }


    template<typename TT>
    constexpr auto operator-(TT&& t)const requires (is_Active){
        return (m_member - t);
    }


    template<typename TT>
    constexpr auto operator*(TT&& t)const requires (is_Active){
        return (m_member * t);
    }


    template<typename TT>
    constexpr auto operator/(TT&& t)const requires (is_Active){
        return (m_member / t);
    }

    constexpr auto* operator&() const requires (is_Active){
        return &m_member;
    }
    auto* operator&() requires (is_Active){
        return &m_member;
    }

    constexpr auto& operator*()const requires (is_Active){
        return *m_member;
    }
    auto& operator*() requires (is_Active){
        return *m_member;
    }
    
    constexpr auto& operator->()const requires (is_Active){
        return *m_member;
    }
    auto& operator->() requires (is_Active){
        return *m_member;
    }
    
    constexpr operator const T2&()const requires (is_Active){
        return m_member;
    }
    operator T2&() requires (is_Active){
        return m_member;
    }

#ifdef _MSC_VER
    [[msvc::no_unique_address]] T2 m_member;
#else
    [[no_unique_address]] T2 m_member;  
#endif
	
};


/**
//Example
template<int i>
struct A{
    
    Optional_Member<int, (i == 2)> a;
    
    inline static constexpr Optional_Member<int, i == 3> b{42}; // gets the value 42 only if i == 3, otherwise the value is ignored
    
};
*/

}


#endif