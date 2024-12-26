//
// Created by vm on 24.26.12.
//
#pragma once

#include "QObject"
#include "src/nodes/dataTypes/fileData.h"
#include "src/nodes/dataTypes/arrayData.h"

class BaseDecoder : public QObject {
Q_OBJECT

public:
    BaseDecoder() = default;

    virtual QString getName() = 0;

    /**
     * Must return newly allocated widget
     * @return
     */
    virtual QWidget *getWidgets() = 0;

    virtual std::shared_ptr<ArrayData<double>> decode(std::shared_ptr<FileData> file) = 0;

    virtual QJsonObject onSave() = 0;

    virtual void onLoad(QJsonObject json) = 0;

    ~BaseDecoder() override = default;

public slots:

    void onUpdate();

Q_SIGNALS:

    void updated();
};