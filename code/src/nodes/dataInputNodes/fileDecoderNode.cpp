//
// Created by vm on 24.30.10.
//
#include "fileDecoderNode.h"
#include <QtWidgets>

FileDecoderNode::FileDecoderNode() {

}

QString FileDecoderNode::caption() const {
    return "File decoder node";
}

QString FileDecoderNode::name() const {
    return "File Decoder";
}

QWidget *FileDecoderNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget;
        comboBox = new QComboBox;
        holder = new QWidget;

        holder->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        base->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
        holder->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));

        for (int i = 0; i < decoders.size(); i++) {
            comboBox->addItem(decoders[i]->getName(), i);
        }
        connect(comboBox, &QComboBox::activated, this, &FileDecoderNode::onSelect);
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        base->layout()->addWidget(comboBox);
        base->layout()->addWidget(holder);
        base->layout()->setSizeConstraint(QLayout::SetMaximumSize);

        assert(selectedDecoder != nullptr);
        assert(holder->layout() != nullptr);

        auto widget = selectedDecoder->getWidgets();
        if (widget != nullptr) {
            holder->layout()->addWidget(widget);
        }
        selectedDecoder->onLoad({});
        connect(selectedDecoder, &BaseDecoder::updated, this, &FileDecoderNode::onUpdate);
    }
    return base;
}

QJsonObject FileDecoderNode::onSave() const {
    return {
            {"decoder", (qint64) typeid(*selectedDecoder).hash_code()},
            {"data",    selectedDecoder->onSave()}
    };
}

bool FileDecoderNode::onLoad(QJsonObject json) {
    auto operation = json["decoder"];
    if (operation == QJsonValue::Undefined) {
        return false;
    }
    std::size_t oper = operation.toInteger();
    //find operation with same hash
    for (int i = 0; i < decoders.size(); i++) {
        auto obj = decoders[i];
        auto oph = typeid(*obj).hash_code();
        if (oper == oph) {
            selectedDecoder = decoders[i];
            auto jsonData = json["data"].toObject();

            selectedDecoder->onLoad(jsonData);
            comboBox->setCurrentIndex(i);
            onSelect(i);
            updated();
            return true;
        }
    }
    return false;
}

std::tuple<std::shared_ptr<ArrayData<double>>>
FileDecoderNode::compute(std::tuple<std::shared_ptr<FileData>> params,
                         std::vector<std::shared_ptr<BaseData>> adParams) {
    if (selectedDecoder != nullptr) {
        return selectedDecoder->decode(std::get<0>(params));
    }
    return nullptr;
}

FileDecoderNode::~FileDecoderNode() {
    for (auto decoder: decoders) {
        delete decoder;
    }
}

void FileDecoderNode::onUpdate() {
    updated();
}

void FileDecoderNode::onSelect(int index) {
    auto data = selectedDecoder->onSave();
    selectedDecoder = decoders[comboBox->itemData(index).toInt()];
    selectedDecoder->onLoad(data);
    //clear holder without actually deleting any widgets
    QLayoutItem *item;
    for (int i = 0; i < holder->layout()->count(); i++) {
        auto w = holder->layout()->takeAt(i)->widget();
        w->hide();
    }

    auto widget = selectedDecoder->getWidgets();
    if (widget != nullptr) {
        holder->layout()->addWidget(widget);
        selectedDecoder->getWidgets()->show();
    }
    holder->layout()->invalidate();
    base->layout()->invalidate();
    connect(selectedDecoder, &BaseDecoder::updated, this, &FileDecoderNode::onUpdate);
    updated();
    emit embeddedWidgetSizeUpdated();
}