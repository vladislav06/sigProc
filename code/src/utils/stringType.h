//
// Created by vm on 24.30.10.
//
#pragma once

template<auto N>
struct StringType {
    constexpr StringType(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];

};

/**
 * Const function that allows StringType compile time concatenation
 * @tparam N1 length of first StringType
 * @tparam N2 length of second StringType
 * @param a first StringType
 * @param b second StringType
 * @return new StringType that is concatenation of first and second
 */
template<auto N1, auto N2>
consteval StringType<N1 + N2 - 1> operator+(StringType<N1> a, StringType<N2> b) {
    char value[N1 + N2 - 1];
    std::copy_n(a.value, N1 - 1, value);
    std::copy_n(b.value, N2, value + N1 - 1);
    StringType<N1 + N2 - 1> s(value);
    return s;
}

/**
 * Const function that allows StringType and char array compile time concatenation
 * @tparam N1 length of first StringType
 * @tparam N2 length of second char array
 * @param a first StringType
 * @param b second char array
 * @return new StringType that is concatenation of first and second
 */
template<auto N1, auto N2>
consteval StringType<N1 + N2 - 1> operator+(StringType<N1> a, const char (&b)[N2]) {
    char value[N1 + N2 - 1];
    std::copy_n(a.value, N1 - 1, value);
    std::copy_n(b, N2, value + N1 - 1);
    StringType<N1 + N2 - 1> s(value);
    return s;
}