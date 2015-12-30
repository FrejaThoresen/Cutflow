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
#include "Cutflow/MyMETAnalysis.h"
#include "Cutflow/METAnalysis.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"
//#include "xAODMissingET/MissingETContainer.h"

using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(MyMETAnalysis)

// Helper macro for checking xAOD::TReturnCode return values
#define EL_RETURN_CHECK( CONTEXT, EXP )                     \
   do {                                                     \
      if( ! EXP.isSuccess() ) {                             \
         Error( CONTEXT,                                    \
                XAOD_MESSAGE( "Failed to execute: %s" ),    \
                #EXP );                                     \
         return EL::StatusCode::FAILURE;                    \
      }                                                     \
   } while( false )


MyMETAnalysis :: MyMETAnalysis ()
{
}



EL::StatusCode MyMETAnalysis :: setupJob (EL::Job& job)
{
    job.useXAOD ();
    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: histInitialize ()
{
    TFile *outputFile = wk()->getOutputFile (outputName);

    METTruthTree = new TNtuple("METTruthTree", "METTruthTree","METTruthpt");
    chPFOMET = new TNtuple("chPFOMET", "chPFOMET","chPFOptMET");
    neuPFOMET = new TNtuple("neuPFOMET", "neuPFOMET","neuPFOptMET");
    neuPFOwlMET = new TNtuple("neuPFOwlMET", "neuPFOwlMET","neuPFOwlptMET");
    chPFOwlMET = new TNtuple("chPFOwlMET", "chPFOwlMET","chPFOwlptMET");


    METTruthTree->SetDirectory (outputFile);
    chPFOMET->SetDirectory (outputFile);
    neuPFOMET->SetDirectory (outputFile);
    neuPFOwlMET->SetDirectory (outputFile);
    chPFOwlMET->SetDirectory (outputFile);

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: fileExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: changeInput (bool firstFile)
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: initialize ()
{

    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: execute ()
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

    
    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- CONTAINERS -------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;

    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs,  "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs, "JetETMissNeutralParticleFlowObjects" ) );

    //-------------------------------------------------------------------------------------------------------
    //---------------------------------------------- PFLOW --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    for (unsigned int i = 0; i < chPFOs->size(); i++) {
        chPFO_vec.push_back(TLorentzVector());
        chPFO_vec.back().SetPtEtaPhiM(chPFOs->at(i)->pt(), chPFOs->at(i)->eta(), chPFOs->at(i)->phi(), chPFOs->at(i)->m());
    }

    for (unsigned int i = 0; i < neuPFOs->size(); i++) {
        neuPFO_vec.push_back(TLorentzVector());
        neuPFO_vec.back().SetPtEtaPhiM(neuPFOs->at(i)->pt(), neuPFOs->at(i)->eta(), neuPFOs->at(i)->phi(), neuPFOs->at(i)->m());
    }


    //-------------------------------------------------------------------------------------------------------
    //------------------------------------------------ MET --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    METAnalysis * metAnalyzer = new METAnalysis();
    metAnalyzer->METfromPFOwWavelets(chPFOs, neuPFOs);
    metAnalyzer->METfromPFO(chPFOs, neuPFOs);
   
    double neuPFOwlptMET = metAnalyzer->getNeuPFOwlMET();
    neuPFOwlMET->Fill(neuPFOwlptMET);

    double chPFOwlptMET = metAnalyzer->getChPFOwlMET();
    chPFOwlMET->Fill(chPFOwlptMET);

    double neuPFOptMET = metAnalyzer->getNeuPFOMET();
    neuPFOMET->Fill(neuPFOptMET);

    double chPFOptMET = metAnalyzer->getChPFOMET();
    chPFOMET->Fill(chPFOptMET);

    double METTruthpt =0;
    //METTruth->Fill(METTruthpt);

    //-------------------------------------------------------------------------------------------------------
    //------------------------------------------------ MET TRUTH --------------------------------------------
    //-------------------------------------------------------------------------------------------------------


    //const xAOD::MissingETContainer* METTruthContain = 0;

    //EL_RETURN_CHECK("execute()", event->retrieve( METTruthContain,  "MET_Truth" ) );
    METTruthTree->Fill(METTruthpt);
/*
    for (unsigned int i = 0; i < METTruthContain->size(); i++) {
        METTruthpt = METTruthContain->at(i)->mpx()
        METTruthTree->Fill(METTruthpt);
    }
*/
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: finalize ()
{
    xAOD::TEvent* event = wk()->xaodEvent();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

