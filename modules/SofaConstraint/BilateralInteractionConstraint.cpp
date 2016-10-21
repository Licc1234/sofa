/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_COMPONENT_CONSTRAINTSET_BILATERALINTERACTIONCONSTRAINT_CPP

#include <SofaConstraint/BilateralInteractionConstraint.inl>

#include <sofa/defaulttype/Vec3Types.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace constraintset
{

namespace bilateralinteractionconstraint
{

using sofa::core::behavior::ConstraintResolution ;
using sofa::core::ConstraintParams ;
using sofa::defaulttype::BaseVector ;

using namespace sofa::defaulttype;
using namespace sofa::helper;

class RigidImpl {};

template<>
class BilateralInteractionConstraintSpecialization<RigidImpl>
{
public:
    template<class T>
    static void getConstraintResolution(BilateralInteractionConstraint<T>& self,
                                        const ConstraintParams* cParams,
                                        std::vector<ConstraintResolution*>& resTab,
                                        unsigned int& offset, double tolerance)
    {
        SOFA_UNUSED(cParams);
        unsigned minp=std::min(self.m1.getValue().size(),
                               self.m2.getValue().size());
        for (unsigned pid=0; pid<minp; pid++)
        {
            resTab[offset] = new BilateralConstraintResolution3Dof();
            offset += 3;
            BilateralConstraintResolution3Dof* temp = new BilateralConstraintResolution3Dof();
            temp->tolerance = tolerance;	// specific (smaller) tolerance for the rotation
            resTab[offset] = temp;
            offset += 3;
        }
    }


    template <class T>
    static void buildConstraintMatrix(BilateralInteractionConstraint<T>& self,
                                      const ConstraintParams* cParams,
                                      typename BilateralInteractionConstraint<T>::DataMatrixDeriv &c1_d,
                                      typename BilateralInteractionConstraint<T>::DataMatrixDeriv &c2_d,
                                      unsigned int &constraintId,
                                      const typename BilateralInteractionConstraint<T>::DataVecCoord &/*x1*/,
                                      const typename BilateralInteractionConstraint<T>::DataVecCoord &/*x2*/)
    {
        SOFA_UNUSED(cParams) ;
        const helper::vector<int> &m1Indices = self.m1.getValue();
        const helper::vector<int> &m2Indices = self.m2.getValue();

        unsigned minp = std::min(m1Indices.size(),m2Indices.size());
        self.cid.resize(minp);

        typename BilateralInteractionConstraint<T>::MatrixDeriv &c1 = *c1_d.beginEdit();
        typename BilateralInteractionConstraint<T>::MatrixDeriv &c2 = *c2_d.beginEdit();

        const Vec<3, typename BilateralInteractionConstraint<T>::Real> cx(1,0,0), cy(0,1,0), cz(0,0,1);
        const Vec<3, typename BilateralInteractionConstraint<T>::Real> vZero(0,0,0);

        for (unsigned pid=0; pid<minp; pid++)
        {
            int tm1 = m1Indices[pid];
            int tm2 = m2Indices[pid];

            self.cid[pid] = constraintId;
            constraintId += 6;

            //Apply constraint for position
            typename BilateralInteractionConstraint<T>::MatrixDerivRowIterator c1_it = c1.writeLine(self.cid[pid]);
            c1_it.addCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(-cx, vZero));

            typename BilateralInteractionConstraint<T>::MatrixDerivRowIterator c2_it = c2.writeLine(self.cid[pid]);
            c2_it.addCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(cx, vZero));

            c1_it = c1.writeLine(self.cid[pid] + 1);
            c1_it.setCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(-cy, vZero));

            c2_it = c2.writeLine(self.cid[pid] + 1);
            c2_it.setCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(cy, vZero));

            c1_it = c1.writeLine(self.cid[pid] + 2);
            c1_it.setCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(-cz, vZero));

            c2_it = c2.writeLine(self.cid[pid] + 2);
            c2_it.setCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(cz, vZero));

            //Apply constraint for orientation
            c1_it = c1.writeLine(self.cid[pid] + 3);
            c1_it.setCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(vZero, -cx));

            c2_it = c2.writeLine(self.cid[pid] + 3);
            c2_it.setCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(vZero, cx));

            c1_it = c1.writeLine(self.cid[pid] + 4);
            c1_it.setCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(vZero, -cy));

            c2_it = c2.writeLine(self.cid[pid] + 4);
            c2_it.setCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(vZero, cy));

            c1_it = c1.writeLine(self.cid[pid] + 5);
            c1_it.setCol(tm1, typename BilateralInteractionConstraint<T>::Deriv(vZero, -cz));

            c2_it = c2.writeLine(self.cid[pid] + 5);
            c2_it.setCol(tm2, typename BilateralInteractionConstraint<T>::Deriv(vZero, cz));
        }

        c1_d.endEdit();
        c2_d.endEdit();
    }


    template <class T>
    static void getConstraintViolation(BilateralInteractionConstraint<T>& self,
                                const ConstraintParams* /*cParams*/,
                                BaseVector *v,
                                const  typename BilateralInteractionConstraint<T>::DataVecCoord &d_x1,
                                const  typename BilateralInteractionConstraint<T>::DataVecCoord &d_x2,
                                const  typename BilateralInteractionConstraint<T>::DataVecDeriv &/*v1*/,
                                const  typename BilateralInteractionConstraint<T>::DataVecDeriv &/*v2*/)
    {
        const helper::vector<int> &m1Indices = self.m1.getValue();
        const helper::vector<int> &m2Indices = self.m2.getValue();

        unsigned min = std::min(m1Indices.size(), m2Indices.size());
        const  typename BilateralInteractionConstraint<T>::VecDeriv& restVector = self.restVector.getValue();
        self.dfree.resize(min);

        const  typename BilateralInteractionConstraint<T>::VecCoord &x1 = d_x1.getValue();
        const  typename BilateralInteractionConstraint<T>::VecCoord &x2 = d_x2.getValue();

        for (unsigned pid=0; pid<min; pid++)
        {
            typename BilateralInteractionConstraint<T>::Coord dof1 = x1[m1Indices[pid]];
            typename BilateralInteractionConstraint<T>::Coord dof2 = x2[m2Indices[pid]];

            getVCenter(self.dfree[pid]) = dof2.getCenter() - dof1.getCenter();
            getVOrientation(self.dfree[pid]) =  dof1.rotate(self.q.angularDisplacement(dof2.getOrientation() ,
                                                                                  dof1.getOrientation())); // angularDisplacement compute the rotation vector btw the two quaternions
            if (pid < restVector.size())
                self.dfree[pid] -= restVector[pid];

            for (unsigned int i=0 ; i<self.dfree[pid].size() ; i++)
                v->set(self.cid[pid]+i, self.dfree[pid][i]);
        }
    }

};


#ifdef SOFA_WITH_DOUBLE
template<>
void BilateralInteractionConstraint<Rigid3dTypes>::getConstraintResolution(const ConstraintParams* cParams,
                                                                           std::vector<ConstraintResolution*>& resTab,
                                                                           unsigned int& offset)
{
    BilateralInteractionConstraintSpecialization<RigidImpl>::getConstraintResolution(*this,
                                                                                     cParams, resTab, offset,
                                                                                     0.01) ;
}

template <>
void BilateralInteractionConstraint<Rigid3dTypes>::buildConstraintMatrix(const ConstraintParams* cParams,
                                                                         DataMatrixDeriv &c1_d,
                                                                         DataMatrixDeriv &c2_d,
                                                                         unsigned int &constraintId,
                                                                         const DataVecCoord &x1, const DataVecCoord &x2)
{
    BilateralInteractionConstraintSpecialization<RigidImpl>::buildConstraintMatrix(*this,
                                                                                   cParams, c1_d, c2_d, constraintId,
                                                                                   x1, x2) ;
}


template <>
void BilateralInteractionConstraint<Rigid3dTypes>::getConstraintViolation(const ConstraintParams* cParams,
                                                                          BaseVector *v,
                                                                          const DataVecCoord &d_x1, const DataVecCoord &d_x2,
                                                                          const DataVecDeriv &v1, const DataVecDeriv &v2)
{
    BilateralInteractionConstraintSpecialization<RigidImpl>::getConstraintViolation(*this,
                                                                                    cParams, v, d_x1, d_x2,
                                                                                    v1, v2) ;
}


template <>
void BilateralInteractionConstraint<Rigid3dTypes>::getVelocityViolation(BaseVector * /*v*/,
                                                                        const DataVecCoord &/*x1*/,
                                                                        const DataVecCoord &/*x2*/,
                                                                        const DataVecDeriv &/*v1*/,
                                                                        const DataVecDeriv &/*v2*/)
{

}

template<>
void BilateralInteractionConstraint<defaulttype::Rigid3dTypes>::addContact(Deriv /*norm*/,
                                                                           Coord P, Coord Q, Real /*contactDistance*/,
                                                                           int m1, int m2,
                                                                           Coord /*Pfree*/, Coord /*Qfree*/,
                                                                           long /*id*/, PersistentID /*localid*/)
{
    helper::WriteAccessor<Data<helper::vector<int> > > wm1 = this->m1;
    helper::WriteAccessor<Data<helper::vector<int> > > wm2 = this->m2;
    helper::WriteAccessor<Data<VecDeriv > > wrest = this->restVector;
    wm1.push_back(m1);
    wm2.push_back(m2);
    Deriv diff;
    getVCenter(diff) = Q.getCenter() - P.getCenter();
    getVOrientation(diff) =  P.rotate(q.angularDisplacement(Q.getOrientation() , P.getOrientation())) ; // angularDisplacement compute the rotation vector btw the two quaternions
    wrest.push_back(diff);
}
#endif

#ifdef SOFA_WITH_FLOAT
template<>
void BilateralInteractionConstraint<Rigid3fTypes>::getConstraintResolution(const ConstraintParams* cParams,
                                                                           std::vector<ConstraintResolution*>& resTab,
                                                                           unsigned int& offset)
{

    BilateralInteractionConstraintSpecialization<RigidImpl>::getConstraintResolution(*this,
                                                                                     cParams, resTab, offset,
                                                                                     0.0001) ;
}

template <>
void BilateralInteractionConstraint<Rigid3fTypes>::buildConstraintMatrix(const ConstraintParams* cParams,
                                                                         DataMatrixDeriv &c1_d,
                                                                         DataMatrixDeriv &c2_d,
                                                                         unsigned int &constraintId,
                                                                         const DataVecCoord &x1, const DataVecCoord &x2)
{
    BilateralInteractionConstraintSpecialization<RigidImpl>::buildConstraintMatrix(*this,
                                                                                   cParams, c1_d, c2_d, constraintId,
                                                                                   x1, x2) ;
}


template <>
void BilateralInteractionConstraint<Rigid3fTypes>::getConstraintViolation(const ConstraintParams* cParams,
                                                                          BaseVector *v,
                                                                          const DataVecCoord &d_x1,
                                                                          const DataVecCoord &d_x2,
                                                                          const DataVecDeriv &v1, const DataVecDeriv &v2)
{
    BilateralInteractionConstraintSpecialization<RigidImpl>::getConstraintViolation(*this,
                                                                                    cParams, v, d_x1, d_x2,
                                                                                    v1, v2) ;
}

template <>
void BilateralInteractionConstraint<Rigid3fTypes>::getVelocityViolation(BaseVector * /*v*/,
                                                                        const DataVecCoord &/*x1*/,
                                                                        const DataVecCoord &/*x2*/,
                                                                        const DataVecDeriv &/*v1*/,
                                                                        const DataVecDeriv &/*v2*/)
{

}



template<>
void BilateralInteractionConstraint<defaulttype::Rigid3fTypes>::addContact(Deriv /*norm*/, Coord P, Coord Q, Real /*contactDistance*/, int m1, int m2, Coord /*Pfree*/, Coord /*Qfree*/, long /*id*/, PersistentID /*localid*/)
{
    helper::WriteAccessor<Data<helper::vector<int> > > wm1 = this->m1;
    helper::WriteAccessor<Data<helper::vector<int> > > wm2 = this->m2;
    helper::WriteAccessor<Data<VecDeriv > > wrest = this->restVector;
    wm1.push_back(m1);
    wm2.push_back(m2);
    Deriv diff;
    getVCenter(diff) = Q.getCenter() - P.getCenter();
    getVOrientation(diff) =  P.rotate(q.angularDisplacement(Q.getOrientation() , P.getOrientation())) ; // angularDisplacement compute the rotation vector btw the two quaternions
    wrest.push_back(diff);
}

#endif


SOFA_DECL_CLASS(BilateralInteractionConstraint)

int BilateralInteractionConstraintClass = core::RegisterObject("TODO-BilateralInteractionConstraint")
        #ifdef SOFA_WITH_DOUBLE
        .add< BilateralInteractionConstraint<Vec3dTypes> >()
        .add< BilateralInteractionConstraint<Rigid3dTypes> >()
        #endif
        #ifdef SOFA_WITH_FLOAT
        .add< BilateralInteractionConstraint<Vec3fTypes> >()
        .add< BilateralInteractionConstraint<Rigid3fTypes> >()
        #endif
        ;

#ifdef SOFA_WITH_DOUBLE
template class SOFA_CONSTRAINT_API BilateralInteractionConstraint<Vec3dTypes>;
template class SOFA_CONSTRAINT_API BilateralInteractionConstraint<Rigid3dTypes>;
#endif
#ifdef SOFA_WITH_FLOAT
template class SOFA_CONSTRAINT_API BilateralInteractionConstraint<Vec3fTypes>;
template class SOFA_CONSTRAINT_API BilateralInteractionConstraint<Rigid3fTypes>;
#endif

}

} // namespace constraintset

} // namespace component

} // namespace sofa

