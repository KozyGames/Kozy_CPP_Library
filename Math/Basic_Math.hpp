#ifndef BASIC_MATH_HPP 
#define BASIC_MATH_HPP

#include <cstdint>

namespace KozyLibrary {
namespace Math {
	
/*


*/
constexpr uint_fast32_t power(uint_fast32_t base, uint_fast32_t exponent) {
	uint_fast32_t res = 1;
	
	while (exponent != 0){
		if (exponent & 0b01){
			res = res * base;
		}
		base = base * base;
		exponent = exponent >> 1;
	}

	return res;
}

/*

*/
constexpr uint_fast64_t squareMultiply(uint_fast64_t base, uint_fast64_t exponent, uint_fast64_t modulo) {
	uint_fast64_t res = 1;

	while (exponent != 0){
		if (exponent & 0b1){
			res = res * base % modulo;
		}
		base = base * base % modulo;
		
		exponent = exponent >> 1;
	}
	return res;
}


}}

#endif