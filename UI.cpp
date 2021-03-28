//
// Created by Dawid Kulpa on 13.03.2021.
//

#include "UI.h"

void UI::start() {
    cout << "#-------------------------------------------------#" << endl;
    cout << "#              IMU Based Cardiograph              #" << endl;
    cout << "#                      2021                       #" << endl;
    cout << "#                                     Dawid Kulpa #" << endl;
    cout << "#-------------------------------------------------#" << endl;
    action= 0;
    sigRed= false;
    runningScript= false;
}

void UI::mainLoop() {
    while(action!=9) {
        if(action==0) {
            cout << "Choose action:" << endl;
            cout << "1 - Load data" << endl;
            if(sigRed) {
                cout << "2 - Draw raw graph" << endl;
                cout << "3 - Draw frequency graph" << endl;
                cout << "4 - Apply filter" << endl;
                cout << "5 - Get heart state" << endl;
                cout << "6 - Integral" << endl;
                cout << "7 - Get max-min" << endl;
                cout << "8 - Test IFFT" << endl;
            }
            cout << "9 - Exit" << endl;
            if(!getUserInput(action, 0, 1, 8))
                action=0;

            if(!sigRed && (action!=1 && action!=9))
                action= 0;
            cout << endl << endl;
        } else if(action==1){
            string filepath;
            cout << "File path: ";
            getUserInput(filepath);

            int lines= dsp.loadDataFromCSV(filepath, 0, 1);
            if(lines<0) {
                cout << "Error while loading data!" << endl;
            } else {
                sigRed= true;
                cout << "Done! Red " << lines << " lines." << endl;
                cout << "Time: 0s - " << dsp.getSigLengthInSec() << "s" << endl << endl;
            }

            action= 0;
        } else if(action==2){
            double s;
            double l;
            int miw=0;
            string filename;
            cout << "Image name: ";
            getUserInput(filename, "plot");

            cout << "Start time [s, 0]: ";
            getUserInput(s, 0, 0, dsp.getSigLengthInSec());

            cout << "Length [s, " << dsp.getSigLengthInSec()-s << "]: ";
            getUserInput(l, dsp.getSigLengthInSec()-s, 1.0, dsp.getSigLengthInSec()-s);

            cout << "Enter max image width (A4: 1448): ";
            getUserInput(miw, 1448, 100, 4096);

            s= s*dsp.getSampleRate();
            l= l*dsp.getSampleRate();

            int imgW= (int)l<miw ? (int)l : miw;

            dsp.plot(filename, imgW, DSP_PLOT_LIMIT_FIT, DSP_PLOT_LIMIT_FIT, s, l, true);
            cout << "Done!" << endl << endl;
            action= 0;
        } else if(action==3){
            int wn;
            int ws;
            string filename;
            cout << "Graph name: ";
            cin >> filename;
            cout << "Window size: ";
            cin >> wn;
            cout << "Window step: ";
            cin >> ws;
            dsp.drawSpectrograph(filename, wn, ws);

            action= 0;
        } else if(action==4){
            int type;
            cout << "Choose typ: " << endl;
            cout << "1 - Exponential Moving Average" << endl;
            cout << "2 - Butterworth Filter" << endl;
            cout << "3 - Remove constant component" << endl;
            cout << "9 - Back" << endl;
            if(!getUserInput(type, 9, 1, 3)){
                action= 0;
            } else {
                action= 40+type;
            }
        } else if(action==6) {
            cout << "Integral..." << endl;
            dsp.integral();
            action= 0;
            cout << endl;
        } else if(action==7){
            cout << endl << "Max - Min = " << dsp.getMaxMinDifference() << endl << endl;
            action =0;
        } else if(action==8){
            fftw_complex* f= FrequencySpectrograph::FFT(&(*dsp.getSigA())[0], dsp.getSigA()->size(), 500);
            double *r= FrequencySpectrograph::IFFT(f, dsp.getSigA()->size(), 500);
            dsp.setSigA(r);

            fftw_free(f);
            fftw_free(r);
            action= 0;
        }else if(action==41){
                int windowN;
                cout << "Enter window size: ";
                cin >> windowN;
                dsp.applyEMAFilter(windowN);
                cout << "Done!" << endl << endl;
                action= 0;
        } else if(action==42){
            int type;
            int order;
            double f0;
            cout << "LPF - 0 / HPF - 1: ";
            cin >> type;
            cout << "Cut-off frequency [Hz]: ";
            cin >> f0;
            cout << "Order: ";
            cin >> order;
            if(type==0 || type==1) {
                dsp.applyButterworthFilter(order, f0, 1.0, type);
                action = 0;
            }
        } else if(action==43){
            dsp.removeConstantComponent(DSP_REMOVECC_INTEGRAL);
            action= 0;
        } else {
            action= 0;
        }
    }
}

void UI::progressBarShow(double progress, int size) {
    cout << "\r[";
    int fill= size*progress;
    for(int j=0; j<size; j++){
        if(j<=fill)
            cout << "#";
        else
            cout << " ";
    }
    cout << "]"<< flush;
}

bool UI::getUserInput(double &in, double def, double bMin, double bMax) {
    string s;
    getUserInput(s, to_string(def));

    stringstream ss(s);

    if(ss>>in && in<=bMax && in>=bMin)
        return true;

    return false;
}

bool UI::getUserInput(int &in, int def, int bMin, int bMax) {
    string s;
    getUserInput(s, to_string(def));

    stringstream ss(s);

    if(ss>>in && in<=bMax && in>=bMin)
        return true;

    return false;
}

bool UI::getUserInput(string &in, const string &def) {
    if(!runningScript){
        getline(cin, in);
    }

    if(in.empty())
        in= def;

    return true;
}
