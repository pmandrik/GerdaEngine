// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef GerdaRandom_H
#define GerdaRandom_H 1

namespace ge {
  static default_random_engine rengine;

  int randint(int start, int end){
    uniform_int_distribution<int> distribution(start, end);
    return distribution(rengine);
  };

  int randrange(int start, int end, int step = 1){
    uniform_int_distribution<int> distribution(0, (end-start)/step);
    return start+distribution(rengine)*step;
  };

  int randone(){ return randrange(-1, 2, 2);};

  float uniform(float start, float end){
    uniform_real_distribution<float> distribution(start,end);
    return distribution(rengine);
  };

  v2 randv2(int start, int end){
    return v2(randint(start, end), randint(start, end));
  };

  v2 randv2(int startX, int endX, int startY, int endY){
    return v2(randint(startX, endX), randint(startY, endY));
  };
};

#endif 
