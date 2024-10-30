//
// Created by vm on 24.30.10.
//



#include "src/nodes/baseNode.h"

class WaveformDataInputNode : public BaseNode<std::tuple<>, std::tuple<ArrayDataType>> {
public:


    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<ArrayDataType> compute(std::tuple<> ports) override;

    WaveformDataInputNode(){
    }
private:
    float a{};
    float b{};
};


