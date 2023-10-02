#pragma once

template <class T> void swap(T& a, T& b)
{
	T c(a); a = b; b = c;
}

//double log(double val, double base) {
//	return 0.0;
//}

//double log(double x, double base) {
//    if (x <= 0 || base <= 1) {
//        // Logarithm is undefined for these values
//        return 0; // NAN (Not-a-Number) represents an undefined result
//    }
//
//    double result = 0.0;
//    while (x >= base) {
//        x /= base;
//        result += 1.0;
//    }
//
//    return result;
//}