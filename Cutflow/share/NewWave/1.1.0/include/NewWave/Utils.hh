#ifndef NEWWAVE_UTILS_HH
#define NEWWAVE_UTILS_HH

#include "NewWave/PixelArray.hh"

#include <functional>
#include <algorithm>
#include <assert.h>
#include <cmath>

namespace NewWave{
  
  using std::size_t;
  
  /// Constant defining \f$2\pi\f$
  const double TWOPI = 2.0 * M_PI;
  
  /// Is an integer an even number?
  /**
   * \return true if the input is even
   */
  inline bool isEven(size_t n){ return (n%2 == 0);}
  
  /// Is an integer an odd number?
  /**
   * \return true if the input is odd
   */
  inline bool isOdd(size_t n){return !isEven(n);}
  
  /// Is an integer radix 2 (=2^m)
  /**
   * Interesting fact: radix shares its etymology with radish (a root)
   * \return true if the input is radix 2
   */
  inline bool isRadix2(size_t n){return !((n-1) & n);}
  
  /// Return the nearest radix 2 value above or equal to the input
  inline size_t nearestRadix2(size_t n){
    if(isOdd(n)) ++n;
    // @todo clever way of doing this?
    while(!isRadix2(n)){
      n+=2;
    }
    return n;
  }
  
  /// Convert a 2D array into a row-major single row
  /**
   *  Converts an array into a 1D vector.
   *  Given C/C++ memory management, this is probably unneccessary
   *  as the array will be stored in memory like that.  We do not want 
   *  results that depend on the internal behaviour of the particular 
   *  system's memory management though, so play it safe.
   *
   *  \param array The input 2D array
   *  \return a 1D vector representing the array
   */
  inline vector<double> makeRowFromArray(const PixelArray &array){
    vector<double> rowMajor;
    
    for(PixelArray::const_iterator row = array.begin();
        row != array.end(); ++row){
      
      for(vector<double>::const_iterator el = row->begin();
          el != row->end(); ++el){
        rowMajor.push_back(*el);
      }
    }
    
    return rowMajor;
  }

  inline PixelArray makeArray(const vector<double> &row){
    size_t n = sqrt(row.size());
    assert(n*n == row.size());
    PixelArray output;

    vector<double> line;
    for(double el: row){
      line.push_back(el);
      if(line.size()==n){
        output.push_back(line);
        line.clear();
      }
    }
    
    return output;
  }
  
  /// Divide each element of one vector by the other
  /**
   *  \param left the numerator
   *  \param right the denominator
   *  \return a vector in which each element i is left[i] / right[i]
   */
  inline vector<double> operator / (vector<double> left,
                                    const vector<double> &right){
    
    std::transform(left.begin(), left.end(), right.begin(), left.begin(), std::divides<double>());
    return left;
  }
  
  /// Divide each element of one array by the other

  inline PixelArray operator / (PixelArray left, const PixelArray &right){
    
    std::transform(left.begin(), left.end(), right.begin(), left.begin(),
                   [](vector<double> num, const vector<double> &denom){return num / denom;});
    return left;
  }
  
  
  /// convert an angle into the range \f$0 < \phi < 2\pi\f$
  inline double mod2Pi(double angle){
    if(angle < TWOPI && angle > 0) return angle;
    
    double dPhi = fmod(angle, TWOPI);
    return (angle >0)? dPhi: TWOPI + dPhi;
  }
  
}
#endif
