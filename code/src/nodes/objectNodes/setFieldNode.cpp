//
// Created by vm on 24.8.11.
//

#include <QBoxLayout>
#include "setFieldNode.h"


QJsonObject SetFieldNode::onSave() const {
    return QJsonObject();
}

bool SetFieldNode::onLoad(QJsonObject json) {
    return false;
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
        fieldName = new QLineEdit();
        base->layout()->addWidget(fieldName);
        connect(fieldName, &QLineEdit::editingFinished, this, &SetFieldNode::editingFinished);
    }
    return base;
}

std::tuple<std::shared_ptr<ObjectDataType>>
SetFieldNode::compute(std::tuple<std::shared_ptr<ObjectDataType>, std::shared_ptr<BaseDataType>> type1) {
    auto object = std::get<0>(type1);
    auto data = std::get<1>(type1);
    if (object == nullptr || data == nullptr) {
        return nullptr;
    }
    if (!fieldName->text().isEmpty()) {
        object->fields[fieldName->text()] = data;

    }
    return object;
}

void SetFieldNode::editingFinished() {
    updated();
}
