//
// Created by vm on 24.10.11.
//

#include <QBoxLayout>
#include "previewNode.h"


QJsonObject PreviewNode::onSave() const {
    return {};
}

bool PreviewNode::onLoad(QJsonObject json) {
    return true;
}

QString PreviewNode::caption() const {
    return "Preview";
}

QString PreviewNode::name() const {
    return "Preview";
}

QWidget *PreviewNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget();
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        text = new QPlainTextEdit();
        text->setReadOnly(true);

        base->layout()->addWidget(text);
    }
    return base;
}

std::tuple<std::shared_ptr<BaseDataType>> PreviewNode::compute(std::tuple<std::shared_ptr<BaseDataType>> type1) {
    auto obj = std::get<0>(type1);
    if (obj == nullptr) {
        text->setPlainText("");
        return type1;
    }
    text->setPlainText(obj->toString());
    return type1;
}
