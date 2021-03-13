//
// Created by Dawid Kulpa on 11.03.2021.
//

#include "FrequencySpectrograph.h"

void FrequencySpectrograph::createGraph(double *d, int n, int wn, int ws, string filename, int fs) {
    double *in;
    fftw_complex *out;
    fftw_plan p;

    result= new cv::Mat((n-wn)/ws, wn/2, CV_8UC1, cv::Scalar(0));
    lineCnt=0;

    in = (double*) fftw_malloc(sizeof(double) * wn);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * wn);
    resultLen= wn;
    p = fftw_plan_dft_r2c_1d(wn, in, out, FFTW_ESTIMATE);

    int linesN= (n-wn)/ws;
    int start= 0;

    for(int i=0; i<linesN; i++) {
        memcpy(in, d+start, wn * sizeof(double));

        fftw_execute(p);
        appendOutToResult(getFrequencyResult(out));

        cout << "\r[";
        int fill= 40*((double)lineCnt/linesN);
        for(int j=0; j<40; j++){
            if(j<=fill)
                cout << "#";
            else
                cout << " ";
        }
        cout << "]"<< flush;
        start+= ws;
    }

    cv::Mat graph;
    cv::applyColorMap(*result, graph, cv::COLORMAP_HOT);

    double ts= 1.0/fs;

    cv::imwrite(filename+"_"+to_string((double)fs/wn)+"_"+to_string(ts*ws)+".png", graph);

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

double *FrequencySpectrograph::getFrequencyResult(fftw_complex *r) const {
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


void FrequencySpectrograph::appendOutToResult(double *o) {
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
        result->at<uchar>(lineCnt, i)= o[i]*254;
    }

    lineCnt++;
    delete[] o;
}
