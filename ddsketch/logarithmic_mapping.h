//
// Created by aendk on 04.12.21.
//
#include <limits>
#ifndef DATA_SUMMARIES_MAPPING_H
#define DATA_SUMMARIES_MAPPING_H

/*
 * Adaptation from the ddsketch-source code.
 * a normal sketch calls logarithmic mapping, which calls KeyMapping.
 * This file merges both, as I am just needing these two.
 */

class Logarithmic_mapping {

public:
  Logarithmic_mapping();
  Logarithmic_mapping(double relative_accuracy, double offset = 0.0);
  ~Logarithmic_mapping();

  //funcs from base class keymapping
  long key(double value);
  double value(long key);

  //funcs from sub-class logarithmic_map
  double log_gamma(double value);
  double pow_gamma(double value);

  double relative_accuracy_;
  double offset_;
  double gamma_;
  double multiplier_;
  double max_possible_;
  double min_possible_;

protected:

private:
};


#endif //DATA_SUMMARIES_MAPPING_H
