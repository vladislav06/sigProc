//
// Created by vm on 24.31.10.
//

#pragma once

#include <QString>
#include <filesystem>
#include "src/nodes/dataTypes/arrayDataType.h"
#include "fstream"

inline std::shared_ptr<ArrayDataType<double>> loadFileWFF(QString path) {
    auto array = std::make_shared<ArrayDataType<double>>();

    if (!std::filesystem::exists(path.toStdString())) {
        return array;
    }
    std::ifstream fin(path.toStdString());
    double ff = 0;

    while (fin >> ff) {
        array->get().push_back(ff);
    }
    return array;
}

inline void saveFileWFF(QString path, std::shared_ptr<ArrayDataType<double>> array) {
    std::ofstream fout(path.toStdString());
    for (auto d: array->get()) {
        fout << d << "\n";
    }
}