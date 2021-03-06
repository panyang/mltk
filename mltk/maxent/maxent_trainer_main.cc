// Copyright (c) 2013 MLTK Project.
// Author: Lifeng Wang (ofandywang@gmail.com)

#include "mltk/maxent/maxent.h"

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "common/base/string/algorithm.h"
#include "mltk/common/instance.h"

DEFINE_string(train_data_file, "", "the filename of training data.");
DEFINE_string(model_file, "", "the filename of maxent model.");
DEFINE_string(optim_method, "LBFGS",
              "the optimization method, LBFGS, OWLQN, or SGD.");
DEFINE_double(l1_reg, 0.0, "the L1 regularization.");
DEFINE_double(l2_reg, 0.0, "the L2 regularization.");
DEFINE_int32(num_heldout, 0, "the number of heldout data.");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Initialize MaxEnt.";
  mltk::maxent::MaxEnt maxent;
  if (FLAGS_optim_method == "LBFGS") {
    maxent.UseLBFGS();
  } else if (FLAGS_optim_method == "OWLQN") {
    maxent.UseOWLQN();
  } else if (FLAGS_optim_method == "SGD") {
    maxent.UseSGD();
  } else {
    LOG(FATAL) << "Invalid optimization method : " << FLAGS_optim_method;
  }
  if (FLAGS_l1_reg > 0.0) {
    maxent.UseL1Reg(FLAGS_l1_reg);
  }
  if (FLAGS_l2_reg > 0.0) {
    maxent.UseL2Reg(FLAGS_l2_reg);
  }

  LOG(INFO) << "Load training data from " << FLAGS_train_data_file;
  std::ifstream fin(FLAGS_train_data_file.c_str());
  if (!fin) {
    LOG(ERROR) << "Can't open train_data file '" << FLAGS_train_data_file
        << "'";
    return -1;
  }

  std::vector<mltk::common::Instance> instances;
  std::string line;
  while (std::getline(fin, line)) {
    mltk::common::Instance instance;
    if (instance.ParseFromText(line)) {
      instances.push_back(instance);
    }
  }
  fin.close();

  LOG(INFO) << "MaxEnt model training.";
  maxent.Train(instances, FLAGS_num_heldout);

  LOG(INFO) << "Save model to " << FLAGS_model_file;
  maxent.SaveModel(FLAGS_model_file);

  return 0;
}
