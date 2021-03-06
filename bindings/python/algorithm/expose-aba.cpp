//
// Copyright (c) 2015-2016 CNRS
//

#include "pinocchio/bindings/python/algorithm/algorithms.hpp"
#include "pinocchio/algorithm/aba.hpp"

namespace pinocchio
{
  namespace python
  {
    
    const Data::RowMatrixXs &
    computeMinverse_proxy(const Model & model, Data & data, const Eigen::VectorXd & q)
    {
      computeMinverse(model,data,q);
      data.Minv.triangularView<Eigen::StrictlyLower>() =
      data.Minv.transpose().triangularView<Eigen::StrictlyLower>();
      return data.Minv;
    }
    
    void exposeABA()
    {
      using namespace Eigen;

      bp::def("aba",
              &aba<double,0,JointCollectionDefaultTpl,VectorXd,VectorXd,VectorXd>,
              bp::args("Model","Data",
                       "Joint configuration q (size Model::nq)",
                       "Joint velocity v (size Model::nv)",
                       "Joint torque tau (size Model::nv)"),
              "Compute ABA, put the result in Data::ddq and return it.",
              bp::return_value_policy<bp::return_by_value>());

      bp::def("aba",
              &aba<double,0,JointCollectionDefaultTpl,VectorXd,VectorXd,VectorXd,Force>,
              bp::args("Model","Data",
                       "Joint configuration q (size Model::nq)",
                       "Joint velocity v (size Model::nv)",
                       "Joint torque tau (size Model::nv)",
                       "Vector of external forces expressed in the local frame of each joint (size Model::njoints)"),
              "Compute ABA with external forces, put the result in Data::ddq and return it.",
              bp::return_value_policy<bp::return_by_value>());
      
      bp::def("computeMinverse",
              &computeMinverse_proxy,
              bp::args("Model","Data",
                       "Joint configuration q (size Model::nq)"),
              "Computes the inverse of the joint space inertia matrix using a variant of the Articulated Body algorithm.\n"
              "The result is stored in data.Minv.",
              bp::return_value_policy<bp::return_by_value>());
      
    }
    
  } // namespace python
} // namespace pinocchio
