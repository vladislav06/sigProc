//
// Created by vm on 24.18.11.
//
#pragma once

#include <QComboBox>
#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/arrayDataType.h"
#include "generators/baseGenerator.h"
#include "generators/sineGenerator.h"
#include "generators/squareGenerator.h"
#include "src/nodes/dataInputNodes/generators/offsetGenerator.h"

class GeneratorNode : public BaseNode<std::tuple<>, std::tuple<ArrayDataType<double>>> {
public:
    std::tuple<std::shared_ptr<ArrayDataType<double>>>
    compute(std::tuple<> params, std::vector<std::shared_ptr<BaseDataType>> adParams) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

public slots:

    void onSelect(int index);

    void onUpdate();

private:
    std::vector<BaseGenerator *> generators = {
            new SineGenerator(),
            new SquareGenerator(),
            new OffsetGenerator()
    };
    BaseGenerator *selectedGenerator = generators[0];
    QWidget *base = nullptr;
    QComboBox *comboBox = nullptr;
    QWidget *holder = nullptr;
};

