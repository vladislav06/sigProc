//
// Created by vm on 24.8.11.
//

#include <QBoxLayout>
#include <QJsonArray>
#include "setFieldNode.h"


QJsonObject SetFieldNode::onSave() const {
    QJsonArray json;
    for (auto field: fieldNames) {
        json.push_back(field->text());
    }
    return {{"fields", json}};
}

bool SetFieldNode::onLoad(QJsonObject json) {
    auto fiels = json["fields"];
    if (fiels == QJsonValue::Undefined) {
        return false;
    }
    auto far = fiels.toArray();
    for (int i = 0; i < fieldNames.size(); i++) {
        fieldNames[i]->setText(far[i].toString());
    }
    return true;
}

QString SetFieldNode::caption() const {
    return "Set field";
}

QString SetFieldNode::name() const {
    return "Set field";
}

QWidget *SetFieldNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget();
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        fieldNames.push_back(new QLineEdit());
        base->layout()->addWidget(fieldNames.back());
        connect(fieldNames.back(), &QLineEdit::editingFinished, this, &SetFieldNode::editingFinished);
    }
    return base;
}


void SetFieldNode::editingFinished() {
    updated();
}

std::tuple<std::shared_ptr<ObjectDataType>>
SetFieldNode::compute(std::tuple<std::shared_ptr<ObjectDataType>> params,
                      std::vector<std::shared_ptr<BaseDataType>> adParams) {
    auto object = std::get<0>(params);
    if (object == nullptr) {
        return nullptr;
    }
    if (adParams.size() != fieldNames.size()) {
        throw std::runtime_error(
                std::format("adParams.size() != fieldNames.size(),{}!={}", adParams.size(), fieldNames.size()));
    }

    for (int i = 0; i < fieldNames.size(); i++) {
        auto fieldName = fieldNames[i];
        auto data = adParams[i];
        if (!fieldName->text().isEmpty()) {
            object->fields[fieldName->text()] = data;
        }
    }
    return object;
}

void SetFieldNode::onInputConnected(int index) {
    //input object input port
    if (index == 0) {
        return;
    }

    fieldNames.push_back(new QLineEdit());
    base->layout()->addWidget(fieldNames.back());
    connect(fieldNames.back(), &QLineEdit::editingFinished, this, &SetFieldNode::editingFinished);

    addInputPort();
    emit embeddedWidgetSizeUpdated();
}

void SetFieldNode::onInputDisconnected(int index) {
    //input object input port
    if (index == 0) {
        return;
    }
    removeInputPort(index, [this, index]() {
        auto line = fieldNames.at(index - 1);
        fieldNames.erase(fieldNames.begin() + index - 1);
        delete line;
    });


    emit embeddedWidgetSizeUpdated();
}
