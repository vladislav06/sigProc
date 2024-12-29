//
// Created by vm on 24.26.12.
//

#include <QBoxLayout>
#include <QFileDialog>
#include "fileEncoderNode.h"

QString FileEncoderNode::caption() const {
    return "File encoder node";
}

QString FileEncoderNode::name() const {
    return "File encoder";
}

QWidget *FileEncoderNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget;
        comboBox = new QComboBox;
        holder = new QWidget;
        path = new QLineEdit;
        pathButton = new QPushButton("Open explorer");
        pathHolder = new QWidget();

        connect(path, &QLineEdit::textChanged, this, &FileEncoderNode::pathChanged);
        connect(pathButton, &QPushButton::pressed, this, &FileEncoderNode::onButtonPress);

        holder->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        base->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
        holder->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
        pathHolder->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));

        for (int i = 0; i < encoders.size(); i++) {
            comboBox->addItem(encoders[i]->getName(), i);
        }
        connect(comboBox, &QComboBox::activated, this, &FileEncoderNode::onSelect);
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        base->layout()->addWidget(comboBox);

        pathHolder->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
        pathHolder->layout()->addWidget(path);
        pathHolder->layout()->addWidget(pathButton);
        base->layout()->addWidget(pathHolder);
        base->layout()->addWidget(holder);
        base->layout()->setSizeConstraint(QLayout::SetMaximumSize);

        assert(selectedEncoder != nullptr);
        assert(holder->layout() != nullptr);

        auto widget = selectedEncoder->getWidgets();
        if (widget != nullptr) {
            holder->layout()->addWidget(widget);
        }
        selectedEncoder->onLoad({});
        connect(selectedEncoder, &BaseEncoder::updated, this, &FileEncoderNode::onUpdate);
    }
    return base;
}

std::tuple<> FileEncoderNode::compute(std::tuple<std::shared_ptr<ArrayData<double>>> params,
                                      std::vector<std::shared_ptr<BaseData>> adParams) {


    if (selectedEncoder != nullptr) {
        auto file = std::make_shared<FileData>(path->text());
        //check if file can be writen to
        std::ofstream fout;
        if (!file->outputStream(fout)) {
            return {};
        }
        fout.close();
        selectedEncoder->encode(file, std::get<0>(params));
    }
    return {};
}

QJsonObject FileEncoderNode::onSave() const {
    return {
            {"encoder", (qint64) typeid(*selectedEncoder).hash_code()},
            {"data",    selectedEncoder->onSave()},
            {"file",    path->text()},
    };
}

bool FileEncoderNode::onLoad(QJsonObject json) {

    path->setText(json["file"].toString(""));
    auto operation = json["encoder"];
    if (operation == QJsonValue::Undefined) {
        return false;
    }
    std::size_t oper = operation.toInteger();
    //find operation with same hash
    for (int i = 0; i < encoders.size(); i++) {
        auto obj = encoders[i];
        auto oph = typeid(*obj).hash_code();
        if (oper == oph) {
            selectedEncoder = encoders[i];
            auto jsonData = json["data"].toObject();

            selectedEncoder->onLoad(jsonData);
            comboBox->setCurrentIndex(i);
            onSelect(i);
            updated();
            return true;
        }
    }
    return false;
}

FileEncoderNode::~FileEncoderNode() {
    for (auto encoder: encoders) {
        delete encoder;
    }
}

void FileEncoderNode::onSelect(int index) {
    auto data = selectedEncoder->onSave();
    selectedEncoder = encoders[comboBox->itemData(index).toInt()];
    selectedEncoder->onLoad(data);
    //clear holder without actually deleting any widgets
    QLayoutItem *item;
    for (int i = 0; i < holder->layout()->count(); i++) {
        auto w = holder->layout()->takeAt(i)->widget();
        w->hide();
    }

    auto widget = selectedEncoder->getWidgets();
    if (widget != nullptr) {
        holder->layout()->addWidget(widget);
        selectedEncoder->getWidgets()->show();
    }
    holder->layout()->invalidate();
    base->layout()->invalidate();
    connect(selectedEncoder, &BaseEncoder::updated, this, &FileEncoderNode::onUpdate);
    updated();
    emit embeddedWidgetSizeUpdated();
}

void FileEncoderNode::pathChanged(QString str) {
    updated();
}

void FileEncoderNode::onButtonPress() {
    QString name = QFileDialog::getSaveFileName(base, tr("Save File"), QDir::currentPath(),tr("csv (*.csv);;WFF (*.WFF);; all (*.*)"));

    if (!name.isEmpty()) {
        path->setText(name);
    }
}

void FileEncoderNode::onUpdate() {
    updated();


}
