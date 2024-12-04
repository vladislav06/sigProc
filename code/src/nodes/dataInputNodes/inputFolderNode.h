//
// Created by vm on 24.2.12.
//
#pragma once

#include <QLineEdit>
#include <QPushButton>
#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/folderDataType.h"

class InputFolderNode : public BaseNode<std::tuple<>, std::tuple<FolderDataType>> {
public:
    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<FolderDataType>>
    compute(std::tuple<> params, std::vector<std::shared_ptr<BaseDataType>> adParams) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

private Q_SLOTS:

    void valueChanged(QString str);

    void onButonPress();

private:
    QWidget *base = nullptr;
    QLineEdit *path = nullptr;
    QPushButton *openExplorer = nullptr;
};
