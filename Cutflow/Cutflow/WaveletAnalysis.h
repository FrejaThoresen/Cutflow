#ifndef Cutflow_WaveletAnalysis_H
#define Cutflow_WaveletAnalysis_H
#include "TObject.h"
#include "TLorentzVector.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include <vector>
#include "xAODJet/JetContainer.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"

using namespace std;
class WaveletAnalysis  { // : public TObject

    private:
        NewWave::PixelDefinition* _pixelDefinition; //!
        NewWave::GSLEngine* _waveletEngine; //!
        NewWave::GSLEngine* _waveletEngine1; //!
        vector<TLorentzVector> chPFOwave; //!
        vector<TLorentzVector> neuPFOwave; //!
        double pileup; //!
    public:
        // constructor
        WaveletAnalysis(int nPixel, double yRange, double N_pileup);

        vector<TLorentzVector> getChPFOwave();
        vector<TLorentzVector> getNeuPFOwave();

        void analyzeNeuPFO(vector<TLorentzVector> neuPFO);
        void analyzeChPFO(vector<TLorentzVector> chPFO);
};

#endif