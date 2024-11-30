//
// Created by vm on 24.28.11.
//
#pragma once

#include "baseOperation.h"
#include "QString"

template<typename T>
class DivisionOperation : public BaseOperation<T> {
public:

    QString getName() override {
        return "Division";
    }

    std::shared_ptr<T> compute(std::shared_ptr<T> t1, std::shared_ptr<T> t2) override {
        return t1 / t2;
    }
};

template<typename T>
class DivisionOperation<ArrayDataType<T>> : public BaseOperation<ArrayDataType<T>> {
public:

    std::shared_ptr<ArrayDataType<T>>
    compute(std::shared_ptr<ArrayDataType<T>> t1, std::shared_ptr<ArrayDataType<T>> t2) override {
        auto out = std::make_shared<ArrayDataType<T>>();
        if (t1 == nullptr || t2 == nullptr) {
            return out;
        }

        out->get().resize(std::max(t1->get().size(), t2->get().size()));

        for (int a = 0; a < t1->get().size(); a++) {
            out->get()[a] = t1->get()[a];
        }
        for (int a = 0; a < t2->get().size(); a++) {
            if (t2->get()[a] == 0) {
                out->get()[a] = 0;
            } else {
                out->get()[a] /= t2->get()[a];
            }
        }
        return out;
    }

    QString getName() override {
        return "Division";
    }
};
