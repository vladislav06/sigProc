//
// Created by vm on 24.2.12.
//

#include <QBoxLayout>
#include <QDir>
#include <QFileDialog>
#include "inputFileNode.h"

QString InputFileNode::caption() const {
    return "Read file";
}

QString InputFileNode::name() const {
    return "Read file";
}

QWidget *InputFileNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget();
        path = new QLineEdit();
        openExplorer = new QPushButton("Open explorer");

        base->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
        base->layout()->addWidget(path);
        base->layout()->addWidget(openExplorer);

        connect(path, &QLineEdit::textChanged, this, &InputFileNode::valueChanged);
        connect(openExplorer, &QPushButton::pressed, this, &InputFileNode::onButonPress);

    }
    return base;
}

std::tuple<std::shared_ptr<FileDataType>>
InputFileNode::compute(std::tuple<> params, std::vector<std::shared_ptr<BaseDataType>> adParams) {
    return {std::make_shared<FileDataType>(path->text())};
}

QJsonObject InputFileNode::onSave() const {
    return {
            {"path", path->text()},
    };
}

bool InputFileNode::onLoad(QJsonObject json) {
    auto s = json["path"].toString(" ");
    if (s.isNull()||s.isEmpty()) {
        s = " ";
    }
    path->setText(s);
    return true;
}

void InputFileNode::valueChanged(QString str) {
    updated();

}

void InputFileNode::onButonPress() {
    QFileDialog dialog(base);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::currentPath());

    QStringList names;
    if (dialog.exec()) {
        names = dialog.selectedFiles();
    }

    assert(names.size() <= 1);
    if (!names.empty()) {
        path->setText(names.first());
    }
}
