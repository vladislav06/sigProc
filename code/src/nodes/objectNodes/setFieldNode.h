//
// Created by vm on 24.8.11.
//
#pragma once

#include <QLineEdit>
#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/objectData.h"

class SetFieldNode : public BaseNode<std::tuple<ObjectData>, std::tuple<ObjectData>, BaseData> {
public:
    SetFieldNode() {
        addInputPort();
    }

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;




    void onInputConnected(int index) override;

    void onInputDisconnected(int index) override;
public slots:

    void editingFinished();

    std::tuple<std::shared_ptr<ObjectData>> compute(std::tuple<std::shared_ptr<ObjectData>> params,
                                                    std::vector<std::shared_ptr<BaseData>> adParams) override;


private:
    QWidget *base = nullptr;
    std::vector<QLineEdit *> fieldNames{};


};

