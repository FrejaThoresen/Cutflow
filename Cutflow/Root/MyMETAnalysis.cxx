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
#include "xAODMissingET/MissingETContainer.h"
#include "TH2.h"

using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(MyMETAnalysis)
/*
* This class is for making NTuples of MET values.
*/

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

    METTruthTree = new TNtuple("METTruthTree", "MET Truth","METTruthpt");
    chPFOMET = new TNtuple("chPFOMET", "Charged Particle Flow MET","chPFOptMET");
    neuPFOMET = new TNtuple("neuPFOMET", "Neutral Particle Flow MET","neuPFOptMET");
    neuPFOwlMET = new TNtuple("neuPFOwlMET", "Neutral Particle Flow MET with Wavelets","neuPFOwlptMET");
    chPFOwlMET = new TNtuple("chPFOwlMET", "Charged Particle Flow MET with Wavelets","chPFOwlptMET");

    PFOMET = new TNtuple("PFOMET", "Particle Flow MET", "pfoMET");
    PFOMETwl = new TNtuple("PFOMETwl", "Particle Flow MET with Wavelets", "pfoMETwl");
    diffPFO = new TNtuple("diffPFOMET", "Difference of Particle Flow MET and MET_Truth", "pfoMETdiff");
    diffPFOwl= new TNtuple("diffPFOMETwl", "Difference of Particle Flow MET with Wavelets and MET_Truth", "pfoMETdiffwl");



    METTruthTree->SetDirectory (outputFile);
    chPFOMET->SetDirectory (outputFile);
    neuPFOMET->SetDirectory (outputFile);
    neuPFOwlMET->SetDirectory (outputFile);
    chPFOwlMET->SetDirectory (outputFile);
    PFOMET->SetDirectory (outputFile);
    PFOMETwl->SetDirectory (outputFile);
    diffPFO->SetDirectory (outputFile);
    diffPFOwl->SetDirectory (outputFile);


    h2 = new TH2F("h2","h2 title", 100, -4, 4, 100, -3, 3 );

    wk()->addOutput(h2);


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
        cout << "This is MC" << endl;
    }

    // fill the branches of our trees
    EventNumber = eventInfo->eventNumber();
    double N_pileup = eventInfo->averageInteractionsPerCrossing();

    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- CONTAINERS -------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;
    const xAOD::MissingETContainer* METcontainer = 0;

    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs,  "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs, "JetETMissNeutralParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( METcontainer, "MET_Truth"));
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
    //---------------------------------------------- CONTOUR PLOT --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------


    for (int i = 0; i < neuPFO_vec.size(); i++) {
        h2->Fill(neuPFO_vec.at(i).Eta(), neuPFO_vec.at(i).Phi(),  neuPFO_vec.at(i).Pt());
    }
    h2->Draw("COLZ");

























    //-------------------------------------------------------------------------------------------------------
    //------------------------------------------------ MET --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    METAnalysis * metAnalyzer = new METAnalysis(N_pileup);
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

    double METTruthpt = 0;

    for (unsigned int i = 0; i < METcontainer->size(); i++) {
        METTruthpt = METcontainer->at(i)->met();
            METTruthTree->Fill(METTruthpt);
            cout << "truth" << METTruthpt << endl;
    }    

    double pfoMET = chPFOptMET + neuPFOptMET;
    double pfoMETwl = chPFOwlptMET + neuPFOwlptMET;
    double pfoMETdiff = METTruthpt - pfoMET;
    double pfoMETdiffwl = METTruthpt - pfoMETwl;

    PFOMET->Fill(pfoMET);
    PFOMETwl->Fill(pfoMETwl);
    diffPFO->Fill(pfoMETdiff);
    diffPFOwl->Fill(pfoMETdiffwl);
    

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: finalize ()
{
    //xAOD::TEvent* event = wk()->xaodEvent();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyMETAnalysis :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

