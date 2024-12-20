//
// Created by vm on 24.2.12.
//
#pragma once

#include <utility>
#include <QDir>
#include "baseDataType.h"
#include "fileDataType.h"
#include "arrayDataType.h"

class FolderDataType : public BaseDataType {
public:
    using DataType = NodeDataTypeType<"Folder", "folder">;
    DataType nodeType;

    explicit FolderDataType(QString path) : folderPath(std::move(path)) {}

    ~FolderDataType() override = default;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    QString toString() override {
        return folderPath;
    }

    QString getFolderPath() {
        return folderPath;
    }

    void setFolderPath(QString path) {
        folderPath = std::move(path);
    }

    std::vector<FolderDataType> getFolders() {
        QDir directory(folderPath);
        if (!directory.exists()) {
            return {};
        }
        std::vector<FolderDataType> folders;
        for (auto dir: directory.entryList(QDir::Dirs)) {
            folders.emplace_back(dir);
        }
        return folders;
    }

    std::shared_ptr<ArrayDataType<std::shared_ptr<FileDataType>>> getFiles() {
        QDir directory(folderPath);
        if (!directory.exists()) {
            return {};
        }
        auto files = std::make_shared<ArrayDataType<std::shared_ptr<FileDataType>>>();
        for (auto file: directory.entryList(QDir::Files)) {
            auto f = std::make_shared<FileDataType>(file);
            files->get().push_back(f);
        }
        return files;
    }

private:
    QString folderPath;
};
