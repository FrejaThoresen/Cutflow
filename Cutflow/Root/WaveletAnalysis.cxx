#include "Cutflow/WaveletAnalysis.h"
#include <iostream>

#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"

using namespace std;
/*
* Tool for wavelet analysis
*/

WaveletAnalysis::WaveletAnalysis(int nPixel, double yRange){
    _pixelDefinition = new NewWave::PixelDefinition(nPixel, yRange);
    _waveletEngine = new NewWave::GSLEngine(gsl_wavelet_haar, 2, *_pixelDefinition);
    _waveletEngine1 = new NewWave::GSLEngine(gsl_wavelet_haar, 2, *_pixelDefinition);

}

vector<TLorentzVector> WaveletAnalysis ::  getChPFOwave(){
    return chPFOwave;
}

vector<TLorentzVector> WaveletAnalysis ::  getNeuPFOwave(){
    return neuPFOwave;
}


void WaveletAnalysis :: analyzeNeuPFO(vector<TLorentzVector> neuPFO) {

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowNeu(neuPFO, *_pixelDefinition, *_waveletEngine);
    wePFlowNeu.denoise(1.);
    neuPFOwave= wePFlowNeu.particles();
}


void WaveletAnalysis :: analyzeChPFO(vector<TLorentzVector> chPFO) {

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowCh(chPFO, *_pixelDefinition, *_waveletEngine1);
    wePFlowCh.denoise(1.);
    chPFOwave = wePFlowCh.particles();
}
