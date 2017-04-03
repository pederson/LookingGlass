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

};


#endif