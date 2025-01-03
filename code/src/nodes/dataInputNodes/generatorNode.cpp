//
// Created by vm on 24.18.11.
//

#include "generatorNode.h"

std::tuple<std::shared_ptr<ArrayData<double>>>
GeneratorNode::compute(std::tuple<> params, std::vector<std::shared_ptr<BaseData>> adParams) {
    if (selectedGenerator != nullptr) {
        return selectedGenerator->generate();
    }
    return nullptr;
}

QJsonObject GeneratorNode::onSave() const {
    return {
            {"generator", (qint64) typeid(*selectedGenerator).hash_code()},
            {"data",      selectedGenerator->onSave()}
    };
}

bool GeneratorNode::onLoad(QJsonObject json) {
    auto operation = json["generator"];
    if (operation == QJsonValue::Undefined) {
        return false;
    }
    std::size_t oper = operation.toInteger();
    //find operation with same hash
    for (int i = 0; i < generators.size(); i++) {
        auto obj = generators[i];
        auto oph = typeid(*obj).hash_code();
        if (oper == oph) {
            selectedGenerator = generators[i];
            auto jsonData = json["data"].toObject();

            selectedGenerator->onLoad(jsonData);
            comboBox->setCurrentIndex(i);
            onSelect(i);
            updated();
            return true;
        }
    }

    return false;
}

QString GeneratorNode::caption() const {
    return "Generator node";
}

QString GeneratorNode::name() const {
    return "Generator node";
}

QWidget *GeneratorNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget;
        comboBox = new QComboBox;
        holder = new QWidget;

        holder->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        base->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
        holder->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

        for (int i = 0; i < generators.size(); i++) {
            comboBox->addItem(generators[i]->getName(), i);
        }
        connect(comboBox, &QComboBox::activated, this, &GeneratorNode::onSelect);

        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        base->layout()->addWidget(comboBox);
        base->layout()->addWidget(holder);
        base->layout()->setSizeConstraint(QLayout::SetMaximumSize);
//        holder->layout()->setSizeConstraint(QLayout::SetMaximumSize);

        holder->layout()->addWidget(selectedGenerator->getWidgets());
        selectedGenerator->onLoad({});
        connect(selectedGenerator, &BaseGenerator::updated, this, &GeneratorNode::onUpdate);
    }
    return base;
}

void GeneratorNode::onSelect(int index) {

    auto data = selectedGenerator->onSave();
    selectedGenerator = generators[comboBox->itemData(index).toInt()];
    selectedGenerator->onLoad(data);
    //clear holder without actually deleting any widgets
    QLayoutItem *item;
    for (int i = 0; i < holder->layout()->count(); i++) {
        auto w = holder->layout()->takeAt(i)->widget();
        w->hide();
    }

    holder->layout()->addWidget(selectedGenerator->getWidgets());
    selectedGenerator->getWidgets()->show();

    holder->layout()->invalidate();
    base->layout()->invalidate();
    connect(selectedGenerator, &BaseGenerator::updated, this, &GeneratorNode::onUpdate);
    updated();
    emit embeddedWidgetSizeUpdated();
}

void GeneratorNode::onUpdate() {
    updated();
}

GeneratorNode::~GeneratorNode() {
    for (auto generator: generators) {
        delete generator;
    }
}

