//
// Created by vm on 24.2.12.
//
#pragma once

#include "baseData.h"
#include "fstream"
#include "QFile"
class FileData : public BaseData {
public:
    using DataType = NodeDataType<"File", "file">;
    DataType nodeType;

    explicit FileData(QString path) : filePath(std::move(path)) {}

    ~FileData() override = default;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    QString toString() override {
        return getFileContentsAsString();
    }

    QString getFilePath() {
        return filePath;
    }

    void setFilePath(QString path) {
        filePath = path;
    }

    bool exist() {
        QFile fi(filePath);
        return fi.exists();
    }

    QString getFileContentsAsString() {
        QFile fi(filePath);
        if (!fi.exists()) {
            return "";
        }

        fi.open(QFile::ReadOnly);
        QString out = fi.readAll();
        fi.close();
        return out;
    }

    QByteArray getFileContentsAsBytes() {
        QFile fi(filePath);
        if (!fi.exists()) {
            return {};
        }

        fi.open(QFile::ReadOnly);
        QByteArray out = fi.readAll();
        fi.close();
        return out;
    }

    /**
     * Opens ifstream, return true if successful, false otherwise
     * @param ifs stream to be opened
     * @return true if successful, false otherwise
     */
    bool inputStream(std::ifstream &ifs) {
        if (exist()) {
            ifs.open(filePath.toStdString());
            if (ifs.is_open()) {
                return true;
            }
        }
        return false;
    }

private:
    QString filePath;
};

inline std::string to_string(FileData &t) {
    return t.toString().toStdString();
}

