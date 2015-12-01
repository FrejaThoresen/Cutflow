#ifndef NEWWAVE_EXCEPTIONS_HH
#define NEWWAVE_EXCEPTIONS_HH

#include <stdexcept>
#include <string>

namespace NewWave {
  
  using std::string;
  using std::to_string;
  
  /// Thrown when a bad definition of the pixels is used
  class PixelDefinitionException : public std::runtime_error{
    
  public:
    PixelDefinitionException(const string &msg):
    std::runtime_error("Pixel array definition is bad: " +msg){};
    
  };
  
  /// Thrown when a feature is called for which support was not compiled
  /**
   *  Some features (e.g. HepMC support, GSL) require external libraries
   *  to be linked at compile time.  This exception should be thrown if
   *  such a feature is called at runtime without having been compiled
   */
  class NotCompiledException : public std::runtime_error{
    
  public:
    NotCompiledException(const string &msg):
    std::runtime_error("Feature is not compiled: " + msg){};
    
  };
  
  /// Thrown when the length of the PixelArray does not match the expected sequence length in the transforming engine
  class PixelArrayMismatch : public std::runtime_error{
  public:
    PixelArrayMismatch():
    std::runtime_error("The pixel array size does not match."){};
  };
  
  /// Thrown when a GSL wavelet transform call fails
  class GSLException : public std::runtime_error{
  public:
    GSLException():
    std::runtime_error("GSL wavelet transform failed due either to lack of memory allocation, data that is not radix 2 or input that is not an NxN array."){};
    
  };
  
  /// Thrown when a non-valid (i.e. negative) pile up threshold is set
  class PileUpThresholdException : public std::runtime_error{
  public:
    PileUpThresholdException():
    std::runtime_error("Pile up threshold must be positive"){};
  };
  
  /// Thrown when an attempt is made to access an invalid FrequencyBand
  class InvalidFrequencyBand: public std::runtime_error{
  public:
    InvalidFrequencyBand(): std::runtime_error("Invalid number of coefficients in frequency band"){}
  };
  
  /// Thrown when an attempt is made to add a coefficient to a non-matching frequency band
  class WrongCoefficient: public std::runtime_error{
  public:
    WrongCoefficient(int y, int phi, int expectedY, int expectedPhi): std::runtime_error("Wavelet levels of coefficient do not match the frequency band: coefficient = {" + to_string(y) + "," + to_string(phi) + "}, expected = {" + to_string(expectedY) + ", " + to_string(expectedPhi) + "}"){}
  };
  
}

#endif
