//
// Created by vm on 24.26.12.
//
#pragma once

#include <QLabel>
#include "baseEncoder.h"

class CSVEncoder : public BaseEncoder {
public:
    CSVEncoder() {
        base = new QWidget();
        base->setLayout(new QBoxLayout(QVBoxLayout::TopToBottom));

        delimiter = new QLineEdit();
        field = new QLineEdit();
        // only one char is allowed
        auto *rv = new QRegularExpressionValidator(QRegularExpression("^.$"));
        delimiter->setValidator(rv);

        std::vector<std::pair<QString, std::variant<QLineEdit *>>> pairs = {{"delimiter", delimiter},
                                                                            {"field",     field}};

        for (auto pair: pairs) {
            std::visit([pair, this](auto box) {
                QWidget *p = new QWidget();
                p->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
                p->layout()->addWidget(new QLabel(pair.first));
                p->layout()->addWidget(box);
                box->resize(150, box->height());
                base->layout()->addWidget(p);
                connect(box, &QLineEdit::editingFinished, this, &CSVEncoder::onUpdate);
            }, pair.second);
        }
    }

    QString getName() override {
        return "CSV encoder";
    }

    QWidget *getWidgets() override {
        return base;
    }

    void encode(std::shared_ptr<FileData> file, std::shared_ptr<ArrayData<double>> array) override {
        if (array == nullptr) {
            return;
        }

        std::ofstream fout;

        if (!file->outputStream(fout)) {
            fout.close();
            return;
        }

        fout << field->text().toStdString() << delimiter->text().toStdString() << "\n";
        for (auto d: array->get()) {
            fout << d << delimiter->text().toStdString() << "\n";
        }

        fout.close();
    }

    QJsonObject onSave() override {
        return {
                {"delimiter", delimiter->text()},
                {"field",     field->text()},
        };
    }

    void onLoad(QJsonObject json) override {
        delimiter->setText(json["delimiter"].toString(","));
        field->setText(json["field"].toString(""));
    }

private:
    QWidget *base = nullptr;
    QLineEdit *delimiter = nullptr;
    QLineEdit *field = nullptr;
};