#ifndef theSearch_CutsInxAOD_H
#define theSearch_CutsInxAOD_H
#include "TObject.h"
#include "TLorentzVector.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include <vector>
#include "xAODJet/JetContainer.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "xAODPFlow/PFOContainer.h"

using namespace std;
class METAnalysis  { // : public TObject

private:
    vector<TLorentzVector> mu_vector_neg; //!
    vector<TLorentzVector> mu_vector_pos; //!
    vector<TLorentzVector> e_vector_neg; //!
    vector<TLorentzVector> e_vector_pos; //!
    vector<TLorentzVector> jet_vector; //!

    vector<TLorentzVector> Z_from_jets; //!
    vector<TLorentzVector> Z_from_muons; //!
    vector<TLorentzVector> Z_from_electrons; //!

    vector<TLorentzVector> chPFO; //!
    vector<TLorentzVector> neuPFO; //!


public:

    // constructor
    METAnalysis();

    // functions
    void METfromPFO(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs);
    void METfromPFOwWavelets(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs);
    void METfromxAOD();
    void METfromParticleContainers();

};

#endif