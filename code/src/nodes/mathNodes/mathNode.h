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
#include "operations/multiplicationOperation.h"
#include "operations/divisionOperation.h"

#define TYPE ArrayData<T>

template<typename T>
class MathNode : public BaseNode<std::tuple<TYPE, TYPE >, std::tuple<TYPE>> {
public:
    MathNode(): BaseNode<std::tuple<ArrayData<T>, ArrayData<T>>, std::tuple<ArrayData<T>>>({"Input a","Input b"},{"result"}) {
        selectedOp = groups[0].ops[0];
    }

    QString caption() const override {
        return "Math node";
    }

    QString name() const override {
        return "Math node";
    }

    QWidget *embeddedWidget() override {
        if (base == nullptr) {
            base = new QWidget();
            button = new QPushButton("Addition");
            base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
            base->layout()->addWidget(button);

            popup = new QWidget(base);
            popup->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Popup);
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
        return base;
    }


public slots:

    void onPress(bool checked) {
        QRect rect = base->geometry();
        QPoint bottomLeft = base->mapToGlobal(rect.topLeft());
        bottomLeft -= QPoint(rect.width() / 2, rect.height() / 2);
        popup->setGeometry(QRect(bottomLeft, QSize(100, 50)));
        popup->show();
    }

public:
    QJsonObject onSave() const override {
        return {
                {"op", (qint64) typeid(*selectedOp).hash_code()},
        };
    }

    bool onLoad(QJsonObject json) override {
        auto operation = json["op"];
        if (operation == QJsonValue::Undefined) {
            return false;
        }
        std::size_t oper = operation.toInteger();
        //find operation with same hash
        for (auto group: groups) {
            for (auto op: group.ops) {
                auto oph = typeid(*op).hash_code();
                if (oper == oph) {
                    selectedOp = op;
                    if (this->button != nullptr) {
                        this->button->setText(op->getName());
                    }

                    return true;
                }
            }
        }
        return false;
    };

    virtual ~MathNode() {
        for (auto group: groups) {
            for (auto op: group.ops) {
                delete op;
            }
        }
    }

    std::tuple<std::shared_ptr<ArrayData<T>>>
    compute(std::tuple<std::shared_ptr<ArrayData<T>>, std::shared_ptr<ArrayData<T>>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override {
        return {selectedOp->compute(std::get<0>(params), std::get<1>(params))};
    }



private:
    QWidget *base = nullptr;
    QPushButton *button = nullptr;
    BaseOperation<TYPE > *selectedOp = nullptr;
    QWidget *popup = nullptr;


    struct OpGroup {
        QString name;
        std::vector<BaseOperation<TYPE > *> ops;
    };

    /**
     * Configuration structure that holds groups with operations
     */
    std::vector<OpGroup> groups = {
            {.name="Math", .ops={
                    new AdditionOperation<TYPE >(),
                    new SubtractionOperation<TYPE >(),
            }},
            {.name="Math", .ops={
                    new MultiplicationOperation<TYPE >(),
                    new DivisionOperation<TYPE >(),

            }},
    };
};


