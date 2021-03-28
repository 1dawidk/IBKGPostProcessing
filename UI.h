//
// Created by Dawid Kulpa on 13.03.2021.
//

#ifndef IBKGPOSTPROCESSING_UI_H
#define IBKGPOSTPROCESSING_UI_H

#include "iostream"
#include "DSP.h"

using namespace std;

class UI {
public:
    void start();
    void mainLoop();

    bool getUserInput(double &in, double def=0.0, double bMin=DBL_MIN, double bMax=DBL_MAX);
    bool getUserInput(int &in, int def=0, int bMin=INT_MIN, int bMax=INT_MAX);
    bool getUserInput(string &in, const string &def="");

    static void progressBarShow(double progress, int size);

private:
    DSP dsp;
    int action;
    bool sigRed;

    bool runningScript;
};


#endif //IBKGPOSTPROCESSING_UI_H
