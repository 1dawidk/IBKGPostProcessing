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
}

void UI::mainLoop() {
    while(action!=9) {
        if(action==0) {
            cout << "Choose action:" << endl;
            cout << "1 - Load data" << endl;
            cout << "2 - Draw raw graph" << endl;
            cout << "3 - Draw frequency graph" << endl;
            cout << "4 - Apply filter" << endl;
            cout << "9 - Exit" << endl;
            cin >> action;
            cout << endl << endl;
        } else if(action==1){
            string filepath;
            cout << "File path: ";
            cin >> filepath;
            if(filepath.empty())
                filepath= "exam.log";

            int lines= dsp.loadDataFromCSV(filepath, 0, 1);
            if(lines<0) {
                cout << "Error while loading data!" << endl;
            } else {
                cout << "Done! Red " << lines << " lines." << endl;
                cout << "Time domain: 0s - " << (lines*(1000/dsp.getSampleRate()))/1000 << endl << endl;
            }

            action= 0;
        } else if(action==2){
            int s;
            int l;
            int miw=0;
            string filename;
            cout << "Image name: ";
            cin >> filename;
            if(filename.empty())
                filename= "graph";
            cout << "Start time [s]: ";
            cin  >> s;
            cout << "Length [s]: ";
            cin >> l;
            cout << "Enter max image width (A4: 1448): ";
            cin >> miw;
            if(miw==0)
                miw=1448;

            s= s*dsp.getSampleRate();
            l= l*dsp.getSampleRate();

            int imgW= l<miw ? l : miw;

            dsp.plot(filename, imgW, DSP_PLOT_LIMIT_FIT, DSP_PLOT_LIMIT_FIT, s, l, true);
            cout << "Done!" << endl << endl;
            action= 0;
        } else if(action==3){
            action= 0;
        } else if(action==4){
            int type;
            cout << "Choose typ: " << endl;
            cout << "1 - Exponential Moving Average" << endl;
            cout << "2 - Low Pass Filter" << endl;
            cout << "3 - High Pass Filter" << endl;
            cout << "9 - Back" << endl;
            cin >> type;

            if(type!=9){
                action= 40+type;
            } else {
                action= 0;
            }
        } else if(action==41){
            double alpha;
            int windowN;
            cout << "Enter multiplier (0-1): ";
            cin >> alpha;
            if(alpha<1.0 && alpha > 0.0){
                cout << "Enter window size: ";
                cin >> windowN;
                dsp.applyEMAFilter(alpha, windowN);
                cout << "Done!" << endl << endl;
                action= 0;
            }
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
