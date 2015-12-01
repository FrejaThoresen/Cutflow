#ifndef NEWWAVE_FREQUENCY_BAND_HH
#define NEWWAVE_FREQUENCY_BAND_HH

#include "NewWave/WaveletCoefficient.hh"

#include <vector>

namespace NewWave {
  
  using std::vector;
  
  /// A frequency band contains all the wavelet coefficients of the same angular scale
  /**
   *  For each pair of wavelet levels (frequencies) there is a fixed 
   *  number of wavelet coefficients that cover the full \f$y-\phi\f$
   *  space.  A frequency band contains the full set of coefficients 
   *  for its \f$y-\phi\f$ levels, each coefficient having an identical
   *  angular scale, but covering a different location in \f$ y-\phi\f$
   */
  class FrequencyBand{
    
  public:
    
    FrequencyBand(int yLevel, int phiLevel);
    
    /// Return the coefficients in this frequency band
    /**
     *  Will throw an InvalidFrequencyBand if the set of coefficients is
     *  not complete
     */
    const WaveletCoefficients &coefficients()const;
    
    /// Return the rms of the coefficients in this frequency band
    /**
     *  Since a wavelet coefficient represents the deviation from the 
     *  mean activity in its location, the average of all coefficients
     *  within a frequency band is zero by definition.  The RMS 
     *  therefore provides a measure of the amount of activity in a 
     *  frequency band and is conceptually similar to the magnitude of 
     *  a Fourier coefficient in a Fourier decomposition
     */
    double rms()const;
    
    /// Add a wavelet coefficient to this band
    /**
     *  Throw an InvalidFrequencyBand if the frequency band is already 
     *  fully populated, or a WrongCoefficient if the coefficient to be 
     *  added does not match the levels of the frequency band
     */
    void addCoefficient(const WaveletCoefficient &coeff);
    
    /// The y-level of this frequency band
    int yLevel()const;
    
    /// The \f$\phi\f$-level of this frequency band
    int phiLevel()const;
    
  private:

    int _nCoeffs;
    int _yLevel;
    int _phiLevel;
    
    mutable double _rms;
    
    WaveletCoefficients _coefficients;
    
  };
  
  typedef vector<FrequencyBand> FrequencyBands;

  
}

#endif
