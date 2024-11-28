//
// Created by vm on 24.27.11.
//

#include "fftNode.h"
#include "src/math/fft/fft.h"

QJsonObject FftNode::onSave() const {
    return QJsonObject();
}

bool FftNode::onLoad(QJsonObject json) {
    return false;
}

QString FftNode::caption() const {
    return "FFT transform";
}

QString FftNode::name() const {
    return "FFT transform";
}

QWidget *FftNode::embeddedWidget() {
    return nullptr;
}

std::tuple<std::shared_ptr<ArrayDataType<double>>, std::shared_ptr<ArrayDataType<double>>, std::shared_ptr<ArrayDataType<double>>, std::shared_ptr<ArrayDataType<double>>>
FftNode::compute(std::tuple<std::shared_ptr<ArrayDataType<double>>> params,
                 std::vector<std::shared_ptr<BaseDataType>> adParams) {
    if (std::get<0>(params) == nullptr) {
        return {};
    }

    auto array = Fft::calculateFft(std::get<0>(params)->get());

    auto amplitude = std::make_shared<ArrayDataType<double>>(std::vector<double>((array.size() / 2)+1));
    auto phase = std::make_shared<ArrayDataType<double>>(std::vector<double>((array.size() / 2)+1));
    auto real = std::make_shared<ArrayDataType<double>>(std::vector<double>(array.size()));
    auto imag = std::make_shared<ArrayDataType<double>>(std::vector<double>(array.size()));
    if (array.size() < 2) {
        return {};
    }

    for (size_t i = 0; i < array.size(); i++) {
        real->get()[i] = array[i][0];
        imag->get()[i] = array[i][1];
    }

    for (size_t i = array.size() / 2; i < array.size(); i++) {
        int index = (i - (array.size() / 2));
        assert(index < amplitude->get().size());
        assert(index < imag->get().size());

        amplitude->get()[index] = sqrt(array[i][0] * array[i][0] + array[i][1] * array[i][1]);
        imag->get()[index] = atan2(array[i][1], array[i][0]);
    }

    return {amplitude, phase, real, imag};
}

