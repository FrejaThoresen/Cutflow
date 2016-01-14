#ifndef theSearch_MyMETAnalysis_H
#define theSearch_MyMETAnalysis_H

#include <EventLoop/Algorithm.h>
#include "TNtuple.h"
#include "NewWave/NewWave.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"


using namespace std;

class MyMETAnalysis : public EL::Algorithm
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
    MyMETAnalysis();
    
    string outputName;
    int m_eventCounter; //!
    int EventNumber; //!
    
    TNtuple* METTruthTree; //!
    TNtuple* chPFOMET; //!
    TNtuple* neuPFOMET; //!
    TNtuple* neuPFOwlMET; //!
    TNtuple* chPFOwlMET; //!

    TNtuple* PFOMET;//!
    TNtuple* PFOMETwl;//!
    TNtuple* diffPFO;//!
    TNtuple* diffPFOwl;//! 

    vector<TLorentzVector> chPFO_vec; //!
    vector<TLorentzVector> chPFO_vec_new; //! 
    vector<TLorentzVector> neuPFO_vec; //!
    vector<TLorentzVector> neuPFO_vec_new; //! 
ClassDef(MyMETAnalysis, 1);
};

#endif
