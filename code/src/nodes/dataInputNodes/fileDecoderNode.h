//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtWidgets>
#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/fileData.h"
#include "src/nodes/dataInputNodes/dataDecoders/baseDecoder.h"
#include "dataDecoders/wffDecoder.h"
#include "dataDecoders/csvDecoder.h"

class FileDecoderNode : public BaseNode<std::tuple<FileData>, std::tuple<ArrayData<double>>> {
public:
    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    FileDecoderNode();

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    std::tuple<std::shared_ptr<ArrayData<double>>>
    compute(std::tuple<std::shared_ptr<FileData>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override;

    ~FileDecoderNode() override;

public slots:

    void onSelect(int index);

    void onUpdate();

private:

    std::vector<BaseDecoder *> decoders = {
            new WFFDecoder(),
            new CSVDecoder(),
    };

    BaseDecoder *selectedDecoder = decoders[0];
    QWidget *base = nullptr;
    QComboBox *comboBox = nullptr;
    QWidget *holder = nullptr;
};


