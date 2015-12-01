#ifndef theSearch_Boildown_H
#define theSearch_Boildown_H

#include <EventLoop/Algorithm.h>
#include "TNtuple.h"
#include "NewWave/NewWave.hh"


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

    TNtuple* chPFO; //!
    TNtuple* chPFOwavelet; //!
    TNtuple* neuPFO; //!
    TNtuple* neuPFOwavelet; //!

  int    nPixel; //!
  double yRange; //!
  NewWave::PixelDefinition *_pixelDefinition; //!
  NewWave::WaveletEngine   *_waveletEngine; //!

  ClassDef(Boildown, 1);

};

#endif
