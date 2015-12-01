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
    // Here you put any code for the base initialization of variables,
    // e.g. initialize all pointers to 0.  Note that you should only put
    // the most basic initialization here, since this method will be
    // called on both the submission and the worker node.  Most of your
    // initialization code will go into histInitialize() and
    // initialize().
}



EL::StatusCode SearchInxAOD :: setupJob (EL::Job& job)
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



EL::StatusCode SearchInxAOD :: histInitialize ()
{
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.

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
    // Here you do everything you need to do when we change input files,
    // e.g. resetting branch addresses on trees.  If you are using
    // D3PDReader or a similar service this method is not needed.
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: initialize ()
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



EL::StatusCode SearchInxAOD :: execute ()
{
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.


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
    //tree->Fill();
    // ---------------------------------------------- VECTORS -----------------------------------------------
    vector<TLorentzVector> mu_vector_pos;
    vector<TLorentzVector> mu_vector_neg;

    //-------------------------------------------------------------------------------------------------------
    //------------------------------------------------ Z BOSONS ---------------------------------------------
    //-------------------------------------------------------------------------------------------------------

    // USING THE CUTS CLASS
    CutsInxAOD *analyzer = new CutsInxAOD();


    //const xAOD::PFOContainer* pflowForwardContainter = 0;
    //const xAOD::PFOContainer* pflowCentralContainter = 0;


    const xAOD::PFOContainer* chPFOs = 0;
    const xAOD::PFOContainer* neuPFOs = 0;


    //EL_RETURN_CHECK("execute()",event->retrieve( pflowForwardContainter, "ParticleFlowIsoForwardEventShape" ));
    //EL_RETURN_CHECK("execute()",event->retrieve( pflowCentralContainter, "ParticleFlowIsoCentralEventShape" ));

    EL_RETURN_CHECK("execute()", event->retrieve( chPFOs,      "JetETMissChargedParticleFlowObjects" ) );
    EL_RETURN_CHECK("execute()", event->retrieve( neuPFOs,     "JetETMissNeutralParticleFlowObjects" ) );


      // * Charged.
  xAOD::PFOContainer::const_iterator PFlowCharged_itr = chPFOs->begin();
  xAOD::PFOContainer::const_iterator PFlowCharged_end = chPFOs->end();
  for ( ; PFlowCharged_itr != PFlowCharged_end; ++PFlowCharged_itr) {
    chPFOpt = (*PFlowCharged_itr)->pt();
    cout << "PLOW ENERGY#########################################" << chPFOpt << endl;
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


    /*m_jetCleaning->msg().setLevel( MSG::DEBUG );

    // get muon container of interest
    const xAOD::MuonContainer* muons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( muons, "Muons" ));

    const xAOD::ElectronContainer* electrons = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( electrons, "Electrons" ));

    const xAOD::JetContainer* jets = 0;
    EL_RETURN_CHECK("execute()",event->retrieve( jets, "AntiKt4LCTopoJets" ));//TauJets

    analyzer->analyzeZbosonsFromJets(jets,m_jetCleaning);
    analyzer->analyzeZbosonsFromElectrons(electrons);
    analyzer->analyzeZbosonsFromMuons(muons);

    Z_from_electrons = analyzer->getZbosonsFromElectrons();
    Z_from_muons = analyzer->getZbosonsFromMuons();
    Z_from_jets = analyzer->getZbosonsFromJets();

    for (int i = 0; i < (int)Z_from_electrons.size(); i++) {
        Z_m_e = Z_from_electrons[i].M();
        Z_pt_e = Z_from_electrons[i].Pt();
        tree_Z_e->Fill(Z_m_e,Z_pt_e);
    }

    for (int j = 0; j < (int)Z_from_muons.size(); j++) {
        Z_m_mu = Z_from_muons[j].M();
        Z_pt_mu  = Z_from_muons[j].Pt();
        tree_Z_mu->Fill(Z_m_mu,Z_pt_mu);
    }

    if (Z_from_jets.size() != 0 ) {
        for (int k = 0; k < (int)Z_from_jets.size(); k++) {
            Z_m_j = Z_from_jets[k].M();
            Z_pt_j  = Z_from_jets[k].Pt();
            cout << Z_m_j << endl;
            tree_Z_j->Fill(Z_m_j,Z_pt_j);
        }
    }
    */
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: postExecute ()
{
    // Here you do everything that needs to be done after the main event
    // processing.  This is typically very rare, particularly in user
    // code.  It is mainly used in implementing the NTupleSvc.
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode SearchInxAOD :: finalize ()
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



EL::StatusCode SearchInxAOD :: histFinalize ()
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

