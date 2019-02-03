/**
 * @file sparse_svm.cpp
 * @author Ayush Chamoli
 *
 * Implementation of Sparse SVM.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_SPARSE_SVM_SPARSE_SVM_IMPL_HPP
#define MLPACK_METHODS_SPARSE_SVM_SPARSE_SVM_IMPL_HPP

// In case it hasn't been included yet.
#include "sparse_svm.hpp"

namespace mlpack {
namespace svm {

template <typename MatType>
template <typename OptimizerType>
SparseSVM<MatType>::SparseSVM(
    const MatType& data,
    const arma::Row<size_t>& labels,
    const size_t numClasses,
    const double lambda,
    OptimizerType optimizer)
{
  Train(data, labels, numClasses, lambda, optimizer);
}

template <typename MatType>
template <typename OptimizerType>
double SparseSVM<MatType>::Train(
    const MatType& data,
    const arma::Row<size_t>& labels,
    const size_t numClasses,
    const double lambda,
    OptimizerType optimizer)
{
  SparseSVMFunction<MatType> svm(arma::conv_to<arma::sp_mat>::from(data),
                                 labels, numClasses, lambda);
  if (parameters.is_empty())
    parameters = svm.InitialPoint();

  // Train the model.
  Timer::Start("sparse_svm_optimization");
  const double out = optimizer.Optimize(svm, parameters);
  Timer::Stop("sparse_svm_optimization");

  Log::Info << "SparseSVM::SparseSVM(): final objective of "
            << "trained model is " << out << "." << std::endl;

  return out;
}

template <typename MatType>
void SparseSVM<MatType>::Classify(
    const MatType& dataset,
    arma::Row<size_t>& labels)
const
{
  // Classify each point of dataset into their suitable class.
  labels = arma::conv_to<arma::Row<size_t>>::from(
      arma::index_max(parameters.t() * dataset));
}

template <typename MatType>
double SparseSVM<MatType>::ComputeAccuracy(
    const MatType& testData,
    const arma::Row<size_t>& testLabels)
const
{
  arma::Row<size_t> labels;

  // Get predictions for the provided data.
  Classify(testData, labels);

  // Increment count for every correctly predicted label.
  size_t count = 0;
  for (size_t i = 0; i < labels.n_elem ; i++)
    if (testLabels(i))
      count++;

  // Return percentage accuracy
  return (count * 100.0) / labels.n_elem;
}

} // namespace svm
} // namespace mlpack

#endif