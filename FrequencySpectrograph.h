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
    void createGraph(double *d, int n, int wn, int ws, string filename, int fs);
    double *getFrequencyResult(fftw_complex *r) const;
private:
    void appendOutToResult(double *o);

    int resultLen;
    cv::Mat *result;
    int lineCnt;
};


#endif //AEKGPOSTPROCESS_FREQUENCYSPECTROGRAPH_H
