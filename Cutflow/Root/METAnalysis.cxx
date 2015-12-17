#include "Cutflow/METAnalysis.h"
#include <iostream>
#include "TLorentzVector.h"
//#include "Event/FourMom_t.h"
using namespace std;


METAnalysis::METAnalysis(){

}

void METAnalysis::METfromPFO(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs) {

	double chMET = 0;
	TLorentzVector chFourMom = TLorentzVector();
	double neuMET = 0;
	TLorentzVector neuFourMom = TLorentzVector();

	for (unsigned int i = 0; i < chPFOs->size(); i++) {
		fourMom += chPFOs->at(i)->p4();
		chMET += chPFOs->at(i)->pt();
	}

	for (unsigned int i = 0; i < neuPFOs->size(); i++) {
		fourMom += neuPFOs->at(i)->p4();
		chMET += neuPFOs->at(i)->pt();
	}
}

void METAnalysis::METfromPFOwWavelets(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs) {

}

void METAnalysis::METfromxAOD() {

}

void METAnalysis::METfromParticleContainers() {

}

