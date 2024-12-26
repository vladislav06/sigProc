//
// Created by vm on 24.26.12.
//
#pragma once

#include "baseDecoder.h"

class WFFDecoder : public BaseDecoder {
public:
    QString getName() override {
        return "WFF decoder";
    }

    QWidget *getWidgets() override {
        return nullptr;
    }

    std::shared_ptr<ArrayData<double>> decode(std::shared_ptr<FileData> file) override {
        if (file == nullptr) {
            return nullptr;
        }
        if (!file->exist()) {
            return nullptr;
        }
        auto array = std::make_shared<ArrayData<double>>();

        std::ifstream fin;

        file->inputStream(fin);

        double ff = 0;

        while (fin >> ff) {
            array->get().push_back(ff);
        }
        return array;
    }

    QJsonObject onSave() override {
        return QJsonObject();
    }

    void onLoad(QJsonObject json) override {

    }

};