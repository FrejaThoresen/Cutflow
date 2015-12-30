// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <Cutflow/SearchInxAOD.h>
#include "xAODRootAccess/tools/Message.h"
// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"



#include <TTree.h>
#include "TFile.h"
#include <vector>
#include "TLorentzVector.h"

#include "xAODPFlow/PFOContainer.h"

#include "Cutflow/CutsInxAOD.h"
#include "xAODJet/JetContainer.h"
using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(SearchInxAOD)

/// Helper macro for checking xAOD::TReturnCode return values
#define EL_RETURN_CHECK( CONTEXT, EXP )                     \
   do {                                                     \
      if( ! EXP.isSuccess() ) {                             \
         Error( CONTEXT,                                    \
                XAOD_MESSAGE( "Failed to execute: %s" ),    \
                #EXP );                                     \
         return EL::StatusCode::FAILURE;                    \
      }                                                     \
   } while( false )


SearchInxAOD :: SearchInxAOD ()
{
}



EL::StatusCode SearchInxAOD :: setupJob (EL::Job& job)
{
    // let's initialize the algorithm to use the xAODRootAccess package
    job.useXAOD ();
    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: histInitialize ()
{

    TFile *outputFile = wk()->getOutputFile (outputName);
    tree_Z_mu = new TNtuple("tree_Z_mu", "tree_Z_mu","Z_m_mu:Z_pt_mu");
    tree_Z_e = new TNtuple("tree_Z_e", "tree_Z_e","Z_m_e:Z_pt_e");
    tree_Z_j = new TNtuple("tree_Z_j", "tree_Z_j","Z_m_j:Z_pt_j");
    chPFO = new TNtuple("chPFO", "chPFO","chPFOpt");
    neuPFO = new TNtuple("neuPFO", "neuPFO","neuPFOpt");

    tree_Z_mu->SetDirectory (outputFile);
    tree_Z_e->SetDirectory (outputFile);
    tree_Z_j->SetDirectory (outputFile);
    chPFO->SetDirectory (outputFile);
    neuPFO->SetDirectory (outputFile);

    //tree->Branch("EventNumber", &EventNumber);

    m_jetCleaning = new JetCleaningTool("JetCleaning");
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: fileExecute ()
{
    // Here you do everything that needs to be done exactly once for every
    // single file, e.g. collect a list of all lumi-blocks processed
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: changeInput (bool firstFile)
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: initialize ()
{

    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: execute ()
{
    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- EVENT INFORMATION -------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    xAOD::TEvent* event = wk()->xaodEvent();

    // print every 100 events, so we know where we are:
    if( (m_eventCounter % 1) ==0 ) //Info("execute()", "Event number = %i", m_eventCounter );
        m_eventCounter++;

    const xAOD::EventInfo* eventInfo = 0;
    EL_RETURN_CHECK("execute",event->retrieve( eventInfo, "EventInfo"));

    // check if the event is data or MC
    // (many tools are applied either to data or MC)
    bool isMC = false;
    // check if the event is MC
    if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
        isMC = true; // can do something with this later
    }

    // fill the branches of our trees
    EventNumber = eventInfo->eventNumber();
    cout << "EventNumber = " << EventNumber << endl;
    //tree->Fill();
    // ---------------------------------------------- VECTORS -----------------------------------------------
    vector<TLorentzVector> mu_vector_pos;
    vector<TLorentzVector> mu_vector_neg;

    //-------------------------------------------------------------------------------------------------------
    //------------------------------------------------ Z BOSONS ---------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    // USING THE CUTS CLASS
    CutsInxAOD *analyzer = new CutsInxAOD();

    /*
    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;

    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs,      "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs,     "JetETMissNeutralParticleFlowObjects" ) );


      // * Charged.
    xAOD::PFOContainer::const_iterator PFlowCharged_itr = chPFOs->begin();
    xAOD::PFOContainer::const_iterator PFlowCharged_end = chPFOs->end();
    for ( ; PFlowCharged_itr != PFlowCharged_end; ++PFlowCharged_itr) {
        chPFOpt = (*PFlowCharged_itr)->pt();
        //cout << "PLOW ENERGY#########################################" << chPFOpt << endl;
        chPFO->Fill(chPFOpt);
    }

    // * neutral.
    xAOD::PFOContainer::const_iterator PFlowNeutral_itr = neuPFOs->begin();
    xAOD::PFOContainer::const_iterator PFlowNeutral_end = neuPFOs->end();
    for ( ; PFlowNeutral_itr != PFlowNeutral_end; ++PFlowNeutral_itr) {
        neuPFOpt = (*PFlowNeutral_itr)->pt();
        neuPFO->Fill(neuPFOpt);
    }


//    const xAOD::JetContainer* jets = 0;
 //   EL_RETURN_CHECK("execute()",event->retrieve( jets, "AntiKt10LCTopoJets" ));//TauJets


    //m_jetCleaning->msg().setLevel( MSG::DEBUG );

    // get muon container of interest


    /*

    const xAOD::JetContainer* jets = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( jets, "AntiKt4LCTopoJets" ));//TauJets

    analyzer->analyzeZbosonsFromJets(jets,m_jetCleaning);

    Z_from_jets = analyzer->getZbosonsFromJets();


    if (Z_from_jets.size() != 0 ) {
        for (int k = 0; k < (int)Z_from_jets.size(); k++) {
            Z_m_j = Z_from_jets[k].M();
            Z_pt_j  = Z_from_jets[k].Pt();
            cout << Z_m_j << endl;
            tree_Z_j->Fill(Z_m_j,Z_pt_j);
        }
    }
    */

    const xAOD::ElectronContainer* electrons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( electrons, "Electrons" ));
    
    analyzer->analyzeZbosonsFromElectrons(electrons);
    Z_from_electrons = analyzer->getZbosonsFromElectrons();

    for (int i = 0; i < (int)Z_from_electrons.size(); i++) {
        Z_m_e = Z_from_electrons[i].M();
        Z_pt_e = Z_from_electrons[i].Pt();
        tree_Z_e->Fill(Z_m_e,Z_pt_e);
    }

    const xAOD::MuonContainer* muons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( muons, "Muons" ));

    analyzer->analyzeZbosonsFromMuons(muons);
    Z_from_muons = analyzer->getZbosonsFromMuons();
    if (Z_from_muons.size() > 0 || Z_from_electrons.size() > 0) {
        cout << "found a Z" << endl;
    }
    else {
        cout << "no Z" << endl;
    }

    for (int j = 0; j < (int)Z_from_muons.size(); j++) {
        Z_m_mu = Z_from_muons[j].M();
        Z_pt_mu  = Z_from_muons[j].Pt();
        tree_Z_mu->Fill(Z_m_mu,Z_pt_mu);
    }

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: finalize ()
{
    xAOD::TEvent* event = wk()->xaodEvent();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

