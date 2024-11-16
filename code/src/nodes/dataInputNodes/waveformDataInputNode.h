//
// Created by vm on 24.30.10.
//



#include <QtWidgets>
#include "src/nodes/baseNode.h"
#include "ui_waveformDataInputNode.h"

class WaveformDataInputNode : public BaseNode<std::tuple<>, std::tuple<ArrayDataType<double>>> {
public:


    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    WaveformDataInputNode();


    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    std::tuple<std::shared_ptr<ArrayDataType<double>>>
    compute(std::tuple<> params, std::vector<std::shared_ptr<BaseDataType>> adParams) override;

private Q_SLOTS:

    void valueChanged(QString str);

    void onButonPress();

private:
    Ui::waveformDataInputNode ui;
    QWidget *base = nullptr;
};


