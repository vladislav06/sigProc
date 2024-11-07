//
// Created by vm on 24.7.11.
//
#pragma once

#include <QComboBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QBoxLayout>
#include "src/nodes/baseNode.h"
#include <QHeaderView>
#include <QPushButton>
#include "operations/baseOperation.h"
#include "operations/additionOperation.h"
#include "operations/subtractionOperation.h"

#define  DataType ArrayDataType<T>

template<typename T>
class MathNode : public BaseNode<std::tuple<DataType, DataType >, std::tuple<DataType>> {
public:
    MathNode() {
        base = new QWidget();
        button = new QPushButton("Addition");
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
        base->layout()->addWidget(button);

        popup = new QWidget(base);
        popup->setWindowFlags(Qt::Window | Qt::FramelessWindowHint| Qt::Popup);
        popup->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
        for (auto group: groups) {
            QWidget *column = new QWidget();
            popup->layout()->addWidget(column);
            column->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
            for (auto op: group.ops) {
                QPushButton *btn = new QPushButton(op->getName());
                column->layout()->addWidget(btn);
                this->connect(btn, &QPushButton::clicked, this, [op, this](bool) {
                    this->popup->close();
                    this->selectedOp = op;
                    this->updated();
                    this->button->setText(op->getName());
                });
            }
        }

        this->connect(button, &QPushButton::clicked, this, &MathNode::onPress);
    }

    QString caption() const override {
        return "Math node";
    }

    QString name() const override {
        return "Math node";
    }

    QWidget *embeddedWidget() override {
        return base;
    }

    std::tuple<std::shared_ptr<ArrayDataType<T>>>
    compute(std::tuple<std::shared_ptr<ArrayDataType<T>>, std::shared_ptr<ArrayDataType<T>>> type1) override {

        return {selectedOp->compute(std::get<0>(type1),std::get<1>(type1))};
    }


public slots:

    void onPress(bool checked) {
        QRect rect = base->geometry();
        QPoint bottomLeft = base->mapToGlobal(rect.topLeft());
        bottomLeft -= QPoint(rect.width() / 2, rect.height() / 2);
        popup->setGeometry(QRect(bottomLeft, QSize(100, 50)));
        popup->show();
    };

private:
    QWidget *base;
    QPushButton *button;
    QWidget *popup;


    struct Group {
        QString name;
        std::vector<BaseOperation<DataType > *> ops;
    };

    std::vector<Group> groups = {
            {.name="Math", .ops={
                    new AdditionOperation<DataType >(),
            }},
            {.name="Math", .ops={
                    new SubtractionOperation<DataType >(),
            }}
    };
    BaseOperation<DataType > *selectedOp = groups[0].ops[0];


};


