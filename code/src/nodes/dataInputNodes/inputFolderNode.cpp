//
// Created by vm on 24.2.12.
//

#include <QBoxLayout>
#include <QFileDialog>
#include "inputFolderNode.h"

QString InputFolderNode::caption() const {
    return "Read Folder";
}

QString InputFolderNode::name() const {
    return "Read Folder";
}

QWidget *InputFolderNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget();
        path = new QLineEdit();
        openExplorer = new QPushButton("Open explorer");

        base->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
        base->layout()->addWidget(path);
        base->layout()->addWidget(openExplorer);

        connect(path, &QLineEdit::textChanged, this, &InputFolderNode::valueChanged);
        connect(openExplorer, &QPushButton::pressed, this, &InputFolderNode::onButonPress);

    }
    return base;
}

std::tuple<std::shared_ptr<FolderData>>
InputFolderNode::compute(std::tuple<> params, std::vector<std::shared_ptr<BaseData>> adParams) {
    return {std::make_shared<FolderData>(path->text())};
}

QJsonObject InputFolderNode::onSave() const {
    return {
            {"path", path->text()},
    };
}

bool InputFolderNode::onLoad(QJsonObject json) {
    path->setText(json["path"].toString(""));
    return true;
}

void InputFolderNode::valueChanged(QString str) {
    updated();
}

void InputFolderNode::onButonPress() {
    QFileDialog dialog(base);

    dialog.setFileMode(QFileDialog::Directory);
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
