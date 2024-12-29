//
// Created by vm on 24.26.12.
//
#pragma once

#include "baseEncoder.h"

class WFFEncoder : public BaseEncoder {
public:
    QString getName() override {
        return "WFF encoder";
    }

    QWidget *getWidgets() override {
        return nullptr;
    }

    void encode(std::shared_ptr<FileData> file, std::shared_ptr<ArrayData<double>> array) override {
        if (array == nullptr) {
            return;
        }

        std::ofstream fout;

        if (!file->outputStream(fout)) {
            fout.close();
            return;
        }

        for (auto d: array->get()) {
            fout << d << "\n";
        }

        fout.close();
    }

    QJsonObject onSave() override {
        return QJsonObject();
    }

    void onLoad(QJsonObject json) override {
    }
};