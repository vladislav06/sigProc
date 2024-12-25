//
// Created by vm on 24.31.10.
//

#include <QFileDialog>
#include "waveformDataOutputNode.h"
#include "src/nodes/dataInputNodes/fileLoader.h"

WaveformDataOutputNode::WaveformDataOutputNode() {

}

QString WaveformDataOutputNode::caption() const {
    return "Waveform input node";;
}

QString WaveformDataOutputNode::name() const {
    return "Output";
}

QWidget *WaveformDataOutputNode::embeddedWidget() {
    if(base== nullptr){
        base = new QWidget();
        ui.setupUi(base);
        connect(ui.filePath, &QLineEdit::textChanged, this, &WaveformDataOutputNode::valueChanged);
        connect(ui.openExplorer, &QPushButton::pressed, this, &WaveformDataOutputNode::onButonPress);
    }
    return base;
}



void WaveformDataOutputNode::valueChanged(QString str) {
    updated();
}

void WaveformDataOutputNode::onButonPress() {
    QString name = QFileDialog::getSaveFileName(base, tr("Save File"), QDir::currentPath(), tr("Waveform (*.WFF)"));

    if (!name.isEmpty()) {
        ui.filePath->setText(name);
    }
}

QJsonObject WaveformDataOutputNode::onSave() const {
    return {
            {"path", ui.filePath->text()},
    };
}

bool WaveformDataOutputNode::onLoad(QJsonObject json) {
    auto path = json["path"];
    if (path == QJsonValue::Undefined) {
        return false;
    }
    ui.filePath->setText(path.toString());
    return true;
}

std::tuple<> WaveformDataOutputNode::compute(std::tuple<std::shared_ptr<ArrayData<double>>> params,
                                             std::vector<std::shared_ptr<BaseData>> adParams) {
    auto array = std::get<0>(params);
    if (array != nullptr) {
        saveFileWFF(ui.filePath->text(), array);
    }
    return {};}


