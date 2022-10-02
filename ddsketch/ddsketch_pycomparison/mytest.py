from ddsketch import DDSketch
import numpy as np
from timeit import default_timer as timer

"""
Small testing suite to check validity of C++ DDSketch against original python-DDSketch
"""

# basic test of add and get_qv
def baseline1():
  sketch = DDSketch()
  values = [0.82237959, -0.61125007, 0.4383297, 0.63498452, 1.30493411]
  print("MAIN:values entered: {}".format(values))
  for v in values:
    print("MAIN: adding value {}  to sketch".format(v))
    sketch.add(v)
    print("Main-printsketch: {}".format(sketch))

  quantiles = [sketch.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  print("Quantiles here: {}".format(quantiles))


def readfile(file):
  # TODO input a working path.
  # file = "/dataset_generators/uniform_dist_[0,1[_1000_samples.txt"
  f = open(file, "r")
  file_in_mem = f.read()
  removed_brackets = file_in_mem[1:]  # slices of first
  removed_brackets = removed_brackets[:-1]  # slices of last
  tokenized = removed_brackets.split(" ")
  for e in range(0, len(tokenized)):
    tokenized[e] = float(tokenized[e].strip())

  return tokenized


# read from file, test add and get_qv
def baseline2():

  # TODO input a working path.
  file = "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[-1,1[_90000000_samples.txt"
  values = readfile(file)
  print("file ingest done")
  sketch = DDSketch()
  #add list to ddsketch
  b4_ts = timer()
  for v in values:
    # print("MAIN: adding value {}  to sketch".format(v))
    sketch.add(v)
    # print("Main-printsketch: {}".format(sketch))
  after_ts = timer()
  print("Ingest/add Time taken: {}".format(after_ts - b4_ts))
  print(" adding done")
  #eval the common quantiles
  qv_start_ts = timer()
  quantiles = [sketch.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  qv_end_ts = timer()
  print("Quantiles here: {}".format(quantiles))
  print("Quantile value Time taken: {}".format(qv_end_ts - qv_start_ts))

def baseline3():

  # TODO input a working path.
  file = "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[0,1[_2000000_samples.txt"
  file2 = "/home/andreas/git/Data_Summaries/project/dataset_generators/uniform_dist_[-1,1[_1000000_samples.txt"
  values1 = readfile(file)
  values2 = readfile(file2)
  print("file ingest done")
  sk1 = DDSketch()
  sk2 = DDSketch()
  #add list to ddsketch
  b4_ts = timer()
  for v in values1:
    sk1.add(v)
  for v in values2:
    sk2.add(v)
  after_ts = timer()
  print("Time taken: {}".format(after_ts - b4_ts))
  print(" adding done")
  #eval the common quantiles
  qv1 = [sk1.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  qv2 = [sk2.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  print("Quantiles here: qv1={}\nqv2={}".format(qv1, qv2))
  sk1.merge(sk2)
  qv3 = [sk1.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  print("merged Quantiles here: qv3={}".format(qv3))


def simple_merge_test1():
  sk1 = DDSketch()
  sk2 = DDSketch()
  values = [0.82237959, -0.61125007, 0.4383297, 0.63498452, 1.30493411]
  values2 = [0.82237959 + 1.0, -0.61125007 - 1.0, 0.4383297 + 1.0, 0.63498452 + 1.0, 1.30493411 - 1.0]

  for e in range(len(values)):
    sk1.add(values[e])
    sk2.add(values2[e])

  qv1 = [sk1.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  qv2 = [sk2.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  print("Quantiles here: \nsk1={} \n sk2={}".format(qv1, qv2))
  sk1.merge(sk2)
  qv3 = [sk1.get_quantile_value(q) for q in [0.5, 0.75, 0.9, 0.95, 0.99, 1.0]]
  print("merged Quantiles here: \nsk3={} ".format(qv3))

def main():
  abc = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  a2v = [0, 4, 4, 4, 5, 4, 7, 4, 4, 0]
  a2v = abc[:]
  print("abc = {}\na2v={}".format(abc, a2v))

  # TODO select a test.
  # simple_merge_test1()
  # baseline1()
  baseline2()
  # baseline3()


main()
