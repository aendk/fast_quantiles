//
// Created by aendk on 20.01.22.
//
#include "exact_quantiles.h"
#include <assert.h>
#include <iostream>
#include <execution>
#include <algorithm>
#include <vector>

using namespace std;
ExactQuantiles::ExactQuantiles() {
}

void ExactQuantiles::add(double val) {
  val_vec_.push_back(val);
  sorted_ = false;
}

void ExactQuantiles::sort() {

  //uses intel thread building blocks
  std::sort(std::execution::par_unseq, val_vec_.begin(), val_vec_.end());
  

  //set sort-status , we assume that a single addition makes a vec unsorted
  sorted_ = true;
}

//check for sorting
bool is_sorted(const vector<double> &arr)
{
    for (size_t i = 0; i < arr.size() - 1; i++)
        if ( ! (arr[i] <= arr[i + 1]) ) 
            return false;
    return true;
}

double ExactQuantiles::get_quantile_value(double quantile) {
  if (quantile < 0.0 || quantile > 1) {
    std::cout << "FAIL: ExactQuantiles.get_quantile_value() called with invalid quantile < 0 or 1 < quantile:" << quantile << std::endl;
    exit(1);
  }

  if(!sorted_) {
    cout << "WARNING slow lookup, needs to sort first" <<  endl;
    sort();
  }

  double position = val_vec_.size() * quantile;
  size_t rounded_pos = static_cast<size_t>(position);
  if (rounded_pos == val_vec_.size()) return val_vec_.back(); // for 100% quantiles
  return val_vec_[rounded_pos];
}
