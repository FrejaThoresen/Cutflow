#ifndef NEWWAVE_PIXEL_DEFINITION_HH
#define NEWWAVE_PIXEL_DEFINITION_HH

#include <vector>

#include "NewWave/PixelArray.hh"

namespace NewWave{
  
  using std::size_t;
  using std::vector;
  
  ///  Defines the range and coordinates of the "pixels" in the rasterised event
  
  /**
   *  The rasterised event representation requires a definiton of the 
   *  pixel ranges and positions.  This class divides a rapidity and \f$\phi\f$
   *  range into a 2-D array of bins
   */
  class PixelDefinition{
    
  public:
    
    /// Constructor using equal no. of bins in y and \f$\phi\f$ and full \f$\phi\f$ coverage
    /**
     *  This will use an equal number of pixels in both the rapidity and
     *  \f$\phi\f$ direction, together with full \f$2\pi\f$ coverage in 
     *  \f$\phi\f$.  The rapidity range is symmetrical around zero, with 
     *  the maximum absolute value specified.
     *
     *  If the specified bumber of bins is not radix two, then the 
     *  rapidity range will need to be padded to allow an increase in the
     *  number of bins.  While this will preserve the size of the bins in 
     *  the rapidity direction, it will lead to smaller size in the 
     *  \f$\phi\f$ direction.  This behaviour must be explicitly requested
     *  using the allowPadding argument, which defaults to false
     *
     *  \param nBins The number of pixel bins
     *  \param yMax The rapidity range spans from -yMax to yMax
     *  \param allowPadding Allow padding, default to no
     */
    PixelDefinition(size_t nBins, double yMax, bool allowPadding=false);
    
    /// Constructor using \f$\phi\f$ and rapidity ranges
    /**
     *  Constructor defining the \f$\phi\f$ and rapidity spans
     *
     *  This will pad the y and \f$\phi\f$ ranges to allow the output raster
     *  array to be square.  While this is not a problem for the y 
     *  direction, it makes less sense in phi, due to the periodicity. 
     *  There is therefore a switch, \param allowPhiPadding, that must 
     *  be turned on to enable such padding.  By default, it will throw
     *  an exception if such padding is necessary.
     *
     *  \param nBinsPhi The number of bins in the \f$\phi\f$ direction
     *  \param phiMin The minimum of the \f$\phi\f$ range
     *  \param phiMax The maximum of the \f$\phi\f$ range
     *  \param nBinsY The number of bins in the rapidity direction
     *  \param yMin The minimum of the rapidity range
     *  \param yMax The maximum of the rapidity range
     */
    PixelDefinition(size_t nBinsPhi, double phiMin, double phiMax,
                    size_t nBinsY, double yMin, double yMax,
                    bool allowPhiPadding=false);
    
    /// Return a square array of pixels of appropriate dimension
    /**
     *  The pixel array must be square, and have sides of length that is
     *  radix 2.  This method returns such an array that guarantees to 
     *  be at least as big as the input phi and y ranges.
     *
     *  \return a PixelArray of the appropriate size
     */
    PixelArray makeEmptyPixelArray()const;
    
    /// Return the index of the pixel in which a given \f$\phi\f$ value lies
    size_t phiPixelIndex(double phi)const;
    
    /// Return the index of the pixel in which a given y value lies
    size_t yPixelIndex(double y)const;
    
    /// Return the number of pixels along the y and \f$\phi\f$ axes
    size_t nPixels()const;
    
    /// Return the minimum of the rapidity covered by this PixelDefinition
    double yMin()const;

    /// Return the maximum of the rapidity covered by this PixelDefinition
    double yMax()const;
    
    /// Returns true if the given rapidity and phi values are inside the covered ranges
    /**
     *  \param y The rapidity of a particle 
     *  \param phi The \f$\phi\f$ of a particle
     *
     *  \return true if the particle is covered by this PixelDefinition
     */
    bool covers(double y, double phi)const;
    
    /// Returns the number of wavelet levels
    /**
     *  The number of wavelet levels includes the smoothing coefficient.  
     *  Increasing the number of levels by one doubles the number of
     *  wavelet coefficients.
     *
     *  \return The number of wavelet levels
     */
    size_t nLevels()const;
    
  private:
    
    size_t _nBinsPhi;
    size_t _nBinsY;
    size_t _nBins;

    size_t _padPhi;
    size_t _padY;
    
    mutable double _rangePhi;
    mutable double _rangeY;
    
    double _phiToBin;
    double _yToBin;
    
    double _phiMin;
    double _yMin;
    
    double _phiMax;
    double _yMax;
    
    double _phi0;
    double _y0;
    
    mutable size_t _nLevels;
    mutable bool _gotLevels;
    
    vector<double> _phiAxis;
    vector<double> _yAxis;
    
  };
}

#endif
