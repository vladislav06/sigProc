//
// Created by vm on 24.31.10.
//

#include <QFileDialog>
#include "waveformDataOutputNode.h"
#include "src/nodes/dataInputNodes/fileLoader.h"

WaveformDataOutputNode::WaveformDataOutputNode() {
    base = new QWidget();
    ui.setupUi(base);

    connect(ui.filePath, &QLineEdit::textChanged, this, &WaveformDataOutputNode::valueChanged);
    connect(ui.openExplorer, &QPushButton::pressed, this, &WaveformDataOutputNode::onButonPress);
}

QString WaveformDataOutputNode::caption() const {
    return "Waveform input node";;
}

QString WaveformDataOutputNode::name() const {
    return "Output";
}

QWidget *WaveformDataOutputNode::embeddedWidget() {
    return base;
}

std::tuple<> WaveformDataOutputNode::compute(std::tuple<std::shared_ptr<ArrayDataType<double>>> in) {
    auto array = std::get<0>(in);
    if (array != nullptr) {
        saveFileWFF(ui.filePath->text(), array);
    }
    return {};
}

void WaveformDataOutputNode::valueChanged(QString str) {
    updated();
}

void WaveformDataOutputNode::onButonPress() {
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


