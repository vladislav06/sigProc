//
// Created by vm on 24.30.10.
//

#pragma once

#include <format>
#include "src/nodes/dataTypes/baseDataType.h"


template<class T>
concept baseDataType = std::is_base_of_v<BaseDataType, T> && requires(T){
    typename   T::DataType;
};

template<class T, std::size_t N>
concept has_tuple_element = baseDataType<std::tuple_element<N, T>>;
/**
 * checks if T is std::tuple<x> and that each element of this tuple is derived from BaseDataType
 * @tparam T
 */
template<class T>
concept tuple = std::__is_specialization_of<T, std::tuple> && requires(T){
    {
    []<std::size_t... N>(std::index_sequence<N...>) {
        return (has_tuple_element<T, N> && ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>())
    };

};


template<baseDataType T>
void test() {

}
