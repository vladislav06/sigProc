//
// Created by vm on 24.18.11.
//
#pragma once

#include "src/nodes/dataTypes/arrayData.h"
#include "QWidget"
#include "QObject"

class BaseGenerator : public QObject {
Q_OBJECT

public:
    BaseGenerator() = default;

    virtual QString getName() = 0;

    /**
     * Must return newly allocated widget
     * @return
     */
    virtual QWidget *getWidgets() = 0;

    virtual std::shared_ptr<ArrayData<double>> generate() = 0;

    virtual QJsonObject onSave() = 0;

    virtual void onLoad(QJsonObject json) = 0;

     ~BaseGenerator() override = default;

public slots:

    void onUpdate();

Q_SIGNALS:

    void updated();

};