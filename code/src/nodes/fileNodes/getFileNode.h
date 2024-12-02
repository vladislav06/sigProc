//
// Created by vm on 24.2.12.
//
#pragma once

#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/folderDataType.h"

class GetFileNode : public BaseNode<std::tuple<FolderDataType>, std::tuple<ArrayDataType<FileDataType>>> {
public:
    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ArrayDataType<FileDataType>>> compute(std::tuple<std::shared_ptr<FolderDataType>> params,
                                                                     std::vector<std::shared_ptr<BaseDataType>> adParams) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

};


