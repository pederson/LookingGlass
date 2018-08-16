#ifndef _COLORRAMP_H
#define _COLORRAMP_H

#include <stdlib.h>
#include <iostream>

enum class CRamp : char {
      MULTI_HUE_RED_1, MULTI_HUE_RED_2, MULTI_HUE_ORANGE, MULTI_HUE_GREEN_1, MULTI_HUE_GREEN_2,
      MULTI_HUE_BG_1, MULTI_HUE_BG_2, MULTI_HUE_BG_3, MULTI_HUE_BLUE, MULTI_HUE_PURPLE,
      MULTI_HUE_PINK_1, MULTI_HUE_PINK_2,
      DIVERGENT_1, DIVERGENT_2, DIVERGENT_3, DIVERGENT_4, DIVERGENT_5, DIVERGENT_6,
      DIVERGENT_7, DIVERGENT_8, DIVERGENT_9,
      QUALITATIVE_1, QUALITATIVE_2, QUALITATIVE_3, QUALITATIVE_4,
      MATLAB_PARULA, MATLAB_JET, MATLAB_HSV, MATLAB_HOT, MATLAB_COOL,
      MATLAB_SPRING, MATLAB_REDBLUE, MATLAB_SUMMER, MATLAB_AUTUMN, MATLAB_WINTER,
      MATLAB_GRAY, MATLAB_BONE, MATLAB_COPPER, MATLAB_PINK, 
      MATLAB_LINES, MATLAB_COLORCUBE, MATLAB_PRISM, MATLAB_FLAG,
      END_OF_LIST};


struct rgb{
  float R;
  float G;
  float B;
};


class ColorRamp{
public:

  ColorRamp();
  ~ColorRamp();

  void cycle_ramp();
  void set_ramp(CRamp ramp_name);
  void set_interpolation(bool interp) {_rgb_interpolation = interp;};
  rgb get_ramp_color(float norm_value) const;
  unsigned int ramp_size() const {return _num_values;};
  const rgb* ramp() const {return _ramp;};

private:

  rgb * _ramp;
  CRamp _current_ramp;
  unsigned int _rgb_max;
  unsigned int _num_values;
  bool _rgb_interpolation;

  void reset_ramp_array();

};

class Color{
public:

  static rgb Red() {return {1.0, 0.0, 0.0};};
  static rgb Blue() {return {0.0, 0.0, 1.0};};;
  static rgb Green() {return {0.0, 1.0, 0.0};};;
  static rgb Orange() {return {1.0, 128.0/255.0, 0.0};};;
  static rgb Yellow() {return {1.0, 1.0, 0.0};};;
  static rgb Violet() {return {127.0/255.0, 0.0, 1.0};};;
  static rgb Pink() {return {1.0, 102.0/255.0, 1.0};};;
  static rgb White() {return {1.0, 1.0, 1.0};};;
  static rgb Black() {return {0.0, 0.0, 0.0};};;
  static rgb Brown() {return {102.0/255.0, 51.0/255.0, 0.0};};;




  // some blues
  static rgb AliceBlue() {return {240.0/255.0, 248.0/255.0, 255.0/255.0};};
  static rgb Lavender() {return {230.0/255.0, 230.0/255.0, 250.0/255.0};};
  static rgb PowderBlue() {return {176.0/255.0, 224.0/255.0, 230.0/255.0};};
  static rgb LightBlue() {return {173.0/255.0, 216.0/255.0, 230.0/255.0};};
  static rgb LightSkyBlue() {return {135.0/255.0, 206.0/255.0, 250.0/255.0};};
  static rgb SkyBlue() {return {135.0/255.0, 206.0/255.0, 235.0/255.0};};
  static rgb DeepSkyBlue() {return {0.0/255.0, 191.0/255.0, 255.0/255.0};};
  static rgb LightSteelBlue() {return {176.0/255.0, 196.0/255.0, 222.0/255.0};};
  static rgb DodgerBlue() {return {30.0/255.0, 144.0/255.0, 255.0/255.0};};
  static rgb CornflowerBlue() {return {100.0/255.0, 149.0/255.0, 237.0/255.0};};
  static rgb SteelBlue() {return {70.0/255.0, 130.0/255.0, 180.0/255.0};};
  static rgb CadetBlue() {return {95.0/255.0, 158.0/255.0, 160.0/255.0};};
  static rgb MediumSlateBlue() {return {123.0/255.0, 104.0/255.0, 238.0/255.0};};
  static rgb SlateBlue() {return {106.0/255.0, 90.0/255.0, 205.0/255.0};};
  static rgb DarkSlateBlue() {return {72.0/255.0, 61.0/255.0, 139.0/255.0};};
  static rgb MediumBlue() {return {0.0/255.0, 0.0/255.0, 205.0/255.0};};
  static rgb RoyalBlue() {return {65.0/255.0, 105.0/255.0, 225.0/255.0};};
  static rgb DarkBlue() {return {0.0/255.0, 0.0/255.0, 139.0/255.0};};
  static rgb Navy() {return {0.0/255.0, 0.0/255.0, 128.0/255.0};};
  static rgb MidnightBlue() {return {25.0/255.0, 25.0/255.0, 112.0/255.0};};
  static rgb BlueViolet() {return {138.0/255.0, 43.0/255.0, 226.0/255.0};};
  static rgb Indigo() {return {75.0/255.0, 0.0/255.0, 130.0/255.0};};



};


#endif