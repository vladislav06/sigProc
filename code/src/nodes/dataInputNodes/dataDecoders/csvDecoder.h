//
// Created by vm on 24.26.12.
//
#pragma once

#include "baseDecoder.h"
#include <QBoxLayout>
#include <QRegularExpressionValidator>

class CSVDecoder : public BaseDecoder {
public:
    CSVDecoder() {
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
                connect(box, &QLineEdit::editingFinished, this, &CSVDecoder::onUpdate);
            }, pair.second);
        }
    }

    QString getName() override {
        return "CSV decoder";
    }

    QWidget *getWidgets() override {
        return base;
    }

    std::shared_ptr<ArrayData<double>> decode(std::shared_ptr<FileData> file) override {
        if (file == nullptr) {
            return nullptr;
        }
        if (!file->exist()) {
            return nullptr;
        }
        auto array = std::make_shared<ArrayData<double>>();

        std::ifstream fin;

        file->inputStream(fin);

        std::string l;

        //determine index of column
        std::getline(fin, l);
        QString line = QString::fromStdString(l);
        auto tokens = line.split(delimiter->text());

        int columnIndex = -1;

        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] == field->text()) {
                columnIndex = i;
                break;
            }
        }

        if (columnIndex == -1) {
            return {};
        }


        while (std::getline(fin, l)) {
            QString line = QString::fromStdString(l);
            auto tokens = line.split(delimiter->text());

            if (tokens.size() <= columnIndex) {
                continue;
            }
            array->get().push_back(tokens[columnIndex].toDouble());
        }


        return array;
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