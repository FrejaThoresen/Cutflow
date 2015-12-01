#ifndef NEWWAVE_WAVELET_ENGINE_HH
#define NEWWAVE_WAVELET_ENGINE_HH

#include "NewWave/WaveletCoefficient.hh"
#include "NewWave/PixelArray.hh"

namespace NewWave {
  
  /// Abstract interface defining methods to perform a wavelet transform
  /**
   *  This interface defines methods that will perform the wavelet transform 
   *  on a sequence of numbers in the PixelArray.  Concrete implementations of 
   *  this class can use e.g. GSL to perform the actual transform
   */
  class WaveletEngine{
    
  public:
    
    /// Perform the wavelet transformation on a 2-D array of pixels
    /**
     *  \param pixels The 2-D array of pixels
     *  \return A list WaveletCoefficients.  No guarantee is made as to their order
     */
    virtual WaveletCoefficients transform(const PixelArray &pixels)const = 0;
    
    /// Perform the inverse wavelet transformation on a list of coefficients
    /**
     *  The inverse wavelet transform takes a list of coefficients and returns 
     *  an array of pixel values in rapidity-\f$\phi\f$ space
     *
     *  \param coefficients The WaveletCoefficients to transform
     *  \return a PixelArray in rapidity-\f$\phi\f$ space
     */
    virtual PixelArray inverseTransform(const WaveletCoefficients &coefficients)const = 0;
    
  private:
    
  };
}

#endif
