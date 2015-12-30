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
    vector<TLorentzVector> chPFO_vec; //!
    vector<TLorentzVector> chPFO_vec_new; //! 
    vector<TLorentzVector> neuPFO_vec; //!
    vector<TLorentzVector> neuPFO_vec_new; //! 

    double chMET; //!
    double neuMET; //!
    double chwlMET; //!
    double neuwlMET; //!

public:
    // constructor
    METAnalysis();

    // functions
    void METfromPFO(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs);
    void METfromPFOwWavelets(const xAOD::PFOContainer* chPFOs, const xAOD::PFOContainer* neuPFOs);
    void METfromxAOD();
    void METfromParticleContainers();
    
    double getNeuPFOwlMET();
    double getChPFOwlMET();  
    double getNeuPFOMET();
    double getChPFOMET();    
};

#endif