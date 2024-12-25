//
// Created by vm on 24.30.10.
//

#pragma once

#include <format>
#include "src/nodes/dataTypes/baseData.h"


template<class T>
concept baseData = std::is_base_of_v<BaseData, T> && requires(T){
    typename T::DataType;
};

template<class T>
concept SharedPtrToBaseData=requires(T){
    typename T::element_type;
    typename T::element_type::DataType;
    baseData<typename T::element_type>;
} && std::is_same_v<std::shared_ptr<typename T::element_type>, T>;


//concept to check if all types in a tuple are derived from Base
template<typename Tuple, std::size_t... Is>
constexpr bool TupleOfDerivedFromBaseImpl(std::index_sequence<Is...>) {
    return (baseData<std::tuple_element_t<Is, Tuple>> && ...);
}

template<typename Tuple>
concept TupleOfDerivedFromBase = TupleOfDerivedFromBaseImpl<Tuple>(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});

template<typename T>
concept tuple = TupleOfDerivedFromBase<T>;


static_assert(SharedPtrToBaseData<std::shared_ptr<BaseData>>);

