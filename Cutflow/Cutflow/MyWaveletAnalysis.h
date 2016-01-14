#ifndef Cutflow_WaveletAnalysis_H
#define Cutflow_WaveletAnalysis_H
#include "TLorentzVector.h"
#include <vector>

using namespace std;
class MyWaveletAnalysis  { // : public TObject

    private:
        vector<TLorentzVector> chPFOwave; //!
        vector<TLorentzVector> neuPFOwave; //!
    public:
        // constructor
        MyWaveletAnalysis(int nPixel, double yRange);

        vector<TLorentzVector> getChPFOwave();
        vector<TLorentzVector> getNeuPFOwave();

        void analyzeNeuPFO(vector<TLorentzVector> neuPFO);
        void analyzeChPFO(vector<TLorentzVector> chPFO);
};

#endif