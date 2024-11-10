//
// Created by vm on 24.8.11.
//
#pragma once

#include <QLineEdit>
#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/objectDataType.h"

class SetFieldNode : public BaseNode<std::tuple<ObjectDataType, BaseDataType>, std::tuple<ObjectDataType>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ObjectDataType>>
    compute(std::tuple<std::shared_ptr<ObjectDataType>, std::shared_ptr<BaseDataType>> type1) override;

public slots:
    void editingFinished();

private:
    QWidget *base = nullptr;
    QLineEdit *fieldName= nullptr;


};

