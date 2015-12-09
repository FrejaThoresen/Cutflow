// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODRootAccess/tools/Message.h"
// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include <TTree.h>
#include "TFile.h"
#include <vector>
#include "TLorentzVector.h"
#include "EventLoop/OutputStream.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"
#include "Cutflow/ObjDef.h"
#include "xAODMuon/MuonContainer.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include <TSystem.h>


using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(ObjDef)

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


ObjDef :: ObjDef ()
{
}



EL::StatusCode ObjDef :: setupJob (EL::Job& job)
{
    // tell EventLoop about our output xAOD:
    EL::OutputStream out ("outputLabel", "xAOD");
    job.outputAdd (out);

    job.useXAOD ();
    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: histInitialize ()
{
    TFile *outputFile = wk()->getOutputFile (outputName);
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: fileExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: changeInput (bool firstFile)
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: initialize ()
{
 
    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // output xAOD
    TFile *file = wk()->getOutputFile ("outputLabel");
    EL_RETURN_CHECK("initialize()",event->writeTo(file));
 


    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    /*m_grl = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
    const char* GRLFilePath = "share/GRLs";
    const char* fullGRLFilePath = gSystem->ExpandPathName (GRLFilePath);
    std::vector<std::string> vecStringGRL;
    vecStringGRL.push_back(fullGRLFilePath);
    EL_RETURN_CHECK("initialize()",m_grl->setProperty( "GoodRunsListVec", vecStringGRL));
    EL_RETURN_CHECK("initialize()",m_grl->setProperty("PassThrough", false)); // if true (default) will ignore result of GRL and will just pass all events
    EL_RETURN_CHECK("initialize()",m_grl->initialize());
*/
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: execute ()
{
    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- EVENT INFORMATION -------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    xAOD::TEvent* event = wk()->xaodEvent();
    
    // print every 100 events, so we know where we are:
    if( (m_eventCounter % 1) ==0 ) //Info("execute()", "Event number = %i", m_eventCounter );
        m_eventCounter++;


    /*Info("execute()", " ");
    Info("execute()", "=====================");
    Info("execute()", " * Event number %4d", m_eventCounter);
    Info("execute()", "---------------------");
    */

    const xAOD::EventInfo* eventInfo = 0;
    EL_RETURN_CHECK("execute",event->retrieve( eventInfo, "EventInfo"));

    // check if the event is data or MC
    // (many tools are applied either to data or MC)
    bool isMC = false;
    // check if the event is MC
    if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
        isMC = true; // can do something with this later
    }

    // if data check if event passes GRL
    /*if(!isMC){ // it's data!
    if(!m_grl->passRunLB(*eventInfo)){
        //return EL::StatusCode::SUCCESS; // go to next event
        cout << "This is not in GoodRunsList" << endl;
        }
    } // end if not MC
*/

    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- CONTAINERS --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;
    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs, "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs, "JetETMissNeutralParticleFlowObjects" ) );

    const xAOD::JetContainer* jets = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( jets, "AntiKt4LCTopoJets" ));
    xAOD::JetContainer::const_iterator jet_itr = jets->begin();
    xAOD::JetContainer::const_iterator jet_end = jets->end();

    const xAOD::MuonContainer* muons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( muons, "Muons" ));
    xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
    xAOD::MuonContainer::const_iterator muon_end = muons->end();

    const xAOD::ElectronContainer* electrons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( electrons, "Electrons" ));
    xAOD::ElectronContainer::const_iterator electron_itr = electrons->begin();
    xAOD::ElectronContainer::const_iterator electron_end = electrons->end();


    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- DEEP COPY CONTAINERS ----------------------------------------
    //-------------------------------------------------------------------------------------------------------
    
    // ---------------------------------------------- Jets --------------------------------------------------
    // Create the new container and its auxiliary store.
    xAOD::JetContainer* goodJets = new xAOD::JetContainer();
    xAOD::AuxContainerBase* goodJetsAux = new xAOD::AuxContainerBase();
    goodJets->setStore( goodJetsAux ); //< Connect the two
       for( ; jet_itr != jet_end; ++jet_itr ) {
        //if( ! m_jetCleaning->accept( **jet_itr ) ) continue;
        // Copy this jet to the output container:
        xAOD::Jet* jet = new xAOD::Jet();
        goodJets->push_back( jet ); // jet acquires the goodJets auxstore
        *jet= **jet_itr; // copies auxdata from one auxstore to the other
    }
    // Record the objects into the output xAOD:
    EL_RETURN_CHECK("execute()",event->record( goodJets, "GoodJets" ));
    EL_RETURN_CHECK("execute()",event->record( goodJetsAux, "GoodJetsAux." ));


    // ---------------------------------------------- Muons --------------------------------------------------
    // Create the new container and its auxiliary store.
    xAOD::MuonContainer* goodMuons = new xAOD::MuonContainer();
    xAOD::AuxContainerBase* goodMuonsAux = new xAOD::AuxContainerBase();
    goodMuons->setStore( goodMuonsAux ); //< Connect the two
    float ptcone;
    for(; muon_itr != muon_end; ++muon_itr) {
        ptcone = (*muon_itr)->auxdata<float>("ptcone20");
        double iso = ptcone / (*muon_itr)->pt();
        double muon_pt = (*muon_itr)->pt();
        if (iso < 0.1 && muon_pt > 10000.0) {
            xAOD::Muon* muon = new xAOD::Muon();
            goodMuons->push_back(muon);
            *muon = **muon_itr;
        }
    }
    // Record the objects into the output xAOD:
    EL_RETURN_CHECK("execute()",event->record( goodMuons, "GoodMuons" ));
    EL_RETURN_CHECK("execute()",event->record( goodMuonsAux, "GoodMuonsAux." ));


    // ---------------------------------------------- Electrons -----------------------------------------------
    // Create the new container and its auxiliary store.
    xAOD::ElectronContainer* goodElectrons = new xAOD::ElectronContainer();
    xAOD::AuxContainerBase* goodElectronsAux = new xAOD::AuxContainerBase();
    goodElectrons->setStore( goodElectronsAux ); //< Connect the two
    for(; electron_itr != electron_end; ++electron_itr) {
        ptcone = (*electron_itr)->auxdata<float>("ptcone20");
        double electron_pt = (*electron_itr)->pt();
        double iso = ptcone / (*electron_itr)->pt();

        if (iso < 0.1 && electron_pt > 10000.0) {
            xAOD::Electron* electron = new xAOD::Electron();
            goodElectrons->push_back(electron);
            *electron = **electron_itr;
        }
    }
    // Record the objects into the output xAOD:
    EL_RETURN_CHECK("execute()",event->record( goodElectrons, "GoodElectrons" ));
    EL_RETURN_CHECK("execute()",event->record( goodElectronsAux, "GoodElectronsAux." ));


    // ---------------------------------------------- PFO --------------------------------------------------
    // Create the new container and its auxiliary store.
    xAOD::PFOContainer* goodPFOneu = new xAOD::PFOContainer();
    xAOD::AuxContainerBase* goodPFOneuAux = new xAOD::AuxContainerBase();
    goodPFOneu->setStore( goodPFOneuAux ); //< Connect the two
    xAOD::PFOContainer* goodPFOch = new xAOD::PFOContainer();
    xAOD::AuxContainerBase* goodPFOchAux = new xAOD::AuxContainerBase();
    goodPFOch->setStore( goodPFOchAux ); //< Connect the two
    for (unsigned int i = 0; i < chPFOs->size(); i++) {
        if (chPFOs->at(i)->pt() > 10000.0) {
                   xAOD::PFO* pfo = new xAOD::PFO();
                   goodPFOch->push_back(pfo);
                   *pfo = *(chPFOs->at(i));
        }
    }
    for (unsigned int i = 0; i < neuPFOs->size(); i++) {
        if (neuPFOs->at(i)->pt() > 10000.0) {
                   xAOD::PFO* pfo = new xAOD::PFO();
                   goodPFOneu->push_back(pfo);
                   *pfo = *(neuPFOs->at(i));
        }
    }
    // Record the objects into the output xAOD:
    EL_RETURN_CHECK("execute()",event->record( goodPFOneu, "GoodPFOneu" ));
    EL_RETURN_CHECK("execute()",event->record( goodPFOneuAux, "GoodPFOneuAux." ));
    EL_RETURN_CHECK("execute()",event->record( goodPFOch, "GoodPFOch" ));
    EL_RETURN_CHECK("execute()",event->record( goodPFOchAux, "GoodPFOchAux." ));


    event->fill();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: finalize ()
{
    xAOD::TEvent* event = wk()->xaodEvent();

   /*   if (m_grl) {
    delete m_grl;
    m_grl = 0;
  }*/
    // finalize and close our output xAOD file:
TFile *file = wk()->getOutputFile ("outputLabel");
EL_RETURN_CHECK("finalize()",event->finishWritingTo( file ));
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode ObjDef :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

