//
// Copyright (c) 2016,2018 CNRS
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __se3_joint_model_hpp__
#define __se3_joint_model_hpp__

#include "pinocchio/assert.hpp"
#include "pinocchio/multibody/joint/joint-collection.hpp"
#include "pinocchio/multibody/joint/joint-basic-visitors.hxx"
#include "pinocchio/container/aligned-vector.hpp"

namespace se3
{

  template<typename JointCollection> struct JointTpl;
  typedef JointTpl<JointCollectionDefault> Joint;

  template<typename JointCollection>
  struct traits< JointTpl<JointCollection> >
  {
    enum {
      Options = JointCollection::Options,
      NQ = Eigen::Dynamic, // Dynamic because unknown at compile time
      NV = Eigen::Dynamic
    };
    typedef typename JointCollection::Scalar Scalar;
    typedef JointDataTpl<JointCollection> JointDataDerived;
    typedef JointModelTpl<JointCollection> JointModelDerived;
    typedef ConstraintTpl<Eigen::Dynamic,Scalar,Options> Constraint_t;
    typedef SE3Tpl<Scalar,Options> Transformation_t;
    typedef MotionTpl<Scalar,Options>  Motion_t;
    typedef MotionTpl<Scalar,Options>  Bias_t;

    typedef Eigen::Matrix<Scalar,6,Eigen::Dynamic,Options> F_t;
    // [ABA]
    typedef Eigen::Matrix<Scalar,6,Eigen::Dynamic,Options> U_t;
    typedef Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic,Options> D_t;
    typedef Eigen::Matrix<Scalar,6,Eigen::Dynamic,Options> UD_t;

    typedef Eigen::Matrix<Scalar,Eigen::Dynamic,1,Options> ConfigVector_t;
    typedef Eigen::Matrix<Scalar,Eigen::Dynamic,1,Options> TangentVector_t;
  };
  
  template<typename JointCollection>
  struct traits< JointDataTpl<JointCollection> >
  { typedef JointTpl<JointCollection> JointDerived; };
  
  template<typename JointCollection>
  struct traits< JointModelTpl<JointCollection> >
  { typedef JointTpl<JointCollection> JointDerived; };

  template<typename JointCollection>
  struct JointDataTpl : public JointDataBase< JointDataTpl<JointCollection> >, JointCollection::JointDataVariant
  {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    typedef JointTpl<JointCollection> JointDerived;
    typedef typename JointCollection::JointDataVariant JointDataVariant;
    
    SE3_JOINT_TYPEDEF_TEMPLATE;

    JointDataVariant & toVariant() { return *static_cast<JointDataVariant*>(this); }
    const JointDataVariant & toVariant() const { return *static_cast<const JointDataVariant*>(this); }

    const Constraint_t      S() const  { return constraint_xd(*this); }
    const Transformation_t  M() const  { return joint_transform(*this); }
    const Motion_t          v() const  { return motion(*this); }
    const Bias_t            c() const  { return bias(*this); }
    
    // // [ABA CCRBA]
    const U_t               U()     const { return u_inertia(*this); }
    U_t                     U()           { return u_inertia(*this); }
    const D_t               Dinv()  const { return dinv_inertia(*this); }
    const UD_t              UDinv() const { return udinv_inertia(*this); }

    JointDataTpl() : JointDataVariant() {}
    JointDataTpl(const JointDataVariant & jdata) : JointDataVariant(jdata) {}

  };

  template<typename JointCollection>
  struct JointModelTpl : public JointModelBase< JointModelTpl<JointCollection> >, JointCollection::JointModelVariant
  {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    typedef typename JointCollection::JointDataVariant JointDataVariant;
    typedef typename JointCollection::JointModelVariant JointModelVariant;
    
    typedef JointModelVariant JointModelBoostVariant;
    typedef JointTpl<JointCollection> JointDerived;
    
    SE3_JOINT_TYPEDEF_TEMPLATE;
    SE3_JOINT_USE_INDEXES;
    using Base::id;
    using Base::setIndexes;
    using Base::operator==;

    JointModelTpl() : JointModelVariant() {}
    JointModelTpl(const JointModelVariant & model_variant) : JointModelVariant(model_variant)
    {}
    
    JointModelVariant & toVariant()
    { return *static_cast<JointModelVariant*>(this); }
    
    const JointModelVariant & toVariant() const
    { return *static_cast<const JointModelVariant*>(this); }

    JointDataVariant createData()
    { return ::se3::createData<JointCollection>(*this); }

    void calc(JointDataDerived & data,const Eigen::VectorXd & q) const
    { calc_zero_order(*this,data,q); }

    void calc(JointDataDerived & data, const Eigen::VectorXd & q, const Eigen::VectorXd & v) const
    { calc_first_order(*this,data,q,v); }
    
    void calc_aba(JointDataDerived & data, Inertia::Matrix6 & I, const bool update_I) const
    { ::se3::calc_aba(*this,data,I,update_I); }
    
    std::string shortname() const { return ::se3::shortname(*this); }
    static std::string classname() { return "JointModel"; }

    int     nq_impl() const { return ::se3::nq(*this); }
    int     nv_impl() const { return ::se3::nv(*this); }

    int     idx_q()   const { return ::se3::idx_q(*this); }
    int     idx_v()   const { return ::se3::idx_v(*this); }

    JointIndex     id()      const { return ::se3::id(*this); }

    void setIndexes(JointIndex id,int nq,int nv) { ::se3::setIndexes(*this,id, nq, nv); }
  };
  
  typedef container::aligned_vector<JointData> JointDataVector;
  typedef container::aligned_vector<JointModel> JointModelVector;

} // namespace se3

#endif // ifndef __se3_joint_model_hpp__
