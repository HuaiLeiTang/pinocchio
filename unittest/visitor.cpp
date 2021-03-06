//
// Copyright (c) 2015-2018 CNRS
//

#include <iostream>

#include <pinocchio/multibody/joint/joint-composite.hpp>
#include <pinocchio/multibody/model.hpp>
#include "pinocchio/multibody/data.hpp"
#include "pinocchio/multibody/visitor.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

namespace pinocchio
{

  struct SimpleVisitor : public pinocchio::fusion::JointVisitorBase<SimpleVisitor>
  {
    typedef boost::fusion::vector<const pinocchio::Model &,
                                  pinocchio::Data &,
                                  JointIndex
                                  > ArgsType;

    template<typename JointModel>
    static void algo(const pinocchio::JointModelBase<JointModel> & jmodel,
                     pinocchio::JointDataBase<typename JointModel::JointDataDerived> & jdata,
                     const pinocchio::Model & model,
                     pinocchio::Data & data,
                     JointIndex jointId);
  };

  template<typename JointModel>
  void SimpleVisitor::algo(const pinocchio::JointModelBase<JointModel> & /*jmodel*/,
                           pinocchio::JointDataBase<typename JointModel::JointDataDerived> & /*jdata*/,
                           const pinocchio::Model & /*model*/,
                           pinocchio::Data & /*data*/,
                           JointIndex /*dummy*/)
  { /* --- do nothing --- */ }

  template<>
  void SimpleVisitor::algo(const pinocchio::JointModelBase<JointModelRevoluteUnaligned> & jmodel,
                           pinocchio::JointDataBase<JointDataRevoluteUnaligned> & /*jdata*/,
                           const pinocchio::Model & /*model*/,
                           pinocchio::Data & /*data*/,
                           JointIndex /*dummy*/)
  {
    BOOST_CHECK( jmodel.shortname() == JointModelRevoluteUnaligned::classname() );
    Eigen::Vector3d axis = jmodel.derived().axis;
    Eigen::Vector3d axis_z; axis_z << 0,0,1;

    BOOST_CHECK ( axis == axis_z );
  }

} // namespace pinocchio

/* Validates the access to memory stored in joint models, by using the class
 * joint model revolute unaligned. 
 */

BOOST_AUTO_TEST_SUITE ( BOOST_TEST_MODULE )

BOOST_AUTO_TEST_CASE ( test_runal )
{
  using namespace pinocchio;

  pinocchio::Model model;
  model.addJoint(0,pinocchio::JointModelRevoluteUnaligned(0,0,1),pinocchio::SE3::Random(),"");
  Data data(model);

  for( Model::JointIndex i=1;i<(Model::JointIndex)model.njoints;++i )
    {
      SimpleVisitor::run(model.joints[i],data.joints[i],
                         SimpleVisitor::ArgsType(model,data,i));
    }
}

BOOST_AUTO_TEST_SUITE_END ()
