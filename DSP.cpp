//
// Created by Dawid Kulpa on 11.03.2021.
//

#include "DSP.h"
#include <sys/stat.h>
#include "UI.h"

int DSP::loadDataFromCSV(const string &filename, int timeAt, int amplitudeAt) {
    UI::progressBarShow(0.0, 50);
    struct stat fileStat;
    stat(filename.c_str(), &fileStat);

    ifstream logFile(filename);
    if(!logFile.is_open()){
        return -1;
    }

    int minLineVals= timeAt<amplitudeAt ? amplitudeAt : timeAt;
    string line;
    long tshift=0;
    size_t red=0;

    while(getline(logFile, line)){
        vector<string> parts= explode(line, ',');
        if(parts.size()>=minLineVals) {
            sigT.push_back(stol(parts[timeAt]));
            sigA.push_back(stod(parts[amplitudeAt]));

            if (sigT.size() == 1) {
                tshift = sigT[0];
            }
            sigT[sigT.size() - 1] -= tshift;
        }

        red+= line.size();
        UI::progressBarShow((double)red/fileStat.st_size, 50);
    }

    UI::progressBarShow(1.0, 50);
    cout << endl;

    return sigT.size();
}

#define IMG_H 1024

void DSP::plot(const string &filename, uint imgWidth, double plim, double nlim, uint start, uint len, bool drawGrid) {
    UI::progressBarShow(0.0, 50);

    int imgsN= ceil((double)len / imgWidth);

    int readStart=0;
    if(plim==0 && nlim==0){
        findMinMax(&sigA[start], len, &nlim, &plim);
    }

    for(int i=0; i<imgsN; i++){
        cv::Mat *img= new cv::Mat(IMG_H, imgWidth, CV_8UC3, cv::Scalar(242, 248, 250));
        double a= (double)(IMG_H-1)/(nlim-plim);
        double b= -plim*a;
        int ypix;

        DSP::plotDrawAxis(img, DSP_PLOT_AXIS_X, b);
        if(drawGrid)
            DSP::plotDrawGrid(img, b, 1000/getSampleRate(), sigT[readStart]);

        UI::progressBarShow((double)i/imgsN, 50);

        int readLen= (len-readStart)<imgWidth ? len-readStart : imgWidth;

        for(int j=0; j<readLen; j++){
            if(j==0){
                ypix= a*sigA[readStart+j]+b;
                img->at<cv::Vec3b>(ypix, j)= cv::Vec3b(0, 68, 255);
            } else {
                cv::line(*img,
                         cv::Point(j-1,a*sigA[readStart+(j-1)]+b),
                         cv::Point(j,a*sigA[readStart+j]+b),
                         cv::Scalar(0, 68, 255));
            }
        }

        readStart+= imgWidth;

        cv::imwrite(filename+"_"+to_string(i)+".png", *img);
        delete img;
    }

    UI::progressBarShow(1.0, 50);
    cout << endl;
}

void DSP::applyEMAFilter(double alpha, int windowN) {
    EMAFilter(&sigA[0], sigA.size(), windowN, alpha);
}

uint DSP::getT(int at) {
    if(at<sigT.size())
        return sigT[at];

    return 0;
}

uint DSP::getA(int at) {
    if(at<sigA.size())
        return sigA[at];

    return 0;
}

double DSP::getSampleRate() {
    return 1000.0/(sigT[1]-sigT[0]);
}


double DSP::EMA(const double *d, int n, double alpha) {
    double ema= d[n-1];

    if(n>1) {
        for (int i = n - 2; i >= 0; i--) {
            ema = alpha * d[i] + (1 - alpha) * ema;
        }
    }

    return ema;
}

void DSP::EMAFilter(double *d, int size, int n, double alpha) {
    int N;

    for(int i=0; i<size; i++){
        if((i+1)<n)
            N= (i+1);
        else
            N= n;

        d[i]= EMA(&d[i], N, alpha);
    }
}

std::vector<std::string> DSP::explode(const string &s, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); ) {
        if (!token.empty())
            result.push_back(std::move(token));
    }

    return result;
}

double DSP::findMax(double *d, int n) {
    double max= d[0];

    for(int i=1; i<n; i++){
        if(d[i]>max)
            max= d[i];
    }

    return max;
}

double DSP::findMin(double *d, int n) {
    double min= d[0];

    for(int i=1; i<n; i++){
        if(d[i]<min)
            min= d[i];
    }

    return min;
}

void DSP::findMinMax(double *d, int n, double *min, double *max) {
    *max= d[0];
    *min= d[0];

    for(int i=1; i<n; i++){
        if(d[i]>*max)
            *max= d[i];

        if(d[i]<*min)
            *min= d[i];
    }
}

void DSP::plotDrawAxis(cv::Mat *img, int axis, int yzero) {
    if(axis==DSP_PLOT_AXIS_X){
        cv::line(*img,
                 cv::Point(0,yzero),
                 cv::Point(img->cols-1, yzero),
                 cv::Scalar(0,0,0),
                 3);
    } else {
        cv::line(*img,
                 cv::Point(0,0),
                 cv::Point(0, img->rows-1),
                 cv::Scalar(0,0,0),
                 3);
    }
}

void DSP::plotDrawGrid(cv::Mat *img, int yzero, int msppx, int tstart) {
    int lastTDraw;
    int at=100;

    cv::Scalar lineColor= CV_RGB(180, 180, 180);
    cv::Scalar textColor= CV_RGB(0, 0, 0);

    //Write zero time
    char buf[10];
    sprintf(buf, "%02d:%02d.%d", 0, tstart/1000, (tstart%1000)/100);
    cv::putText(*img, buf, cv::Point(2, yzero-5),
                cv::FONT_HERSHEY_COMPLEX_SMALL,
                0.9,
                textColor);
    lastTDraw= 0;

    //Draw vertical lines and time texts
    while(at<img->cols) {
        cv::line(*img,
                 cv::Point(at, 0),
                 cv::Point(at, img->rows - 1),
                 lineColor,
                 1);

        if(at >= lastTDraw+300){
            char buf[10];
            sprintf(buf, "%02d:%02d.%d", 0, (at*msppx+tstart)/1000, ((at*msppx+tstart)%1000)/100);
            cv::putText(*img, buf, cv::Point(at+2, yzero-5),
                        cv::FONT_HERSHEY_COMPLEX_SMALL,
                        0.9,
                        textColor);
            lastTDraw= at;
        }
        at+=100;
    }

    //Draw positive horizontal lines
    at= yzero+100;
    while(at<img->rows) {
        cv::line(*img,
                 cv::Point(0,at),
                 cv::Point(img->cols-1, at),
                 lineColor,
                 1);
        at+=100;
    }

    //Draw negative horizontal lines
    at= yzero-100;
    while(at>=0) {
        cv::line(*img,
                 cv::Point(0,at),
                 cv::Point(img->cols-1, at),
                 lineColor,
                 1);
        at-=100;
    }
}
