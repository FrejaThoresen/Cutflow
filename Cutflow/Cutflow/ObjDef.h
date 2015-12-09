#ifndef theSearch_ObjDef_H
#define theSearch_ObjDef_H

#include <EventLoop/Algorithm.h>
#include "TNtuple.h"
#include "NewWave/NewWave.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "JetSelectorTools/JetCleaningTool.h"


using namespace std;

class ObjDef : public EL::Algorithm
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
    ObjDef();
    string outputName;
    int m_eventCounter; //!
    int EventNumber; //!
GoodRunsListSelectionTool *m_grl; //!
    JetCleaningTool *m_jetCleaning; //!

  ClassDef(ObjDef, 1);

};

#endif
