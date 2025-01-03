//
// Created by vm on 24.7.11.
//
#pragma once

#include "baseOperation.h"
#include "QString"

template<typename T>
class SubtractionOperation : public BaseOperation<T> {
public:
    T compute(T t1, T t2) override {
        return t1 - t2;
    }

    QString getName() override {
        return "Subtraction";
    }
};


template<typename T>
class SubtractionOperation<ArrayData<T>> : public BaseOperation<ArrayData<T>> {
public:

    std::shared_ptr<ArrayData<T>> compute(std::shared_ptr<ArrayData<T>> t1, std::shared_ptr<ArrayData<T>> t2) override {
        auto out = std::make_shared<ArrayData<T>>();
        if (t1 == nullptr || t2 == nullptr) {
            return out;
        }
        out->get().resize(std::max(t1->get().size(), t2->get().size()));

        for (int a = 0; a < t1->get().size(); a++) {
            out->get()[a] += t1->get()[a];
        }
        for (int a = 0; a < t2->get().size(); a++) {
            out->get()[a] -= t2->get()[a];
        }

        return out;
    }

    QString getName()

    override {
        return "Subtraction";
    }

};