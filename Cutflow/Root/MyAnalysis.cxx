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
#include "Cutflow/MyAnalysis.h"
#include "Cutflow/METAnalysis.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"
#include "Cutflow/WaveletAnalysis.h"

using namespace std;

ClassImp(MyAnalysis)
/*
* This class is made for showing the effect on PFO pt when using wavelets.
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


MyAnalysis :: MyAnalysis ()
{
}



EL::StatusCode MyAnalysis :: setupJob (EL::Job& job)
{
    job.useXAOD ();
    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: histInitialize ()
{
    TFile *outputFile = wk()->getOutputFile (outputName);

    chPFO = new TNtuple("chPFO", "chPFO","chPFOpt");
    neuPFO = new TNtuple("neuPFO", "neuPFO","neuPFOpt");
    neuPFOwavelet = new TNtuple("neuPFOwavelet", "neuPFOwavelet","neuPFOwaveletPt");
    chPFOwavelet = new TNtuple("chPFOwavelet", "chPFOwavelet","chPFOwaveletPt");

    chPFO->SetDirectory (outputFile);
    neuPFO->SetDirectory (outputFile);
    neuPFOwavelet->SetDirectory (outputFile);
    chPFOwavelet->SetDirectory (outputFile);

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: fileExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: changeInput (bool firstFile)
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: initialize ()
{

    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: execute ()
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
    double N_pileup = eventInfo->averageInteractionsPerCrossing();

    cout << "Pile up is: " << N_pileup << endl;
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
        chPFO->Fill(chPFOs->at(i)->pt());
        chPFO_vec.push_back(TLorentzVector());
        chPFO_vec.back().SetPtEtaPhiM(chPFOs->at(i)->pt(), chPFOs->at(i)->eta(), chPFOs->at(i)->phi(), chPFOs->at(i)->m());
    }

    for (unsigned int i = 0; i < neuPFOs->size(); i++) {
        neuPFO->Fill(neuPFOs->at(i)->pt());
        neuPFO_vec.push_back(TLorentzVector());
        neuPFO_vec.back().SetPtEtaPhiM(neuPFOs->at(i)->pt(), neuPFOs->at(i)->eta(), neuPFOs->at(i)->phi(), neuPFOs->at(i)->m());
    }

    //-------------------------------------------------------------------------------------------------------
    //---------------------------------------------- WAVELET ------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    WaveletAnalysis* wlAnalyzer = new WaveletAnalysis(64, 3.2, N_pileup);

    //int nPixel =  64;
    //double yRange = 3.2;
    //NewWave::PixelDefinition* _pixelDefinition = new NewWave::PixelDefinition(nPixel, yRange);
    //NewWave::GSLEngine* _waveletEngine = new NewWave::GSLEngine(gsl_wavelet_haar, 2, *_pixelDefinition);

    /*NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowNeu(neuPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowNeu.denoise(1.);
    neuPFO_vec_new = wePFlowNeu.particles();

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowCh(chPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowCh.denoise(1.);
    chPFO_vec_new = wePFlowCh.particles();
*/
    wlAnalyzer->analyzeNeuPFO(neuPFO_vec);
    wlAnalyzer->analyzeChPFO(chPFO_vec);

    chPFO_vec_new = wlAnalyzer->getChPFOwave();
    neuPFO_vec_new = wlAnalyzer->getNeuPFOwave();


    double neuPFOwaveletPt = 0.;
    for (unsigned int j = 0; j < neuPFO_vec_new.size(); j++) {
        neuPFOwaveletPt = neuPFO_vec_new.at(j).Pt();
        neuPFOwavelet->Fill(neuPFOwaveletPt);
    }

    double chPFOwaveletPt = 0.;
    for (unsigned int j = 0; j < chPFO_vec_new.size(); j++) {
        chPFOwaveletPt = chPFO_vec_new.at(j).Pt();
        chPFOwavelet->Fill(chPFOwaveletPt);
    }

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: finalize ()
{
    xAOD::TEvent* event = wk()->xaodEvent();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAnalysis :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

