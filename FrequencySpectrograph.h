//
// Created by Dawid Kulpa on 11.03.2021.
//

#ifndef AEKGPOSTPROCESS_FREQUENCYSPECTROGRAPH_H
#define AEKGPOSTPROCESS_FREQUENCYSPECTROGRAPH_H

#include <cstring>
#include <fftw3.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <zconf.h>
#include <iostream>

using namespace std;

class FrequencySpectrograph {
public:
    void createGraph(double *d, int n, int windowWidth, int windowStep, const string &filename, int sampleRate, int imgHeight);
    double *getFrequencyResponse(fftw_complex *r) const;

    static fftw_complex *FFT(double *sig, uint n, uint sr);
    static double *IFFT(fftw_complex *freq, uint n, uint sr);
private:
    void appendOutToResult(double *o, int at);
    void putTimeMarks(cv::Mat &img, double msppx, double vppx);

    int resultLen;
    cv::Mat *result;
    int lineCnt;
};


#endif //AEKGPOSTPROCESS_FREQUENCYSPECTROGRAPH_H
