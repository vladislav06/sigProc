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
        maxX = 0;
        maxY = INT32_MIN;
        minY = INT32_MAX;

        //clear previous array
        for (auto &pointList: pointLists) {
            pointList.clear();
        }
        pointLists.clear();

        for (std::shared_ptr<ArrayDataType<T>> array: adParams) {
            if (array == nullptr) {
                continue;
            }
            auto size = array->get().size();

            // compress entire array to 1000 points , each point is local maximum in 1 sample which is size/1000
            QList<QPointF> list;
            size_t pointCount = 500;
            size_t sampleSize = size < pointCount ? 1 : (size / pointCount);
            list.reserve(pointCount);

            //local min max
            double LminY = INT32_MAX;
            double LmaxY = INT32_MIN;
            double prevSAvg = 0;
            int i = 0;
            for (; i < array->get().size(); i++) {
                if (array->get()[i] > LmaxY) {
                    LmaxY = array->get()[i];
                }
                if (array->get()[i] < LminY) {
                    LminY = array->get()[i];
                }
                if (i % sampleSize == 0) {
                    if (((LmaxY + LminY) / 2) > prevSAvg) {
                        list.append(QPointF(i, LmaxY));
                    } else {
                        list.append(QPointF(i, LminY));
                    }

                    //global min max
                    if (LmaxY > maxY) {
                        maxY = LmaxY;
                    }
                    if (LminY < minY) {
                        minY = LminY;
                    }
                    prevSAvg = ((LmaxY + LminY) / 2);

                    LminY = INT32_MAX;
                    LmaxY = INT32_MIN;
                }
            }
            //append remaining point
            for (int n = i; i < array->get().size(); n++) {
                list.append(QPointF(n, array->get()[i]));
            }

            if (array->get().size()-1 > maxX) {
                maxX = array->get().size()-1;
            }
            pointLists.push_back(list);
        }
        return {};
    }

    void afterCompute() override {
        chart->removeAllSeries();

        for (const auto &pointList: pointLists) {
            auto *series = new QLineSeries();
            series->append(pointList);
            chart->addSeries(series);

            series->attachAxis(xAxis);
            series->attachAxis(yAxis);
        }
        //scale to fit all
        yAxis->setMax(maxY);
        yAxis->setMin(minY);
        xAxis->setMax(maxX);
        emit this->embeddedWidgetSizeUpdated();
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
    std::vector<QList<QPointF>> pointLists;
    int maxX = 0;
    double maxY = INT32_MIN;
    double minY = INT32_MAX;

    QWidget *base = nullptr;
    QChartView *view = nullptr;
    QChart *chart = nullptr;
    QValueAxis *xAxis;
    QValueAxis *yAxis;
};


