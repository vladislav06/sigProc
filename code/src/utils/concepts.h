//
// Created by vm on 24.30.10.
//

#pragma once

#include <format>
#include "src/nodes/dataTypes/baseDataType.h"

template<class T>
concept baseDataType = std::is_base_of_v<BaseDataType, T> && requires(T){
    typename T::DataType;
};

//concept to check if all types in a tuple are derived from Base
template<typename Tuple, std::size_t... Is>
constexpr bool TupleOfDerivedFromBaseImpl(std::index_sequence<Is...>) {
    return (baseDataType<std::tuple_element_t<Is, Tuple>> && ...);
}

template<typename Tuple>
concept TupleOfDerivedFromBase = TupleOfDerivedFromBaseImpl<Tuple>(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});

template<typename T>
concept tuple = TupleOfDerivedFromBase<T>;



