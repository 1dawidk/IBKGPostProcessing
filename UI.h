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

    static void progressBarShow(double progress, int size);

private:
    DSP dsp;
    int action;
};


#endif //IBKGPOSTPROCESSING_UI_H
