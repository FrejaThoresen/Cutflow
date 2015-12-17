#ifndef theSearch_Boildown_H
#define theSearch_Boildown_H

#include <EventLoop/Algorithm.h>
#include "TNtuple.h"
#include "NewWave/NewWave.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "JetSelectorTools/JetCleaningTool.h"


using namespace std;

class Boildown : public EL::Algorithm
{



public:
    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob (EL::Job& job);
    virtual EL::StatusCode fileExecute ();
    virtual EL::StatusCode histInitialize ();
    virtual EL::StatusCode changeInput (bool firstFile);
    virtual EL::StatusCode initialize ();
    virtual EL::StatusCode execute ();
    virtual EL::StatusCode postExecute ();
    virtual EL::StatusCode finalize ();
    virtual EL::StatusCode histFinalize ();
    
    Boildown();

    string outputName;
    int m_eventCounter; //!
    int EventNumber; //!
    GoodRunsListSelectionTool *m_grl; //!
    JetCleaningTool *m_jetCleaning; //!

    vector<TLorentzVector> Z_from_muons; //!
    vector<TLorentzVector> Z_from_electrons; //! 

    ClassDef(Boildown, 1);
};

#endif
