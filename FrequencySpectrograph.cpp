//
// Created by Dawid Kulpa on 11.03.2021.
//

#include "FrequencySpectrograph.h"
#include "UI.h"

#define MARGIN_LEFT_PX  50
#define MARGIN_TOP_PX   50

void FrequencySpectrograph::createGraph(double *d, int n, int windowWidth, int windowStep, const string &filename, int sampleRate, int imgHeight) {
    double *in= (double*) fftw_malloc(sizeof(double) * windowWidth);;
    fftw_complex *out= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * windowWidth);;
    fftw_plan p= fftw_plan_dft_r2c_1d(windowWidth, in, out, FFTW_ESTIMATE);
    resultLen= windowWidth;

    int linesN= (n - windowWidth) / windowStep + 1;
    int imgsN= floor((double)linesN/imgHeight);

    result= new cv::Mat(imgHeight, windowWidth / 2, CV_8UC1, cv::Scalar(0));
    cv::Mat plot(result->rows+MARGIN_LEFT_PX, result->cols+MARGIN_TOP_PX, CV_8UC3, cv::Scalar(255, 255, 250));
    for(int i=0; i<imgsN; i++){

        for(int j=0; j<imgHeight; j++) {
            memcpy(in, d+(i*imgHeight*windowStep)+(j*windowStep), windowWidth * sizeof(double));

            fftw_execute(p);
            appendOutToResult(getFrequencyResponse(out), j);
        }

        cv::Mat spectrograph;
        cv::applyColorMap(*result, spectrograph, cv::COLORMAP_HOT);
        spectrograph.copyTo(plot(cv::Rect(MARGIN_LEFT_PX, MARGIN_TOP_PX, spectrograph.cols, spectrograph.rows)));

        cv::imwrite("imgs/"+filename+"_"+to_string(i)+".png", plot);

        UI::progressBarShow(((double)imgsN/i), 50);

        *result= cv::Scalar(0);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

double *FrequencySpectrograph::getFrequencyResponse(fftw_complex *r) const {
    double *res= new double[resultLen/2];
    double factor= 1.0/resultLen;

    for(int i=0; i<resultLen/2; i++){
        res[i]= r[i+1][0]*r[i+1][0] + r[i+1][1]*r[i+1][1];
        res[i]= sqrt(res[i]);

        if(i==0){
            res[i]*= factor;
        } else {
            res[i]*= 2*factor;
        }
    }

    return res;
}

fftw_complex *FrequencySpectrograph::FFT(double *sig, uint n, uint sr) {
    fftw_complex *out= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);;
    fftw_plan p= fftw_plan_dft_r2c_1d(n, sig, out, FFTW_ESTIMATE);
    fftw_execute(p);

    fftw_destroy_plan(p);

    return out;
}

double *FrequencySpectrograph::IFFT(fftw_complex *freq, uint n, uint sr) {
    double *out= (double*) fftw_malloc(sizeof(double) * n);
    fftw_plan p= fftw_plan_dft_c2r_1d(n, freq, out, FFTW_ESTIMATE);
    fftw_execute(p);

    fftw_destroy_plan(p);
    return out;
}


void FrequencySpectrograph::appendOutToResult(double *o, int at) {
    //Find max for normalization
    double max=o[0];

    for(int i=1; i<result->cols; i++){
        if(o[i]>max){
            max= o[i];
        }
    }

    //Normalize and supply image
    for(int i=0; i<result->cols; i++){
        o[i]/=max;
        result->at<uchar>(at, i)= o[i]*254;
    }

    lineCnt++;
    delete[] o;
}

void FrequencySpectrograph::putTimeMarks(cv::Mat &img, double msppx, double vppx) {

}
