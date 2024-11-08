//
// Created by vm on 24.7.11.
//

#pragma once


template<typename T>
class BaseOperation {
public:
    virtual std::shared_ptr<T> compute(std::shared_ptr<T> t1, std::shared_ptr<T> t2) = 0;

    virtual QString getName() = 0;
    virtual ~BaseOperation()=default;
};
