#ifndef theSearch_MyAnalysis_H
#define theSearch_MyAnalysis_H

#include <EventLoop/Algorithm.h>
#include "TNtuple.h"
#include "NewWave/NewWave.hh"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"


using namespace std;

class MyAnalysis : public EL::Algorithm
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
    MyAnalysis();
    
    string outputName;
    int m_eventCounter; //!
    int EventNumber; //!
    
    TNtuple* tree_Z_mu; //!
    TNtuple* tree_Z_e; //!
    TNtuple* tree_Z_j; //!
    TNtuple* chPFO; //!
    TNtuple* neuPFO; //!
    TNtuple* chPFOwavelet; //!
    TNtuple* neuPFOwavelet; //!
  
    vector<TLorentzVector> chPFO_vec; //!
    vector<TLorentzVector> chPFO_vec_new; //! 
    vector<TLorentzVector> neuPFO_vec; //!
    vector<TLorentzVector> neuPFO_vec_new; //! 
ClassDef(MyAnalysis, 1);
};

#endif
