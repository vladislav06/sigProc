//
// Created by vm on 24.27.11.
//
#pragma once


#include <mutex>
#include <vector>

/**
 * Thread safe fftw library wrapper
 */
class Fft {
private:
    static inline std::mutex fftMutex{};
public:
    /**
     * Calculate real and imaginary part of a signal
     * @param input input data to be decomposed
     * @return [real, imagenary]
     */
    static std::vector<double[2]> calculateFft(std::vector<double> &input);

};


