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
#include "Cutflow/Boildown.h"
#include "Cutflow/CutsInxAOD.h"
#include "xAODMuon/MuonContainer.h"
#include "NewWave/NewWave.hh"
#include "NewWave/GSLEngine.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include <TSystem.h>

#include <fstream>
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
}



EL::StatusCode Boildown :: setupJob (EL::Job& job)
{
    // tell EventLoop about our output xAOD:
    EL::OutputStream out ("outputLabel", "xAOD");
    job.outputAdd (out);

    job.useXAOD ();

    EL_RETURN_CHECK( "setupJob()", xAOD::Init() ); // call before opening first file
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: histInitialize ()
{
    TFile *outputFile = wk()->getOutputFile (outputName);
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: fileExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: changeInput (bool firstFile)
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: initialize ()
{
 
    // count number of events
    m_eventCounter = 0;
    xAOD::TEvent* event = wk()->xaodEvent();

    // output xAOD
    TFile *file = wk()->getOutputFile ("outputLabel");
    EL_RETURN_CHECK("initialize()",event->writeTo(file));

    // as a check, let's see the number of events in our xAOD
    Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

    /*
    m_grl = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
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



EL::StatusCode Boildown :: execute ()
{
    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- EVENT INFORMATION -------------------------------------------
    //-------------------------------------------------------------------------------------------------------
    bool copyEvent = false;
    xAOD::TEvent* event = wk()->xaodEvent();
    
    // print every 100 events, so we know where we are:
    if( (m_eventCounter % 1) ==0 ) //Info("execute()", "Event number = %i", m_eventCounter );
        m_eventCounter++;

    /*
    Info("execute()", " ");
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
        cout << "------ Data is from MC" << endl;
    }
    else {
        cout << "------ Data is from raw data" << endl;
    }

    // if data check if event passes GRL
    /*
    if(!isMC){ // it's data!
    if(!m_grl->passRunLB(*eventInfo)){
        //return EL::StatusCode::SUCCESS; // go to next event
        cout << "This is not in GoodRunsList" << endl;
        }
    } // end if not MC
    */

    //-------------------------------------------------------------------------------------------------------
    //----------------------------------------- CONTAINERS --------------------------------------------------
    //-------------------------------------------------------------------------------------------------------

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

    CutsInxAOD* cutAnalyzer = new CutsInxAOD();

    // ---------------------------------------------- Muons --------------------------------------------------

    cutAnalyzer->analyzeZbosonsFromElectrons(electrons);
    cutAnalyzer->analyzeZbosonsFromMuons(muons);

    Z_from_electrons = cutAnalyzer->getZbosonsFromElectrons();
    Z_from_muons = cutAnalyzer->getZbosonsFromMuons();

    if (Z_from_muons.size() > 0 || Z_from_electrons.size() > 0) {
        copyEvent = true;
    }
    else {
        copyEvent = false;
    }

    if (copyEvent == true) {
        cout << "copying event " << m_eventCounter << endl;
        string line;
        ifstream myfile ("/hep/thoresen/work/Cutflow/Cutflow/share/output.txt");
        if (myfile.is_open())
        {
            cout << "   file is open" << endl;
            while ( getline (myfile,line) )
            {
                if (event->copy(line).isSuccess()) {
                    //cout << "reading line.." << endl;
                    //cout << line << endl;
                    //cout << typeid(line).name() << endl;
                    EL_RETURN_CHECK("execute()",event->copy(line));
                }
            }
            myfile.close();
            cout << "   file is closed" << endl;
            event->fill();
        }
        else cout << "Unable to open file"; 
    }
    else {
        //cout << "event not copied" << endl;
    }

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: postExecute ()
{
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: finalize ()
{
    xAOD::TEvent* event = wk()->xaodEvent();

    /*   
    if (m_grl) {
    delete m_grl;
    m_grl = 0;
    }
    */
    
    // finalize and close our output xAOD file:
    TFile *file = wk()->getOutputFile ("outputLabel");
    EL_RETURN_CHECK("finalize()",event->finishWritingTo( file ));
    return EL::StatusCode::SUCCESS;
}



EL::StatusCode Boildown :: histFinalize ()
{
    return EL::StatusCode::SUCCESS;
}

