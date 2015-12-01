// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODRootAccess/tools/Message.h"
// EDM includes:
#include "xAODEventInfo/EventInfo.h"

#include <TTree.h>
#include "TFile.h"
#include <vector>
#include "TLorentzVector.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"
#include "Cutflow/Boildown.h"

#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"

using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(Boildown)

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


Boildown :: Boildown ()
{
    // Here you put any code for the base initialization of variables,
    // e.g. initialize all pointers to 0.  Note that you should only put
    // the most basic initialization here, since this method will be
    // called on both the submission and the worker node.  Most of your
    // initialization code will go into histInitialize() and
    // initialize().
}



EL::StatusCode Boildown :: setupJob (EL::Job& job)
{
    // Here you put code that sets up the job on the submission object
    // so that it is ready to work with your algorithm, e.g. you can
    // request the D3PDReader service or add output files.  Any code you
    // put here could instead also go into the submission script.  The
    // sole advantage of putting it here is that it gets automatically
    // activated/deactivated when you add/remove the algorithm from your
    // job, which may or may not be of value to you.

    // let's initialize the algorithm to use the xAODRootAccess package
    job.useXAOD ();
    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: histInitialize ()
{
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.

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



EL::StatusCode Boildown :: fileExecute ()
{
    // Here you do everything that needs to be done exactly once for every
    // single file, e.g. collect a list of all lumi-blocks processed
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: changeInput (bool firstFile)
{
    // Here you do everything you need to do when we change input files,
    // e.g. resetting branch addresses on trees.  If you are using
    // D3PDReader or a similar service this method is not needed.
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: initialize ()
{
    // Here you do everything that you need to do after the first input
    // file has been connected and before the first event is processed,
    // e.g. create additional histograms based on which variables are
    // available in the input files.  You can also create all of your
    // histograms and trees in here, but be aware that this method
    // doesn't get called if no events are processed.  So any objects
    // you create here won't be available in the output if you have no
    // input events.

    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: execute ()
{
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.


    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- EVENT INFORMATION -------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    xAOD::TEvent* event = wk()->xaodEvent();
    /*Info("execute()", " ");
    Info("execute()", "=====================");
    Info("execute()", " * Event number %4d", m_eventCounter);
    Info("execute()", "---------------------");
*/
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
    //---------------------------------------------- PFLOW --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;

    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs, "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs, "JetETMissNeutralParticleFlowObjects" ) );

    double chPFOpt = 0.;
    double neuPFOpt = 0.;

      // * Charged.
    vector<TLorentzVector> chPFO_vec_new; 
    vector<TLorentzVector> chPFO_vec; 

    for (unsigned int i = 0; i < chPFOs->size(); i++) {
        chPFOpt = chPFOs->at(i)->pt();
        if (chPFOpt > 10000.0) {
                    chPFO->Fill(chPFOpt);
                    chPFO_vec.push_back(TLorentzVector());
                    chPFO_vec.back().SetPtEtaPhiM(chPFOs->at(i)->pt(), chPFOs->at(i)->eta(), chPFOs->at(i)->phi(), chPFOs->at(i)->m());
        }
    }

    vector<TLorentzVector> neuPFO_vec_new; 
    vector<TLorentzVector> neuPFO_vec; 

    for (unsigned int i = 0; i < neuPFOs->size(); i++) {
        neuPFOpt = neuPFOs->at(i)->pt();
        if (neuPFOpt > 10000.0) {
                    neuPFO->Fill(neuPFOpt);
                    neuPFO_vec.push_back(TLorentzVector());
                    neuPFO_vec.back().SetPtEtaPhiM(neuPFOs->at(i)->pt(), neuPFOs->at(i)->eta(), neuPFOs->at(i)->phi(), neuPFOs->at(i)->m());
        }
    }

    //-------------------------------------------------------------------------------------------------------
    //---------------------------------------------- WAVELET ------------------------------------------------
    //-------------------------------------------------------------------------------------------------------
   // hello
    int nPixel =  64;
    double yRange = 3.2;
    NewWave::PixelDefinition* _pixelDefinition = new NewWave::PixelDefinition(nPixel, yRange);
    NewWave::GSLEngine* _waveletEngine = new NewWave::GSLEngine(gsl_wavelet_haar, 2, *_pixelDefinition);

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowNeu(neuPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowNeu.denoise(1.);
    neuPFO_vec_new = wePFlowNeu.particles();

    NewWave::WaveletEvent<vector<TLorentzVector>> wePFlowCh(chPFO_vec, *_pixelDefinition, *_waveletEngine);
    wePFlowCh.denoise(1.);
    chPFO_vec_new = wePFlowCh.particles();

    
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



EL::StatusCode Boildown :: postExecute ()
{
    // Here you do everything that needs to be done after the main event
    // processing.  This is typically very rare, particularly in user
    // code.  It is mainly used in implementing the NTupleSvc.
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: finalize ()
{
    // This method is the mirror image of initialize(), meaning it gets
    // called after the last event has been processed on the worker node
    // and allows you to finish up any objects you created in
    // initialize() before they are written to disk.  This is actually
    // fairly rare, since this happens separately for each worker node.
    // Most of the time you want to do your post-processing on the
    // submission node after all your histogram outputs have been
    // merged.  This is different from histFinalize() in that it only
    // gets called on worker nodes that processed input events.

    xAOD::TEvent* event = wk()->xaodEvent();
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: histFinalize ()
{
    // This method is the mirror image of histInitialize(), meaning it
    // gets called after the last event has been processed on the worker
    // node and allows you to finish up any objects you created in
    // histInitialize() before they are written to disk.  This is
    // actually fairly rare, since this happens separately for each
    // worker node.  Most of the time you want to do your
    // post-processing on the submission node after all your histogram
    // outputs have been merged.  This is different from finalize() in
    // that it gets called on all worker nodes regardless of whether
    // they processed input events.
    return EL::StatusCode::SUCCESS;
}

