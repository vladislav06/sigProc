//
// Created by vm on 24.27.11.
//

#include "fmModulationNode.h"


QJsonObject FmModulationNode::onSave() const {
    return QJsonObject();
}

bool FmModulationNode::onLoad(QJsonObject json) {
    return false;
}

QString FmModulationNode::caption() const {
    return "FM modulation";
}

QString FmModulationNode::name() const {
    return "FM modulation";
}

QWidget *FmModulationNode::embeddedWidget() {
    return nullptr;
}


std::tuple<std::shared_ptr<ArrayDataType<double, "Modulated">>> FmModulationNode::compute(
        std::tuple<std::shared_ptr<ArrayDataType<double, "Carrier">>, std::shared_ptr<ArrayDataType<double, "Modulator">>> params,
        std::vector<std::shared_ptr<BaseDataType>> adParams) {
    if (std::get<0>(params) == nullptr || std::get<1>(params) == nullptr) {
        return {};
    }

    auto carrier = std::get<0>(params);
    auto modulator = std::get<1>(params);
    auto result = std::make_shared<ArrayDataType<double, "Modulated">>();

    /*
     * to modulate carrier, skip or interpolate amount of samples in carrier specified in modulator
     */
    double carrierCounter = 0;

    for (auto m: modulator->get()) {
        double shift = m;
        if (shift > 0) {
            carrierCounter += shift + 1;
            // linearly interpolates any point between int(carrierCounter) and  int(carrierCounter+1)
            // allows independent sampling of carrier
            if (carrierCounter + 1 >= carrier->get().size()) { return {result}; }
            auto res = std::lerp(carrier->get()[carrierCounter], carrier->get()[carrierCounter + 1],
                                 carrierCounter - ((int) carrierCounter));
            result->get().push_back(res);

        } else if (shift < 0) {
            shift = 1 / (-shift + 1);
            carrierCounter += shift;

            if ((carrierCounter + 1) >= carrier->get().size()) { return {result}; }
            auto res = std::lerp(carrier->get()[carrierCounter], carrier->get()[carrierCounter + 1],
                                 carrierCounter - ((int) carrierCounter));
            result->get().push_back(res);

        } else {
            carrierCounter++;
            if (carrierCounter >= carrier->get().size()) { return {result}; }
            result->get().push_back(carrier->get()[carrierCounter]);
        }
        if (carrierCounter >= carrier->get().size()) { return {result}; }

    }

    return {result};
}