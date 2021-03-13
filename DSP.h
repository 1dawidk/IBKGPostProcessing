//
// Created by Dawid Kulpa on 11.03.2021.
//

#ifndef AEKGPOSTPROCESS_DSP_H
#define AEKGPOSTPROCESS_DSP_H

#include <filesystem>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "FrequencySpectrograph.h"

#define DSP_PLOT_AXIS_X 0
#define DSP_PLOT_AXIS_Y 1
#define DSP_PLOT_LIMIT_FIT  0

using namespace std;

class DSP {
public:
    int loadDataFromCSV(const string &filename, int timeAt, int amplitudeAt);
    void plot(const string &filename, uint imgWidth=4096, double plim=0, double nlim=0, uint start=0, uint len=-1, bool drawGrid=false);
    void applyEMAFilter(double alpha, int windowN);

    uint getT(int at);
    uint getA(int at);
    double getSampleRate();

    static double EMA(const double *d, int n, double alpha);
    static void EMAFilter(double *d, int size, int n, double alpha);
private:
    vector<uint> sigT;
    vector<double> orgSigA;
    vector<double> sigA;

    FrequencySpectrograph fs;

    static std::vector<std::string> explode(std::string const & s, char delim);
    static double findMax(double *d, int n);
    static double findMin(double *d, int n);
    static void findMinMax(double *d, int n, double *min, double *max);
    static void plotDrawAxis(cv::Mat *img, int axis, int yzero=0);
    static void plotDrawGrid(cv::Mat *img, int yzero, int msppx, int tstart);
};


#endif //AEKGPOSTPROCESS_DSP_H
