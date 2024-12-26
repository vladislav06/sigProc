//
// Created by vm on 24.26.12.
//
#pragma once

#include <QComboBox>
#include <QPushButton>
#include "QLineEdit"
#include "src/nodes/baseNode.h"
#include "dataEncoders/baseEncoder.h"
#include "dataEncoders/wffEncoder.h"
#include "dataEncoders/csvEncoder.h"
class FileEncoderNode : public BaseNode<std::tuple<ArrayData<double>>, std::tuple<>> {
public:
    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<> compute(std::tuple<std::shared_ptr<ArrayData<double>>> params,
                         std::vector<std::shared_ptr<BaseData>> adParams) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    ~FileEncoderNode() override;


public slots:

    void onSelect(int index);

    void onUpdate();

    void pathChanged(QString str);

    void onButtonPress();

private:

    std::vector<BaseEncoder *> encoders = {
            new WFFEncoder(),
            new CSVEncoder(),
    };

    BaseEncoder *selectedEncoder = encoders[0];
    QWidget *base = nullptr;
    QComboBox *comboBox = nullptr;
    QLineEdit *path = nullptr;
    QPushButton *pathButton = nullptr;
    QWidget *pathHolder = nullptr;
    QWidget *holder = nullptr;
};

