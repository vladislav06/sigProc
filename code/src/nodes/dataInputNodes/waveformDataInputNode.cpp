//
// Created by vm on 24.30.10.
//
#include "fileLoader.h"

#include "waveformDataInputNode.h"
#include "src/nodes/dataTypes/arrayDataType.h"
#include <QtWidgets>

WaveformDataInputNode::WaveformDataInputNode() {
    base = new QWidget();
    ui.setupUi(base);

    connect(ui.filePath, &QLineEdit::textChanged, this, &WaveformDataInputNode::valueChanged);
    connect(ui.openExplorer, &QPushButton::pressed, this, &WaveformDataInputNode::onButonPress);
}

QString WaveformDataInputNode::caption() const {
    return "Waveform input node";
}

QString WaveformDataInputNode::name() const {
    return "Input";
}

QWidget *WaveformDataInputNode::embeddedWidget() {
    return base;
}

std::tuple<std::shared_ptr<ArrayDataType<double>>>
WaveformDataInputNode::compute(std::tuple<> in) {
    //load file in to ArrayDataType
    auto arr = loadFileWFF(ui.filePath->text());
    return {arr};
}

void WaveformDataInputNode::valueChanged(QString str) {
    updated();
}

void WaveformDataInputNode::onButonPress() {
    QFileDialog dialog(base);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::currentPath());

    QStringList names;
    if (dialog.exec()) {
        names = dialog.selectedFiles();
    }
    assert(names.size() <= 1);
    if (!names.empty()) {
        ui.filePath->setText(names.first());
    }
}




