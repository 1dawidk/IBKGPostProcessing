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
        cout << "Max: " << plim << endl;
        cout << "Min: " << nlim << endl;
    }

    double a= (double)(IMG_H-1)/(nlim-plim);
    double b= -plim*a;

    for(int i=0; i<imgsN; i++){
        cv::Mat *img= new cv::Mat(IMG_H, imgWidth, CV_8UC3, cv::Scalar(242, 248, 250));
        int ypix;

        if(b<IMG_H)
            DSP::plotDrawAxis(img, DSP_PLOT_AXIS_X, b);
        if(drawGrid)
            DSP::plotDrawGrid(img, b, 1000/getSampleRate(), sigT[readStart], nlim, plim);

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

        cv::imwrite("imgs/"+filename+"_"+to_string(i)+".png", *img);
        delete img;
    }

    UI::progressBarShow(1.0, 50);
    cout << endl;
}

void DSP::applyEMAFilter(int windowN) {
    int N;

    UI::progressBarShow(0.0, 50);
    for(int i=1; i<sigA.size(); i++){
        double alpha= (2.0/(N+1));

        sigA[i]= (sigA[i] - sigA[i-1])*alpha + sigA[i-1];
        UI::progressBarShow((double)i/sigA.size(), 50);
    }

    UI::progressBarShow(1.0, 50);
    cout << endl;
}

void DSP::applyFIRFilter(double *ir, uint size) {
    UI::progressBarShow(0, 50);
    for(int j=sigA.size()-1; j>=size-1; j--) {
        uint s= j-size+1;
        double output = 0;

        for (int i = 0; i < size; i++) {
            output+= sigA[s+i]*ir[i];
        }

        sigA[s+size]= output;
        UI::progressBarShow((double)j/sigA.size(), 50);
    }

    UI::progressBarShow(1, 50);
}

void DSP::drawSpectrograph(const string &name, int wn, int ws) {
    UI::progressBarShow(0, 50);
    fs.createGraph(&sigA[0], sigA.size(), wn, ws, name, getSampleRate(), 1448);
    UI::progressBarShow(1, 50);
}

void DSP::integral() {
    double sum= 0;
    double changed= sigA[0];
    sigA[0]= 0;
    int t= 0;

    for(int i=1; i<sigA.size(); i++){
        sum+= (0.5*((sigT[i]-sigT[i-1]) * (sigA[i]+changed)));
        //t+= (sigT[i]-sigT[i-1]);
        //cout << t << "," << sigA[i] << "," << (0.5*((sigT[i]-sigT[i-1]) * (sigA[i]+changed))) << endl;

        changed= sigA[i];
        sigA[i]= sum;
        UI::progressBarShow((double)i/sigA.size(), 50);
    }

    UI::progressBarShow(1.0, 50);
}

double DSP::definiteIntegral(uint s, uint e, double C) {
    double sum= C;

    for(uint i=s+1; i<e; i++){
        sum+= (0.5*((sigT[i]-sigT[i-1]) * (sigA[i]+sigA[i-1])));
        UI::progressBarShow((double)i/sigA.size(), 50);
    }

    return sum;
}

double DSP::sum(uint s, uint e) {
    double sum= 0;

    for(int i=s; i<e; i++)
        sum+= sigA[i];

    return sum;
}

void DSP::removeConstantComponent(uint type, double e) {
    double avg=1;
    while(abs(avg)>e) {
        UI::progressBarShow(0.0, 50);
        double sum;
        sum= definiteIntegral(0, sigA.size(), 0.0);

        avg = sum / (sigA.size() * 2);

        for (int i = 0; i < sigA.size(); i++) {
            sigA[i] -= avg;
        }

        UI::progressBarShow(1.0, 50);
        cout << endl;
        cout << "Signal definiteIntegral <" << 0 << ", " << sigA.size() << "> : " << sum << endl;
        cout << "Signal const component: " << avg << endl << endl;
    }
}

void DSP::steppingRemoveConstantComponent(uint maxwn) {
    uint windowSize= sigA.size();
    uint partsNo=1;

    uint s, e;

    while(windowSize>maxwn){
        s= 0;
        e= windowSize;

        for(int i=0; i<partsNo; i++){
            double cc= sum(s, e)/windowSize;

            for(int j=s; j<e; j++){
                sigA[j]-= cc;
            }

            s= e;
            e+= windowSize;
        }

        partsNo*= 2;
        windowSize= sigA.size()/partsNo;
    }
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
    return 1000.0/(sigT[10]-sigT[9]);
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

void DSP::plotDrawGrid(cv::Mat *img, int yzero, int msppx, int tstart, double min, double max) {
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
    at= yzero-100;
    sprintf(buf, "%.10f", max);
    cv::putText(*img, buf, cv::Point(5, 15),
                cv::FONT_HERSHEY_COMPLEX_SMALL,
                0.9,
                textColor);
    while(at>=0) {
        cv::line(*img,
                 cv::Point(0,at),
                 cv::Point(img->cols-1, at),
                 lineColor,
                 1);
        at-=100;


    }

    //Draw negative horizontal lines
    at= yzero+100;
    sprintf(buf, "%.10f", min);
    cv::putText(*img, buf, cv::Point(5, img->rows-15),
                cv::FONT_HERSHEY_COMPLEX_SMALL,
                0.9,
                textColor);
    while(at<img->rows) {
        cv::line(*img,
                 cv::Point(0,at),
                 cv::Point(img->cols-1, at),
                 lineColor,
                 1);
        at+=100;
    }
}


double DSP::getMaxMinDifference() {
    double min;
    double max;

    findMinMax(&sigA[0], sigA.size(), &min, &max);

    return max-min;
}

vector<double> *DSP::getSigA(){
    return &sigA;
}

void DSP::setSigA(double *s) {
    for(int i=0; i<sigA.size(); i++){
        sigA[i]= s[i];
    }
}

void DSP::applyButterworthFilter(int order, double f0, double dcGain, uint type) {
    uint N= sigA.size();

    fftw_complex *freqOut= FrequencySpectrograph::FFT(&sigA[0], N, getSampleRate());

    double binWidth= getSampleRate()/N;
    double ratio;
    double binFreq;
    double gain;
    uint numBins= N/2;

    for(int i=1; i<numBins; i++) {
        binFreq = binWidth * i;

        if(type==DSP_HPF)
            ratio= f0/binFreq;
        else
            ratio= binFreq / f0;

        gain = dcGain / (sqrt((1 + pow(ratio, 2.0 * order))));
        freqOut[i][0] *= gain;
        freqOut[i][1] *= gain;
        freqOut[N - i][0] *= gain;
        freqOut[N - i][1] *= gain;
    }

    double *r= FrequencySpectrograph::IFFT(freqOut, N, getSampleRate());

    for(int i=0; i<N; i++){
        sigA[i]= r[i]/sigA.size();
    }

    fftw_free(freqOut);
    fftw_free(r);
}

int DSP::getSigLength() {
    return sigT.size();
}

double DSP::getSigLengthInSec() {
    return (sigT.size()*(1000.0/getSampleRate()))/1000.0;
}
