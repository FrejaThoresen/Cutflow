#ifndef NEWWAVE_GSL_ENGINE_HH
#define NEWWAVE_GSL_ENGINE_HH

#include <vector>

#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_wavelet2d.h>

#include "NewWave/WaveletEngine.hh"
#include "NewWave/PixelDefinition.hh"

namespace NewWave{
  
  using std::size_t;
  using std::vector;
  
  /// Implementation of the WaveletEngine using the GNU Scientific Library
  class GSLEngine : public WaveletEngine{

  public:
    
    GSLEngine(const gsl_wavelet_type *type, size_t degree,
              const PixelDefinition &pixelDefn);
    
    ~GSLEngine();
    
    WaveletCoefficients transform(const PixelArray &pixels)const;
    
    PixelArray inverseTransform(const WaveletCoefficients &coefficients)const;

    
  private:
    
    void init(size_t nSegments);

    void initYLevels();

    void initPhiLevels();

    /// The wavelet type (Daubechies, Harr...)
    const gsl_wavelet_type *_waveletType;
    
    /// The wavelet degree (i.e. 4 for Daubechies 4)
    size_t _degree;
    
    /// gsl wavelet object
    gsl_wavelet *_wavelet;
    /// workspace for the wavelet transform
    gsl_wavelet_workspace *_workspace;
    
    size_t _nSegments;
    size_t _nSegments2;
    size_t _nLevels;
    
    vector<size_t> _yLevels;
    vector<size_t> _phiLevels;
    
  };

}


#endif
