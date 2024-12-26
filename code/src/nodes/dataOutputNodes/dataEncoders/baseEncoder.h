//
// Created by vm on 24.26.12.
//
#pragma once

#include <QObject>
#include "src/nodes/dataTypes/fileData.h"
#include "src/nodes/dataTypes/arrayData.h"


class BaseEncoder : public QObject {
Q_OBJECT

public:
    BaseEncoder() = default;

    virtual QString getName() = 0;

    /**
     * Must return newly allocated widget
     * @return
     */
    virtual QWidget *getWidgets() = 0;

    virtual void encode(std::shared_ptr<FileData> file, std::shared_ptr<ArrayData<double>> array) = 0;

    virtual QJsonObject onSave() = 0;

    virtual void onLoad(QJsonObject json) = 0;

    ~BaseEncoder() override = default;

public slots:

    void onUpdate();

Q_SIGNALS:

    void updated();
};

