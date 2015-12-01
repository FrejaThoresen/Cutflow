#include "Cutflow/CutsInxAOD.h"
#include <iostream>

using namespace std;
//ClassImp(CutsInxAOD)

CutsInxAOD::CutsInxAOD(){

}

vector<TLorentzVector> CutsInxAOD ::  getElectronNeg(){
    return e_vector_neg;
}

vector<TLorentzVector> CutsInxAOD ::  getElectronPos(){
    return e_vector_pos;
}

vector<TLorentzVector> CutsInxAOD ::  getJets(){
    return jet_vector;
}

vector<TLorentzVector> CutsInxAOD ::  getZbosonsFromElectrons(){
    return Z_from_electrons;
}

vector<TLorentzVector> CutsInxAOD ::  getZbosonsFromJets(){
    return Z_from_jets;
}

vector<TLorentzVector> CutsInxAOD ::  getZbosonsFromMuons(){
    return Z_from_muons;
}

vector<TLorentzVector> CutsInxAOD ::  getMuonNeg(){
    return mu_vector_neg;
}

vector<TLorentzVector> CutsInxAOD ::  getMuonPos(){
    return mu_vector_pos;
}


void CutsInxAOD :: analyzeJets(const xAOD::JetContainer* jets,JetCleaningTool *m_jetCleaning) {

    // loop over the jets in the container
    xAOD::JetContainer::const_iterator jet_itr = jets->begin();
    xAOD::JetContainer::const_iterator jet_end = jets->end();
    double jet_pt;
    double jet_ptcone;
    double jet_iso;

    for( ; jet_itr != jet_end; ++jet_itr ) {
        jet_pt = (*jet_itr)->pt();
        if( !m_jetCleaning->accept( **jet_itr )) continue; //only keep good clean jets
        //if (jet_pt > 10000.0) continue;

        jet_vector.push_back(TLorentzVector());
        jet_vector.back().SetPtEtaPhiM(jet_pt, (*jet_itr)->eta(), (*jet_itr)->phi(), (*jet_itr)->m());

        //cout << "jet_pt = " << jet_pt << ", jet_eta = " << (*jet_itr)->eta() << ", jet_phi = " << (*jet_itr)->phi() << ", jet_m = " << (*jet_itr)->m() << endl;
        //cout << "jet_pt = " << jet_vector.back().Pt() << ", jet_eta = " << jet_vector.back().Eta() << ", jet_phi = " << jet_vector.back().Phi() << ", jet_m = " << jet_vector.back().M() << endl;

    } // end for loop over jets
}


void CutsInxAOD :: analyzeMuons(const xAOD::MuonContainer* muons) {

    // loop over the muons in the container
    xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
    xAOD::MuonContainer::const_iterator muon_end = muons->end();

    muon_counter = 0;

    float ptcone;

    for(; muon_itr != muon_end; ++muon_itr) {
        double muon_pt = (*muon_itr)->pt();
        ptcone = (*muon_itr)->auxdata<float>("ptcone20");
        double iso = ptcone / muon_pt;

        if (iso < 0.1 && muon_pt > 10000.0) {
            double muon_eta = (*muon_itr)->eta();
            double muon_phi = (*muon_itr)->phi();
            double muon_charge = (*muon_itr)->charge();
            if (muon_charge > 0) {
                mu_vector_pos.push_back(TLorentzVector());
                mu_vector_pos.back().SetPtEtaPhiM(muon_pt, muon_eta, muon_phi, 105.6);
            }
            else {
                mu_vector_neg.push_back(TLorentzVector());
                mu_vector_neg.back().SetPtEtaPhiM(muon_pt, muon_eta, muon_phi, 105.6);
            }
        }
    }
}

void CutsInxAOD :: analyzeElectrons(const xAOD::ElectronContainer* electrons) {
// loop over the muons in the container
    xAOD::ElectronContainer::const_iterator e_itr = electrons->begin();
    xAOD::ElectronContainer::const_iterator e_end = electrons->end();

    for (; e_itr != e_end; ++e_itr) {
        double e_pt = (*e_itr)->pt();
        double ptcone = (*e_itr)->auxdata<float>("ptcone20");
        double iso = ptcone / e_pt;

        if (iso < 0.1 && e_pt > 10000.0) {
            double e_eta = (*e_itr)->eta();
            double e_phi = (*e_itr)->phi();
            double e_charge = (*e_itr)->charge();
            if (e_charge > 0) {
                e_vector_pos.push_back(TLorentzVector());
                e_vector_pos.back().SetPtEtaPhiM(e_pt, e_eta, e_phi, 0.511);
            }
            else {
                e_vector_neg.push_back(TLorentzVector());
                e_vector_neg.back().SetPtEtaPhiM(e_pt, e_eta, e_phi, 0.511);
            }
        }
    }

}

void CutsInxAOD :: analyzeZbosonsFromElectrons(const xAOD::ElectronContainer* electrons) {
    analyzeElectrons(electrons);

    if (e_vector_neg.size() != 0 && e_vector_pos.size() != 0) {
        for (unsigned int i = 0; i < e_vector_pos.size(); i++) {
            for (unsigned int j = 0; j < e_vector_neg.size(); j++) {
                if ((e_vector_pos[i] + e_vector_neg[j]).M() > 400000) {

                    Z_from_electrons.push_back(e_vector_pos[i] + e_vector_neg[j]);

                }
            }
        }
    }

}


void CutsInxAOD :: analyzeZbosonsFromJets(const xAOD::JetContainer* jets,JetCleaningTool *m_jetCleaning) {
    analyzeJets(jets,m_jetCleaning);

    if (jet_vector.size() != 0) {
        for (unsigned int i = 0; i < jet_vector.size() - 1; i = i + 2) {
            if ((jet_vector[i] + jet_vector[i+1]).M() > 400000) {
                Z_from_jets.push_back(jet_vector[i] + jet_vector[i + 1]);
            }
        }
    }

}


void CutsInxAOD :: analyzeZbosonsFromMuons(const xAOD::MuonContainer* muons) {
    analyzeMuons(muons);

    if (mu_vector_neg.size() != 0 && mu_vector_pos.size() != 0) {
        for (unsigned int i = 0; i < mu_vector_pos.size(); i++) {
            for (unsigned int j = 0; j < mu_vector_neg.size(); j++) {
                if ((mu_vector_pos[i] + mu_vector_neg[j]).M() > 400000) {
                    Z_from_muons.push_back(mu_vector_pos[i] + mu_vector_neg[j]);
                }
            }
        }
    }
}

void CutsInxAOD :: printZbosonsFromElectrons() {
    for (unsigned int i = 0; i < Z_from_electrons.size(); i++) {
        Info("execute()", "  Z boson from electrons      pt = %.2f GeV   eta = %.3f   phi = %.3f    m = %.1f",
             Z_from_electrons[i].Pt() * 0.001,
             Z_from_electrons[i].Eta(),
             Z_from_electrons[i].Phi(),
             Z_from_electrons[i].M());

        Info("execute()", "       1st electron pt = %.2f GeV   eta = %.3f   phi = %.3f",
             e_vector_pos[i].Pt() * 0.001,
             e_vector_pos[i].Eta(),
             e_vector_pos[i].Phi());
        Info("execute()", "       2nd electron pt = %.2f GeV   eta = %.3f   phi = %.3f",
             e_vector_neg[i].Pt() * 0.001,
             e_vector_neg[i].Eta(),
             e_vector_neg[i].Phi());
        cout <<
        "-----------------------------------------------------------------------------------------------------------------------" <<
        endl;
    }
}

void CutsInxAOD :: printZbosonsFromMuons() {
    for (unsigned int i = 0; i < Z_from_muons.size(); i++) {
        Info("execute()", "  Z boson from muons      pt = %.2f GeV   eta = %.3f   phi = %.3f    m = %.1f",
             Z_from_muons[i].Pt() * 0.001,
             Z_from_muons[i].Eta(),
             Z_from_muons[i].Phi(),
             Z_from_muons[i].M());

        Info("execute()", "       1st muon pt = %.2f GeV   eta = %.3f   phi = %.3f",
             mu_vector_pos[i].Pt() * 0.001,
             mu_vector_pos[i].Eta(),
             mu_vector_pos[i].Phi());
        Info("execute()", "       2nd muon pt = %.2f GeV   eta = %.3f   phi = %.3f",
             mu_vector_neg[i].Pt() * 0.001,
             mu_vector_neg[i].Eta(),
             mu_vector_neg[i].Phi());
        cout <<
        "-----------------------------------------------------------------------------------------------------------------------" <<
        endl;

    }
}