#ifndef theSearch_CutsInxAOD_H
#define theSearch_CutsInxAOD_H
#include "TObject.h"
#include "TLorentzVector.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include <vector>
#include "xAODJet/JetContainer.h"
#include "JetSelectorTools/JetCleaningTool.h"

using namespace std;
class CutsInxAOD  { // : public TObject

private:
    vector<TLorentzVector> mu_vector_neg; //!
    vector<TLorentzVector> mu_vector_pos; //!
    vector<TLorentzVector> e_vector_neg; //!
    vector<TLorentzVector> e_vector_pos; //!
    vector<TLorentzVector> jet_vector; //!

    vector<TLorentzVector> Z_from_jets; //!
    vector<TLorentzVector> Z_from_muons; //!
    vector<TLorentzVector> Z_from_electrons; //!

public:

    // constructor
    CutsInxAOD();

    // functions
    void analyzeMuons(const xAOD::MuonContainer* muons);
    void analyzeJets(const xAOD::JetContainer* jets,JetCleaningTool *m_jetCleaning);

    void analyzeElectrons(const xAOD::ElectronContainer* electrons);
    void analyzeZbosonsFromElectrons(const xAOD::ElectronContainer* electrons);
    void analyzeZbosonsFromMuons(const xAOD::MuonContainer* muons);
    void analyzeZbosonsFromJets(const xAOD::JetContainer* jets,JetCleaningTool *m_jetCleaning);

    void printZbosonsFromElectrons();
    void printZbosonsFromMuons();

    // vectors
    vector<TLorentzVector> getMuonNeg();
    vector<TLorentzVector> getMuonPos();
    vector<TLorentzVector> getElectronNeg();
    vector<TLorentzVector> getElectronPos();
    vector<TLorentzVector> getJets();

    // variables
    int muon_counter; //!
    double muon_count; //!
    double muon_pt; //!
    double muon_eta; //!
    double muon_phi; //!
    double muon_charge;//!

    vector<TLorentzVector>  getZbosonsFromElectrons();
    vector<TLorentzVector>  getZbosonsFromMuons();
    vector<TLorentzVector>  getZbosonsFromJets();

};

#endif