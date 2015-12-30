#include "Cutflow/METAnalysis.h"
#include <iostream>
#include "TLorentzVector.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"

using namespace std;


METAnalysis::METAnalysis(){


}

void METAnalysis::METfromPFO(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs) {
     chMET = 0;
     neuMET = 0;
	TLorentzVector chFourMom = TLorentzVector();
	TLorentzVector neuFourMom = TLorentzVector();

	for (unsigned int i = 0; i < chPFOs->size(); i++) {
		chFourMom += chPFOs->at(i)->p4();
		chMET += chPFOs->at(i)->pt();
	}

	for (unsigned int i = 0; i < neuPFOs->size(); i++) {
		neuFourMom += neuPFOs->at(i)->p4();
		neuMET += neuPFOs->at(i)->pt();
	}

    cout << "Missing ET from PFO without wavelets = " << endl;
    cout << "                                 neu = " << neuMET << endl;
    cout << "                                  ch = " << chMET << endl;

}

void METAnalysis::METfromPFOwWavelets(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs) {
     chwlMET = 0;
     neuwlMET = 0;
    for (unsigned int i = 0; i < chPFOs->size(); i++) {
        chPFO_vec.push_back(TLorentzVector());
        chPFO_vec.back().SetPtEtaPhiM(chPFOs->at(i)->pt(), chPFOs->at(i)->eta(), chPFOs->at(i)->phi(), chPFOs->at(i)->m());
    }

    for (unsigned int i = 0; i < neuPFOs->size(); i++) {
        neuPFO_vec.push_back(TLorentzVector());
        neuPFO_vec.back().SetPtEtaPhiM(neuPFOs->at(i)->pt(), neuPFOs->at(i)->eta(), neuPFOs->at(i)->phi(), neuPFOs->at(i)->m());
    }

	int nPixel =  64;
    double yRange = 3.2;
    NewWave::PixelDefinition* _pixelDefinition = new NewWave::PixelDefinition(nPixel, yRange);
    NewWave::GSLEngine* _waveletEngine = new NewWave::GSLEngine(gsl_wavelet_haar, 2, *_pixelDefinition);

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowNeu(neuPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowNeu.denoise(1.);
    neuPFO_vec_new = wePFlowNeu.particles();

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowCh(chPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowCh.denoise(1.);
    chPFO_vec_new = wePFlowCh.particles();

    
    for (unsigned int j = 0; j < neuPFO_vec_new.size(); j++) {
        neuwlMET += neuPFO_vec_new.at(j).Pt();
    }
    for (unsigned int j = 0; j < chPFO_vec_new.size(); j++) {
        chwlMET += chPFO_vec_new.at(j).Pt();
    }

    cout << "Missing ET from PFO with wavelets = " << endl;
    cout << "                              neu = " << neuwlMET << endl;
    cout << "                               ch = " << chwlMET << endl;

}

void METAnalysis::METfromxAOD() {

}

void METAnalysis::METfromParticleContainers() {

}


double METAnalysis::getNeuPFOwlMET() {
    return neuwlMET;
}

double METAnalysis::getChPFOwlMET() {
    return chwlMET;
} 

double METAnalysis::getNeuPFOMET() {
    return neuMET;
}

double METAnalysis::getChPFOMET() {
    return chMET;
}
