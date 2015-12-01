#ifndef NEWWAVE_WAVELET_EVENT_HH
#define NEWWAVE_WAVELET_EVENT_HH

#include "NewWave/PixelDefinition.hh"
#include "NewWave/WaveletEngine.hh"
#include "NewWave/Exceptions.hh"
#include "NewWave/FrequencyBand.hh"
#include "NewWave/Utils.hh"
#include "NewWave/MomentumHelpers.hh"

#include <functional>
#include <map>
#include <cmath>

#include <iostream>

namespace HepMC{
  class GenEvent;
}

namespace NewWave{
  
  using std::size_t;
  using std::map;
  using std::make_pair;
  
  /// A representation of the event in the wavelet domain
  template<typename T, typename momentum_type = Momentum<T> >
  class WaveletEvent{
    
  public:
    
    WaveletEvent(const T &particles,
                 const PixelDefinition &pixelDefn,
                 const WaveletEngine &engine):
    _pixelDefn(pixelDefn),
    _originalPixels(pixelDefn.makeEmptyPixelArray()),
    _engine(engine),
    _doInvert(false),
    _pileUpThreshold(0.){
      
      _init(particles);
    }
    
    /// Return the wavelet coefficients
    /**
     *  Returns the set of wavelet coefficients, including the effect of 
     *  any event processing in the wavelet domain, e.g. noise removal, 
     *  thresholding, level selection.
     *
     *  \return The wavelet coefficients
     */
    const WaveletCoefficients &coefficients()const{return _coefficients();}
    
    const FrequencyBands &frequencyBands()const{
      
      if(_frequencyBands.size() == _pixelDefn.nLevels() * _pixelDefn.nLevels()) return _frequencyBands;
      
      _frequencyBands.clear();
      _bandLookup.clear();
            
      for(const auto &c: _coefficients()){
        int hash = c.frequencyHash(_pixelDefn.nLevels());
        map<int, int>::const_iterator index = _bandLookup.find(hash);
        if(index==_bandLookup.end()){
          index = _bandLookup.insert(make_pair(hash, _frequencyBands.size())).first;
          _frequencyBands.push_back(FrequencyBand(c.yLevel(), c.phiLevel()));
        }
        
        _frequencyBands[index->second].addCoefficient(c);
      }
      
      return _frequencyBands;
    }
    
    /// Return the event as rasterised pixels in rapidity=\f$\phi\f$ space
    /**
     *  Return a copy of the RasterisedEvent that includes the effect of
     *  any processing in the wavelet domain (e.g. noise thresholding).
     *  This is obtained by inverse transforming the set of coefficients.
     *
     *  \return The rasterised event as Pixels
     */
    const PixelArray pixels()const{
      
      if(!_doInvert) return _modifiedPixels;
      
      _modifiedPixels = _engine.inverseTransform(coefficients());
      _doInvert = false;
      return _modifiedPixels;
    }
    
    const PixelArray &originalPixels()const{
      return _originalPixels;
    }
    
    /// Return the particles from the filtered event
    /**
     *  This inverts (if necessary) the wavelet transformation on the 
     *  (possibly filtered) wavelet coefficients, de-rasterises the 
     *  event and returns those particles for which the ratio of
     *  output to input rasterised event is greater than the pile up 
     *  threshold (see WaveletEvent<T>::setPileUpThreshold(double)).
     *
     *  The effect of this is to adjust the momenta of the output 
     *  particles according to the wavelet analysis.  Those particles 
     *  that fail the pile up threshold cut are removed.  By default, 
     *  the pile up threshold is 0, so all particles are kept (barring 
     *  those with negative pT).  The pile up threshold is likely to 
     *  be dependent on the detector used, and should be tuned 
     *  experimentally.
     *
     *  Note that the removal of pile up and the removal of soft 
     *  contributions to the signal event should be performed in 
     *  separate steps - first one should remove pile up, then run a 
     *  second wavelet analysis on the remaining particles in order to 
     *  remove soft activity from the signal.
     *
     *  \return T a list of particles modified by wavelet analysis
     */
    const T &particles()const{
      
      if(!_doInvert) return _modifiedParticles;
      
      _ratio = pixels() / _originalPixels ;
      
      _modifiedParticles.clear();
      
      for(auto p: _originalParticles){
        size_t ybin   = _pixelDefn.yPixelIndex(momentum_type::rapidity(p));
        size_t phiBin = _pixelDefn.phiPixelIndex(momentum_type::phi(p));
        double ratio = _ratio[ybin][phiBin];
        
        momentum_type::update(_modifiedParticles, p, ratio, _pileUpThreshold);
        
      }
      
      _doInvert = false;
      
      return _modifiedParticles;
    }
    
    
    /// De-noise the event using a simple flat threshold
    /**
     *  Event de-noising removes all coefficients whose magnitude is 
     *  below the given threshold.  Note that since different analysis
     *  systems use different units (GeV, MeV...) this threshold must be
     *  set consistently with that.  In other words, if you want a 
     *  threshold of 1GeV and your units are MeV, the argument is 1000.
     *
     *  \param noiseThreshold The threshold for coefficients
     */
    void denoise(double noiseThreshold){
      
      filter([noiseThreshold](const WaveletCoefficient &c)->bool {return (fabs(c.value()) > noiseThreshold); });
      return;
    }

    /// Filter the event by setting the unselected coefficients to zero
    /**
     *  The std::function passed in here selects WaveletCoefficients and 
     *  should return true if they are to be kept, or false if rejected.
     *  This function is applied to all of the coefficients in the event.
     *  Note that it can be called several times, so filters can be 
     *  combined (e.g. de-noising followed by removal of high-frequency 
     *  terms).
     *
     *  The WaveletEvent::denoise method calls this to select only 
     *  coefficients above threshold.  It provides a good example of 
     *  defining a selection function inline using lamdas.  Of course,
     *  you can also use functors or function pointers.
     *
     *  \param select Function that determines whether a single 
     *  coefficient is kept or set to zero.  It should take a 
     *  NewWave::WaveletCoefficient as the sole argument.
     *
     */
    
    void filter(const std::function<bool (const WaveletCoefficient&)> &select){
      
      for(WaveletCoefficient &coeff: _coefficients()){
        if(!select(coeff)){
          coeff.setValue(0.);
          _doInvert = true;
        }
      }
      return;
    }
    
    void setPileUpThreshold(double threshold){
      if(threshold < 0.){
        throw PileUpThresholdException();
      }
      _pileUpThreshold = threshold;
      _doInvert = true;
      return;
    }
    
  private:
    
    WaveletCoefficients &_coefficients()const{
      if(_coeffs.size() != 0) return _coeffs;
      _coeffs = _engine.transform(_originalPixels);
      return _coeffs;
    }
    
    void _init(const T &input){
      
      for(auto p: input){
        if(_pixelDefn.covers(momentum_type::rapidity(p), momentum_type::phi(p))){
          _originalParticles.push_back(p);
          _addParticle(momentum_type::rapidity(p), momentum_type::phi(p), momentum_type::pT(p));
        }
      }
      
    }
    
    void _addParticle(double rapidity, double phi, double pT){
      size_t ybin   = _pixelDefn.yPixelIndex(rapidity);
      size_t phiBin = _pixelDefn.phiPixelIndex(phi);
      _originalPixels[ybin][phiBin] += pT;
      return;
    }
    
    HepMC::GenEvent *_genEvent()const;
    
    PixelDefinition _pixelDefn;
    
    mutable PixelArray _modifiedPixels;
    PixelArray _originalPixels;
    
    T _originalParticles;
    
    const WaveletEngine &_engine;
    
    mutable WaveletCoefficients _coeffs;
    
    mutable FrequencyBands _frequencyBands;
    mutable map<int, int> _bandLookup;
    
    mutable bool _doInvert;
    
    double _pileUpThreshold;
    
    mutable PixelArray _ratio;
    
    mutable T _modifiedParticles;
    
  };
  
  template<>
  HepMC::GenEvent* const &WaveletEvent<HepMC::GenEvent *>::particles()const;
  
  template<>
  void WaveletEvent<HepMC::GenEvent*>::_init(HepMC::GenEvent* const &input);
  
  
}

#endif
