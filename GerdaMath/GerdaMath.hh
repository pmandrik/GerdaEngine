// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaMath_H
#define GerdaMath_H 1

namespace ge {
  template <class T> int check_sign(T a){ if(a>0.){return 1;} if(a<0.){return -1;} return 0; }
  template <class T> bool check_in(T a, T min, T max){ return (a<min or a>max) ? false : true;}
};

#include "GerdaMathConst.hh"
#include "GerdaRandom.hh"
#include "GerdaAlgo.hh"

#endif
