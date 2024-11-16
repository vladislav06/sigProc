//
// Created by vm on 24.30.10.
//
#include "fileLoader.h"

#include "waveformDataInputNode.h"
#include "src/nodes/dataTypes/arrayDataType.h"
#include <QtWidgets>

WaveformDataInputNode::WaveformDataInputNode() {

}

QString WaveformDataInputNode::caption() const {
    return "Waveform input node";
}

QString WaveformDataInputNode::name() const {
    return "Input";
}

QWidget *WaveformDataInputNode::embeddedWidget() {
    if (base == nullptr) {
        base = new QWidget();
        ui.setupUi(base);

        connect(ui.filePath, &QLineEdit::textChanged, this, &WaveformDataInputNode::valueChanged);
        connect(ui.openExplorer, &QPushButton::pressed, this, &WaveformDataInputNode::onButonPress);
    }
    return base;
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

QJsonObject WaveformDataInputNode::onSave() const {
    return {
            {"path", ui.filePath->text()},
    };
}

bool WaveformDataInputNode::onLoad(QJsonObject json) {
    auto path = json["path"];
    if (path == QJsonValue::Undefined) {
        return false;
    }
    ui.filePath->setText(path.toString());
    return true;
}

std::tuple<std::shared_ptr<ArrayDataType<double>>>
WaveformDataInputNode::compute(std::tuple<> params, std::vector<std::shared_ptr<BaseDataType>> adParams) {
    //load file in to ArrayDataType
    auto arr = loadFileWFF(ui.filePath->text());
    return {arr};}




