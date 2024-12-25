//
// Created by vm on 24.2.12.
//
#pragma once

#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/folderData.h"

class GetFileNode : public BaseNode<std::tuple<FolderData>, std::tuple<ArrayData<std::shared_ptr<FileData>>>> {
public:
    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ArrayData<std::shared_ptr<FileData>>>> compute(std::tuple<std::shared_ptr<FolderData>> params,
                                                                              std::vector<std::shared_ptr<BaseData>> adParams) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

};


