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