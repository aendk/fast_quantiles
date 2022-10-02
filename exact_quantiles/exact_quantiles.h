//
// Created by aendk on 20.01.22.
//

#include <vector>


class ExactQuantiles {
public:
  ExactQuantiles();
  ~ExactQuantiles(){}

  void add(double val);
  void sort(); // sorts array multithreaded
  int get_N() const;
  double get_quantile_value(double quantile);

protected:

private:
std::vector<double> val_vec_;
  bool sorted_ = false;

};
