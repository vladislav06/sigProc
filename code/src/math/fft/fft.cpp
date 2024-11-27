//
// Created by vm on 24.27.11.
//

#include <fftw3.h>
#include "fft.h"

std::vector<double[2]> Fft::calculateFft(std::vector<double> &input) {

    if (input.size() < 2) {
        return {};
    }
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * input.size());
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * input.size());

    for (int i = 0; i < input.size(); i++) {
        in[i][0] = input[i];
        in[i][1] = 0;
    }

    fftMutex.lock();
    p = fftw_plan_dft_1d(input.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftMutex.unlock();

    fftw_execute(p);

    std::vector<double[2]> outv(input.size());

    for (int i = 0; i < outv.size(); i++) {
        outv[i][0] = out[i][0];
        outv[i][1] = out[i][1];
    }

    fftMutex.lock();
    fftw_destroy_plan(p);
    fftMutex.unlock();

    fftw_free(in);
    fftw_free(out);

    return outv;
}
