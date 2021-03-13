#include <iostream>
#include <fstream>
#include <vector>
#include "FrequencySpectrograph.h"
#include "DSP.h"
#include "UI.h"

#define MAX_DATA_SIZE   1000000

using namespace std;

int main() {
    UI ui;
    FrequencySpectrograph fs{};
    int dataSize=0;
    vector<long> t;
    vector<double> y;

    ui.start();

    ui.mainLoop();

    /*cout << "Reading log file..." << endl;
    ifstream logFile("/home/dkulpa/AEKG.log");
    if(!logFile.is_open()){
        return 1;
    }

    string line;
    long tshift=0;
    while(getline(logFile, line)){
        size_t tend= line.find(',');
        size_t xend= line.find(',', tend+1);
        size_t yend= line.find(',', xend+1);

        t.push_back(stol(line.substr(0, tend)));
        y.push_back( stod(line.substr(yend+1)) );

        if(t.size()==1){
            tshift= t[0];
        }
        t[t.size()-1]-= tshift;
    }

    cout << "Applying EMA filter..." << endl;
    double *o= DSP::EMAFilter(&y[0], t.size(), 15, 0.4);

    cout << "Drawing raw data image" << endl;
    fs.createRaw(o+16384, 4096, "Filtered");
    fs.createRaw(&y[16384], 4096, "Raw");

    cout << "Red " << t.size() << " samples" << endl;

    cout << "Applying low pass filter..." << endl;

    cout << "Applying high pass filter..." << endl;

    cout << "Removeing noise..." << endl;


    cout << "Generating spectrograph..." << endl;
    fs.createGraph(&y[0] ,t.size(), 1024, 256, "spec2", 500);*/

    return 0;
}
