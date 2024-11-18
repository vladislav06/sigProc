//
// Created by vm on 24.18.11.
//
#pragma once

#include "src/nodes/baseNode.h"
#include "src/nodes/dataTypes/arrayDataType.h"
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>
#include <QBoxLayout>
#include "QChart"

template<typename T>
class GraphPreviewNode : public BaseNode<std::tuple<>, std::tuple<>, ArrayDataType<T>> {
public:
    GraphPreviewNode() {
        this->addInputPort();

    }

    QJsonObject onSave() const override {
        return QJsonObject();
    }

    bool onLoad(QJsonObject json) override {
        return false;
    }

    QString caption() const override {
        return "Array preview";
    }

    QString name() const override {
        return "Array preview";
    }

    QWidget *embeddedWidget() override {
        if (base == nullptr) {
            base = new QWidget();
            base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

            view = new QChartView();
            chart = new QChart();
            view->setChart(chart);

            base->layout()->addWidget(view);

            xAxis = new QValueAxis;
            yAxis = new QValueAxis;

            chart->addAxis(xAxis, Qt::AlignBottom);
            chart->addAxis(yAxis, Qt::AlignLeft);

        }
        return base;
    }

    std::tuple<> compute(std::tuple<> params, std::vector<std::shared_ptr<ArrayDataType<T>>> adParams) override {
        chart->removeAllSeries();
        int maxX = 0;
        double maxY = INT32_MIN;
        double minY = INT32_MAX;

        for (std::shared_ptr<ArrayDataType<T>> array: adParams) {
            if (array == nullptr) {
                continue;
            }
            QLineSeries *series = new QLineSeries();
            QList<QPointF> myList;
            myList.reserve(array->get().size());
            for (int i = 0; i < array->get().size(); i++) {
                myList.append(QPointF(i, array->get()[i]));
                if (array->get()[i] > maxY) {
                    maxY = array->get()[i];
                }
                if (array->get()[i] < minY) {
                    minY = array->get()[i];
                }
            }
            if (array->get().size() > maxX) {
                maxX = array->get().size();
            }
            series->append(myList);
            chart->addSeries(series);

            series->attachAxis(xAxis);
            series->attachAxis(yAxis);
        }
        //scale to fit all
        yAxis->setMax(maxY);
        yAxis->setMin(minY);
        xAxis->setMax(maxX);
        emit this->embeddedWidgetSizeUpdated();

        return {};
    }

    void onInputConnected(int index) override {
        this->addInputPort();
        emit this->embeddedWidgetSizeUpdated();
    }

    void onInputDisconnected(int index) override {
        this->removeInputPort(index, [this, index]() {
        });
        emit this->embeddedWidgetSizeUpdated();
    }

private:
    QWidget *base = nullptr;
    QChartView *view = nullptr;
    QChart *chart = nullptr;
    QValueAxis *xAxis;
    QValueAxis *yAxis;
};


