#ifndef NEWWAVE_MOMENTUM_HELPERS_HH
#define NEWWAVE_MOMENTUM_HELPERS_HH

#include <type_traits>

namespace NewWave{
  
  using std::enable_if;
  using std::is_member_function_pointer;
  
  template <typename T>
  struct has_momentum {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::momentum)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };

  template <typename T>
  struct has_setMomentum {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::setMomentum)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };
  
  
  template <typename T>
  struct has_rapidity {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::rapidity)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };

  template <typename T>
  struct has_Rapidity {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::Rapidity)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };
  
  template <typename T>
  struct has_phi {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::phi)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };

  template <typename T>
  struct has_Phi {
    
    template <typename U>
    static constexpr bool
    _value(typename enable_if<is_member_function_pointer<decltype(&U::Phi)>::value, U*>::type){
      return true;
    }
    
    template <typename U>
    static constexpr bool
    _value(...){
      return false;
    }
    
    static constexpr bool value = _value<T>(0);
  };
  
  template<typename T>
  double pT(const T &p){
    return p.Pt();
  }
  
  template<typename T>
  typename enable_if<has_rapidity<T>::value, double>::type
  rapidity(const T &p){
    return p.rapidity();
  }

  template<typename T>
  typename enable_if<has_Rapidity<T>::value &&
  !has_rapidity<T>::value, double>::type
  rapidity(const T &p){
    return p.Rapidity();
  }
  
  template<typename T>
  typename enable_if<has_momentum<T>::value &&
  !has_Rapidity<T>::value &&
  !has_rapidity<T>::value, double>::type
  rapidity(const T &p){
    return rapidity(p.momentum());
  }
  
  template<typename T>
  typename enable_if<has_phi<T>::value, double>::type
  phi(const T &p){
    return p.phi();
  }
  
  template<typename T>
  typename enable_if<has_Phi<T>::value &&
  !has_phi<T>::value, double>::type
  phi(const T &p){
    return p.Phi();
  }
  
  template<typename T>
  typename enable_if<has_momentum<T>::value &&
  !has_Phi<T>::value &&
  !has_phi<T>::value, double>::type
  phi(const T &p){
    return phi(p.momentum());
  }
  
  
  template<typename T>
  typename enable_if<has_momentum<T>::value &&
  has_setMomentum<T>::value>::type
  scaleMomentum(double scale, T &p){
    p.setMomentum(p.momentum() * scale);
    return;
  }
  
  template<typename T>
  typename enable_if<!has_momentum<T>::value>::type
  scaleMomentum(double scale, T &p){
    p *= scale;
    return;
  }
  
  template<typename T>
  class Momentum {
    
  public:
    
    using element_type = typename T::value_type;
    
    inline static double rapidity(const element_type &p){
      return NewWave::rapidity(p);
    }
    
    inline static double pT(const element_type &p){
      return NewWave::pT(p);
    }
    
    inline static double phi(const element_type &p){
      return NewWave::phi(p);
    }
    
    /// Update a container of type T with a modified particle
    /**
     *  This decides whether a particle should be kept, and if so modifies
     *  it according to the wavelet filtering and inserts it into the 
     *  container of particles
     *
     *  Providing a different version of this function allows the logic
     *  determining what to do with rejected particles to be changed. 
     *  Some frameworks require the rejected particles to be zeroed, 
     *  rather than discarded.
     *
     *  \param toModify the list of particles to modify.  Normally the 
     *         resulting particles are simply pushed_back into this 
     *         container
     *  \param particle the input particle to test, modify and store
     *  \param ratio the ratio of before/after filtering, which 
     *         determines whether a particle is kept and how it is 
     *         modified
     *  \param threshold the threshold for (pile up) rejection.  
     *         Typically, if the ratio is below threshold, a particle is
     *         rejected
     */
    
    inline static void update(T &toModify, element_type &particle, double ratio, double threshold){
      if(ratio > threshold){
        NewWave::scaleMomentum(ratio, particle);
        toModify.push_back(particle);
      }
      
      return;
    }
    
  };
  
}

#endif


