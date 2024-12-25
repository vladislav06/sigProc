//
// Created by vm on 24.2.12.
//
#pragma once

#include <utility>
#include <QDir>
#include "baseData.h"
#include "fileData.h"
#include "arrayData.h"

class FolderData : public BaseData {
public:
    using DataType = NodeDataType<"Folder", "folder">;
    DataType nodeType;

    explicit FolderData(QString path) : folderPath(std::move(path)) {}

    ~FolderData() override = default;

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

    std::vector<FolderData> getFolders() {
        QDir directory(folderPath);
        if (!directory.exists()) {
            return {};
        }
        std::vector<FolderData> folders;
        for (auto dir: directory.entryList(QDir::Dirs)) {
            folders.emplace_back(dir);
        }
        return folders;
    }

    std::shared_ptr<ArrayData<std::shared_ptr<FileData>>> getFiles() {
        QDir directory(folderPath);
        if (!directory.exists()) {
            return {};
        }
        auto files = std::make_shared<ArrayData<std::shared_ptr<FileData>>>();
        for (auto file: directory.entryList(QDir::Files)) {
            auto f = std::make_shared<FileData>(folderPath + QDir::separator() + file);
            files->get().push_back(f);
        }
        return files;
    }

private:
    QString folderPath;
};
