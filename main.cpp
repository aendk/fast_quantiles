//
// Created by aendk on 15.11.21.
//

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "pthread.h"
#include <unistd.h>
#include "ddsketch/ddsketch.h"
#include "exact_quantiles/exact_quantiles.h"
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

// fills a vector of a given size with reproducible uniformly distributed random numbers
void generate_uniform_dataset(std::vector<double> &values ) {
  std::chrono::steady_clock::time_point begin_gen = std::chrono::steady_clock::now();

  std::default_random_engine eng;
  int start = -1;
  int end = 1;
  std::uniform_real_distribution<double> urd(start, end);

  // populate vector without random seed for reproducability
  for (size_t e = 0; e < values.size(); e++) values[e] = urd(eng);
  std::chrono::steady_clock::time_point end_gen = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_gen - begin_gen).count();
  auto s  = std::chrono::duration_cast<std::chrono::seconds>(end_gen - begin_gen).count();
  cout << "Dataset generation finished, size=" << values.size() << " range  [" << start << ", " << end <<  "]"
       << "\nTime taken:" << ms << "[ms] " << s << "[s]" << endl;
}

// read dataset from file created by python. Slow ingest, use for validation between python/c++ only
void readfile(std::vector<double> &values, string file) {
//  string file = "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[0,1[_1000_samples.txt";
//  string file = "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[0,1[_2000000_samples.txt";
  ifstream infile(file);
  string token;
  vector<string> token_vec;
  while ( infile >> token) {
    token_vec.push_back(token);
  }

  token_vec[0] = token_vec[0].substr(1,token_vec[0].size());
  token_vec[token_vec.size()-1] = token_vec[token_vec.size()-1].substr(0,token_vec[0].size() - 1);

  for (size_t e = 0; e < token_vec.size(); e++) {
    string elem = token_vec[e];
    for (int j = 0; j < elem.size(); j++) {
      if (token_vec[e][j] == '\n') {
        cout << "CUT IT" << endl;
        token_vec[e] = token_vec[e].substr(0, token_vec[e].size() - 1);
      }
    }
//    cout << " adding " << token_vec[e] << endl;
    values.push_back(stod(token_vec[e]));
  }
  cout << "done reading file; size of resulting vector=" << values.size() << endl;
}

string check_qvs(DDsketch& sk) {
  std::vector<double> qvs {0.5, 0.75, 0.9, 0.95, 0.99, 1.0};
  std::string qv_str = "[";
  for (auto& e : qvs) {
    double qv = sk.get_quantile_value(e);
    qv_str.append(std::to_string(qv));
    qv_str.append(", ");
  }
  qv_str.pop_back();
  qv_str.pop_back();
  qv_str.append("]");
  return qv_str;

}

string check_qvs(ExactQuantiles& eq) {
  std::vector<double> qvs {0.5, 0.75, 0.9, 0.95, 0.99, 1.0};
  std::string qv_str = "[";
  for (auto& e : qvs) {
    double qv = eq.get_quantile_value(e);
    qv_str.append(std::to_string(qv));
    qv_str.append(", ");
  }
  qv_str.pop_back();
  qv_str.pop_back();
  qv_str.append("]");
  return qv_str;

}

//test get_quantile_value for ExactQuantiles
void eq_qv() {

  std::vector<double> values1{1.0, 2.0, 3.0, 4.0, 5.0}; // => ??? GB in memory
  ExactQuantiles eq  = ExactQuantiles();
  for(auto & e : values1) eq.add(e);
  eq.sort();
  cout << "eq-qv=" << eq.get_quantile_value(0.5) << endl;
}

//function called for threads to add vals to sketch
void t_add(int t_id, DDsketch* sk, vector<double> *vals_to_add, size_t start, size_t stop) {

  for(size_t e = start; e < stop; e++) sk->add((*vals_to_add)[e]);

}

//compares calculated quantiles between ddsketch and ExactQuantiles
void cmd_ddsketch_eq() {

  std::vector<double> values1(999990);
  generate_uniform_dataset(values1);
  ExactQuantiles eq  = ExactQuantiles();
  DDsketch sk1 = DDsketch();
  for (auto& elem: values1) {
    sk1.add(elem);
    eq.add(elem);
  }
  eq.sort();
  string qv_str_dd = check_qvs(sk1);
  string qv_str_eq = check_qvs(eq);

  cout << "ddsketch:: " << qv_str_dd << "\n exactq:: " << qv_str_eq;

}

void baseline1() {
  DDsketch sk1 = DDsketch();

  std::vector<double> values { 0.82237959, -0.61125007, 0.4383297, 0.63498452, 1.30493411};
  for (auto& e : values) {
    std::cout << " adding elem: " << e << "\n";
    sk1.add(e);
    std::cout << " adding done of elem: " << e << "\n";
  }

  std::vector<double> qvs {0.5, 0.75, 0.9, 0.95, 0.99, 1.0};
  std::string qv_str = "[";
  for (auto& e : qvs) {
    double qv = sk1.get_quantile_value(e);
    qv_str.append(std::to_string(qv));
    qv_str.append(", ");
  }
  qv_str.pop_back();
  qv_str.pop_back();
  qv_str.append("]");

  std::cout << "quantiles here: qv=" << qv_str << std::endl;
}

void baseline2() {
  DDsketch sk1 = DDsketch();
  std::vector<double> values;
//  readfile(values, "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[-1,1[_90000000_samples.txt");
  readfile(values, "dataset_generators/uniform_dist_[-1,1[_10_samples.txt");

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  for (auto& e : values) sk1.add(e);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::vector<double> qvs {0.5, 0.75, 0.9, 0.95, 0.99, 1.0};
  std::string qv_str = "[";
  std::chrono::steady_clock::time_point qvbegin = std::chrono::steady_clock::now();
  for (auto& e : qvs) {
    double qv = sk1.get_quantile_value(e);
    qv_str.append(std::to_string(qv));
    qv_str.append(", ");
  }
  std::chrono::steady_clock::time_point qvend = std::chrono::steady_clock::now();
  qv_str.pop_back();
  qv_str.pop_back();
  qv_str.append("]");

  std::cout << "quantiles here: qv=" << qv_str << std::endl;
  std::cout << "Ingest/add time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms], " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
  std::cout << "quantile value time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(qvend - qvbegin).count() << "[ms], " << std::chrono::duration_cast<std::chrono::milliseconds>(qvend - qvbegin).count() << "[µs]" << std::endl;

}

void simple_merge_test1() {

  DDsketch sk1 = DDsketch();
  DDsketch sk2 = DDsketch();

  std::vector<double> values {        0.82237959,       -0.61125007,       0.4383297,       0.63498452,       1.30493411};
  std::vector<double> values2 { 0.82237959 + 1.0, -0.61125007 - 1.0, 0.4383297 + 1.0, 0.63498452 + 1.0, 1.30493411 - 1.0};
  for (int e = 0; e < values.size(); e++) {
    std::cout << " sk1 adding elem: " << values[e] << "\n";
    sk1.add(values[e]);
    sk2.add(values2[e]);
    std::cout << " sk1 adding done of elem: " << values[e] << "\n";
  }
  string qv_str_1 = check_qvs(sk1);
  string qv_str_2 = check_qvs(sk2);

  std::cout << "quantiles here: \nqv1=" << qv_str_1 << "\nqv2=" << qv_str_2 << std::endl;
  sk1.merge(sk2);
  string qv_str_3 = check_qvs(sk1);
  std::cout << "merged quantiles here: \nqv3=" << qv_str_3 << std::endl;
}

// tests merging of two datasets
void baseline3(){

  string file1 = "dataset_generators/uniform_dist_[0,1[_200000_samples.txt";
  string file2 = "dataset_generators/uniform_dist_[-1,1[_100000_samples.txt";
  DDsketch sk1 = DDsketch();
  DDsketch sk2 = DDsketch();
  std::vector<double> values1, values2;
  readfile(values1, file1);
  readfile(values2, file2);
  for (auto& e : values1) sk1.add(e);
  for (auto& e : values2) sk2.add(e);


  string qv_str_1 = check_qvs(sk1);
  string qv_str_2 = check_qvs(sk2);

  std::cout << "quantiles here: \nqv1=" << qv_str_1 << "\nqv2=" << qv_str_2 << std::endl;
  ///*
  sk1.merge(sk2);
  string qv_str_3 = check_qvs(sk1);
  std::cout << "merged quantiles here: \nqv3=" << qv_str_3 << std::endl;
}

//baseline3 but just faster, with several threads
void baseline4_threaded() {

    // TODO either use a file or generate random values on the fly (faster)
//  string file = "/dataset_generators/uniform_dist_[-1,1[_599999990_samples.txt";
//  readfile(values1, file);

//  std::vector<double> values1(599999990); //=> 4.5 GB in memory
//  std::vector<double> values1(799999990); //=> 6 GB in memory
  std::vector<double> values1(999999999); //=> 7.4 GB in memory
  generate_uniform_dataset(values1);

  int num_threads = 4;
  vector<DDsketch> sketch_vec(num_threads);

  vector<std::thread> threads;
  size_t range = values1.size() / num_threads;
  size_t start_range = 0, end_range = range;
  std::chrono::steady_clock::time_point begin_add = std::chrono::steady_clock::now();
  for (int e = 0; e < num_threads; e++) {
    if (e == num_threads - 1) end_range = values1.size(); // to account for oddly-sized datasets
    threads.push_back(std::thread(t_add, e, &sketch_vec[e], &values1, start_range, end_range));
    start_range += range;
    end_range += range;
  }
  for (auto & t: threads) t.join();
  std::chrono::steady_clock::time_point end_add = std::chrono::steady_clock::now();
  /**
   *  Initially, we thought about fast greedy merging. Sketches would be merged once they've finished ingesting.
   *  The last sketches merged would be the stragglers.
   *  Our profiling results however indicate that merging is irrelevant compared to the rest of the workload.
   *  So we did not optimize the merging process further.
   */
  std::chrono::steady_clock::time_point begin_merge = std::chrono::steady_clock::now();
  sketch_vec[0].merge(sketch_vec[1]);
  sketch_vec[2].merge(sketch_vec[3]);
  sketch_vec[0].merge(sketch_vec[2]);
  std::chrono::steady_clock::time_point end_merge = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point qvbegin = std::chrono::steady_clock::now();
  string qv_str_3 = check_qvs(sketch_vec[0]);
  std::chrono::steady_clock::time_point qvend = std::chrono::steady_clock::now();

  std::cout << "merged quantiles here: \nqv3=" << qv_str_3 << std::endl;
  std::cout << "Ingest/ADD time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_add - begin_add).count() << "[ms], " << std::chrono::duration_cast<std::chrono::microseconds>(end_add - begin_add).count() << "[µs]" << std::endl;
  std::cout << "Ingest/MERGE time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - begin_merge).count() << "[ms], " << std::chrono::duration_cast<std::chrono::microseconds>(end_merge - begin_merge).count() << "[µs]" << std::endl;
  std::cout << "quantile value time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(qvend - qvbegin).count() << "[ms], " << std::chrono::duration_cast<std::chrono::milliseconds>(qvend - qvbegin).count() << "[µs]" << std::endl;
}




int main() {
    // All tests are invoked like this, parameters are hardcoded.
    baseline4_threaded();
  return 0;
}
