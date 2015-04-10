/// \file integrTime.h
/// \brief classes that perform time-integration steps
/// \author LNM RWTH Aachen: Joerg Grande, Sven Gross, Volker Reichelt; SC RWTH Aachen: Oliver Fortmeier

/*
 * This file is part of DROPS.
 *
 * DROPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DROPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DROPS. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Copyright 2009 LNM/SC RWTH Aachen, Germany
*/

#ifndef DROPS_STO_INTEGRTIME2PHASE_H
#define DROPS_STO_INTEGRTIME2PHASE_H

#include "stokes/stokes.h"
#include "stokes/instatstokes2phase.h"
//#include "stokes/integrTime.h"
#ifdef _PAR
# include "misc/problem.h"
#endif
#include <typeinfo>

namespace DROPS
{

template< class StokesT, class SolverT, class LsetT>
class TimeDiscStokes2phaseCL
{
  protected:
    StokesT& _Stokes;
    SolverT& _solver;
    LsetT& _lset;

    VelVecDescCL *_b, *_old_b;        // rhs + couplings with poisson matrix A
    VelVecDescCL *_cplM, *_old_cplM;  // couplings with mass matrix M
    VectorCL      _rhs;
    MLMatrixCL    _mat;               // M + theta*dt*A

    double _theta, _dt;

  public:
    TimeDiscStokes2phaseCL( StokesT& Stokes, SolverT& solver, LsetT& lset, double theta= 0.5)
        : _Stokes( Stokes), _solver( solver), _lset(lset), _b( &Stokes.b), _old_b( new VelVecDescCL),
        _cplM( new VelVecDescCL), _old_cplM( new VelVecDescCL), _rhs( Stokes.b.RowIdx->NumUnknowns()),
        _theta( theta)
        {
            _old_b->SetIdx( _b->RowIdx); _cplM->SetIdx( _b->RowIdx); _old_cplM->SetIdx( _b->RowIdx);
            //_Stokes.SetupInstatRhs( _old_b, &_Stokes.c, _old_cplM, _Stokes.v.t, _old_b, _Stokes.v.t);

            // only _old_cplM is required here, if theta!=1 _old_b is needed as well           
            _Stokes.SetupSystem1( &_Stokes.A, &_Stokes.M, _old_b, _old_b, _old_cplM, _lset, _Stokes.v.t);            

        };

    virtual ~TimeDiscStokes2phaseCL()
    {
        if (_old_b == &_Stokes.b)
            delete _b;
        else
            delete _old_b;
        delete _cplM; delete _old_cplM;
    }

    double GetTheta()    const { return _theta; }
    double GetTime()     const { return _Stokes.v.t; }
    double GetTimeStep() const { return _dt; }

    /// \brief Get constant reference on solver
    const SolverT& GetSolver() const { return _solver; }
    /// \brief Get reference on solver
    SolverT& GetSolver()       { return _solver; }

    virtual void SetTimeStep( double dt)= 0;

    virtual void SetTimeStep( double dt, double theta)= 0;

    virtual void DoStep( VectorCL& v, VectorCL& p)= 0;

    MLMatrixCL* GetUpperLeftBlock() { return &_mat; }
    const MLMatrixCL* GetUpperLeftBlock() const { return &_mat; }
};

template < class StokesT, class SolverT, class LsetT>
class InstatStokes2phaseThetaSchemeCL : public TimeDiscStokes2phaseCL< StokesT, SolverT, LsetT>
//**************************************************************************
//  for solving the instationary Stokes equation of type StokesT with a
//  1-step-theta-scheme: theta=1   -> impl. Euler (BDF1, backward Euler)
//                       theta=1/2 -> Crank-Nicholson (Trapezregel)
//
//  Inner stationary Stokes-type problems are solved with a SolverT-solver.
//  The matrices A, B, M and the rhs b, c of the Stokes class have to be set
//  properly! After construction, SetTimeStep has to be called once. Then
//  every DoStep performs one step in time. Changing time steps require
//  further calls to SetTimeStep.
//**************************************************************************
{
  private:
    typedef TimeDiscStokes2phaseCL< StokesT, SolverT, LsetT> base_;
    using base_:: _Stokes;
    using base_:: _solver;
    using base_::  _lset;

    using base_:: _b;        using base_::_old_b;        // rhs + couplings with poisson matrix A
    using base_:: _cplM;     using base_::_old_cplM;  // couplings with mass matrix M
    using base_:: _rhs;
    using base_:: _mat;               // M + theta*dt*A

    using base_:: _theta;    using base_:: _dt;


  public:
    InstatStokes2phaseThetaSchemeCL( StokesT& Stokes, SolverT& solver, LsetT& lset, double theta= 0.5 )
        :base_(Stokes, solver, lset, theta){}

    ~InstatStokes2phaseThetaSchemeCL(){}

    void SetTimeStep( double dt)
    {
        _dt= dt;        
        _mat.LinComb( 1./_dt, _Stokes.M.Data, _theta, _Stokes.A.Data);
        //_mat.LinComb( 0.0, _Stokes.M.Data, 1.0, _Stokes.A.Data);
    }

    void SetTimeStep( double dt, double theta)
    {        
        _theta= theta;
        SetTimeStep( dt );
    }

    void DoStep(VectorCL &v, VectorCL &p);

    //void DoStep( VectorCL& v, VectorCL& p){ DoStep(v,p,0); }
    //void DoStep( VectorCL& v, VectorCL& p);
};

template <class StokesT, class SolverT, class LsetT>
void InstatStokes2phaseThetaSchemeCL<StokesT,SolverT,LsetT>::DoStep( VectorCL& v, VectorCL& p)
{
    _Stokes.v.t+= _dt;

    // if system matrices do not change (i.e. stationary interface) quite inefficient
    // but cplM is needed if boundary conditions are time dependent; cplA is inside b    

    _Stokes.SetupSystem1( &_Stokes.A, &_Stokes.M, _b, _b, _cplM, _lset, _Stokes.v.t);
    _Stokes.SetupSystem2( &_Stokes.B, &_Stokes.c, _lset, _Stokes.v.t );

    // if moving interface: uncomment the following lines
    //_Stokes.SetupSystem2(_Stokes.B, _Stokes.c, _lset, _Stokes.v.t);    
    // epsP = _Stokes.GetXidx().GetBound();
    //_Stokes.SetupC( &_Stokes.C, _lset, epsP );
    // SetTimeStep(_dt);

    // compute surface tension integral for rhs    
    VelVecDescCL surften( &_Stokes.vel_idx );
    _lset.AccumulateBndIntegral( surften );
    _b->Data += surften.Data;

    _rhs=  _Stokes.A.Data * v;
    _rhs*= (_theta-1.);
    _rhs+= (1./_dt)*(_Stokes.M.Data*v + _cplM->Data - _old_cplM->Data)
    //_rhs = (1./_dt)*(_Stokes.M.Data*v + _cplM->Data - _old_cplM->Data)
         +  _theta*_b->Data + (1.-_theta)*_old_b->Data;
    //_rhs = (1./_dt) * ( _Stokes.M.Data * v ) + _b->Data;
    //_rhs = _Stokes.b.Data + (1./_dt) * ( _Stokes.M.Data * v );

    // solve stabilized version for two-phase flow
    std::cout << "dostep: solver.solve" << std:: endl;
    _solver.Solve( _mat, _Stokes.B.Data, _Stokes.C.Data, v, p, _rhs, _Stokes.c.Data, _Stokes.vel_idx.GetEx(), _Stokes.pr_idx.GetEx());
    //_solver.Solve( _mat, _Stokes.B.Data, v, p, _rhs, _Stokes.c.Data, _Stokes.vel_idx.GetEx(), _Stokes.pr_idx.GetEx());


    std::cout << "dostep: swap" << std:: endl;
    std::swap( _b, _old_b);
    std::swap( _cplM, _old_cplM);
}

/*
template< class StokesT, class SolverT>
class TimeDiscStokesCL
{
  protected:
    StokesT& _Stokes;
    SolverT& _solver;

    VelVecDescCL *_b, *_old_b;        // rhs + couplings with poisson matrix A
    VelVecDescCL *_cplM, *_old_cplM;  // couplings with mass matrix M
    VectorCL      _rhs;
    MLMatrixCL    _mat;               // M + theta*dt*A

    double _theta, _dt;

  public:
    TimeDiscStokesCL( StokesT& Stokes, SolverT& solver, double theta= 0.5)
        : _Stokes( Stokes), _solver( solver), _b( &Stokes.b), _old_b( new VelVecDescCL),
        _cplM( new VelVecDescCL), _old_cplM( new VelVecDescCL), _rhs( Stokes.b.RowIdx->NumUnknowns()),
        _theta( theta)
        {
            _old_b->SetIdx( _b->RowIdx); _cplM->SetIdx( _b->RowIdx); _old_cplM->SetIdx( _b->RowIdx);
            _Stokes.SetupInstatRhs( _old_b, &_Stokes.c, _old_cplM, _Stokes.v.t, _old_b, _Stokes.v.t);
        };

    virtual ~TimeDiscStokesCL()
    {
        if (_old_b == &_Stokes.b)
            delete _b;
        else
            delete _old_b;
            delete _cplM; delete _old_cplM;
    }

    double GetTheta()    const { return _theta; }
    double GetTime()     const { return _Stokes.t; }
    double GetTimeStep() const { return _dt; }

    /// \brief Get constant reference on solver
    const SolverT& GetSolver() const { return _solver; }
    /// \brief Get reference on solver
    SolverT& GetSolver()       { return _solver; }

    virtual void SetTimeStep( double dt)= 0;

    virtual void SetTimeStep( double dt, double theta)= 0;

    virtual void DoStep( VectorCL& v, VectorCL& p)= 0;

    MLMatrixCL* GetUpperLeftBlock() { return &_mat; }
    const MLMatrixCL* GetUpperLeftBlock() const { return &_mat; }
};

template < class StokesT, class SolverT, class LsetT=bool>
class InstatStokesThetaSchemeCL : public TimeDiscStokesCL< StokesT, SolverT>
//**************************************************************************
//  for solving the instationary Stokes equation of type StokesT with a
//  1-step-theta-scheme: theta=1   -> impl. Euler (BDF1, backward Euler)
//                       theta=1/2 -> Crank-Nicholson (Trapezregel)
//
//  Inner stationary Stokes-type problems are solved with a SolverT-solver.
//  The matrices A, B, M and the rhs b, c of the Stokes class have to be set
//  properly! After construction, SetTimeStep has to be called once. Then
//  every DoStep performs one step in time. Changing time steps require
//  further calls to SetTimeStep.
//**************************************************************************
{
  private:
    typedef TimeDiscStokesCL< StokesT, SolverT> base_;
    using base_:: _Stokes;
    using base_:: _solver;

    using base_:: _b;        using base_::_old_b;        // rhs + couplings with poisson matrix A
    using base_:: _cplM;     using base_::_old_cplM;  // couplings with mass matrix M
    using base_:: _rhs;
    using base_:: _mat;               // M + theta*dt*A

    using base_:: _theta;    using base_:: _dt;

    LsetT& _lset;

  public:
    InstatStokesThetaSchemeCL( StokesT& Stokes, SolverT& solver, double theta= 0.5, LsetT &lset = LsetT())
        :base_(Stokes, solver, theta), _lset(lset){};

    // only in c++11 delegating constructor
    //InstatStokesThetaSchemeCL(StokesT &Stokes, SolverT &solver, LsetT &lset, double theta = 1)
    //    :InstatStokesThetaSchemeCL(Stokes,solver,theta), _lset(lset){}

    ~InstatStokesThetaSchemeCL(){}

    void SetTimeStep( double dt)
    {
        _dt= dt;
        _mat.LinComb( 1./dt, _Stokes.M.Data, _theta, _Stokes.A.Data);
    }

    void SetTimeStep( double dt, double theta)
    {
        _dt= dt;
        _mat.LinComb( 1./dt, _Stokes.M.Data, _theta, _Stokes.A.Data);
         _theta= theta;
    }

    void DoStep(VectorCL &v, VectorCL &p, double epsP);

    void DoStep( VectorCL& v, VectorCL& p){ DoStep(v,p,0); }
    //void DoStep( VectorCL& v, VectorCL& p);
};


template< template<class, class> class BaseMethod, class StokesT, class SolverT>
class StokesFracStepSchemeCL : public BaseMethod<StokesT, SolverT>
{
  private:
    static const double facdt_[3];
    static const double theta_[3];

    typedef BaseMethod<StokesT, SolverT> base_;

    double dt3_;
    int step_;



  public:
    StokesFracStepSchemeCL( StokesT& Stokes, SolverT& solver, int step = -1)
    	   : base_( Stokes, solver), step_((step >= 0) ? step%3 : 0) {}
    double GetSubTimeStep() const { return facdt_[step_]*dt3_; }
    double GetSubTheta()    const { return theta_[step_]; }
    int    GetSubStep()     const { return step_; }

    void SetTimeStep (double dt) { // overwrites baseclass-version
        dt3_= dt;
    }

    void SetTimeStep( double dt, double) { // overwrites baseclass-version
        dt3_= dt;
    }

    void DoSubStep( VectorCL& v, VectorCL& p) {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Fractional Step Method: Substep " << step_ << '\n';
        base_::SetTimeStep( GetSubTimeStep(), GetSubTheta());
        base_::DoStep( v, p);
        step_= (step_ + 1)%3;
    }

    void DoStep( VectorCL& v, VectorCL& p) {
        DoSubStep( v, p);
        DoSubStep( v, p);
        DoSubStep( v, p);
    }
};

template < template<class, class> class BaseMethod, class StokesT, class SolverT>
const double StokesFracStepSchemeCL<BaseMethod, StokesT, SolverT>::facdt_[3]
  = { 1.0 - std::sqrt( 0.5), std::sqrt( 2.0) - 1.0, 1.0 - std::sqrt( 0.5) };

template < template<class, class> class BaseMethod, class StokesT, class SolverT>
const double StokesFracStepSchemeCL<BaseMethod, StokesT, SolverT>::theta_[3]
  = { 2.0 - std::sqrt( 2.0), std::sqrt( 2.0) - 1.0, 2.0 - std::sqrt( 2.0) };


//=================================
//     template definitions
//=================================

template <class StokesT, class SolverT, class LsetT>
void InstatStokesThetaSchemeCL<StokesT,SolverT,LsetT>::DoStep( VectorCL& v, VectorCL& p, double epsP)
{
    _Stokes.v.t+= _dt;
    if( typeid(LsetT) == typeid(bool) )
    {
        //no level set is defined, i.e. one-phase flow problem
        _Stokes.SetupInstatRhs( _b, &_Stokes.c, _cplM, _Stokes.v.t, _b, _Stokes.v.t);
    }
    else
    {
        // if system matrices do not change (i.e. stationary interface) quite inefficient
        // but cplM is needed if boundary conditions are time dependent; cplA is inside b
        _Stokes.SetupSystem1( &_Stokes.A, &_Stokes.M, _b, _b, _cplM, _lset, _Stokes.v.t);

        // if moving interface: uncomment the following three lines
        //_Stokes.SetupSystem2(_Stokes.B, _Stokes.c, _lset, _Stokes.v.t);
        //_Stokes.SetupC( &_Stokes.C, _lset, epsP ); // OK epsP really needs to get in here somehow differently
        // SetTimeStep(_dt);

        // compute surface tension integral for rhs
        VelVecDescCL surften( &_Stokes.vel_idx );
        _lset.AccumulateBndIntegral( surften );
        _b->Data += surften.Data;
    }

    _rhs=  _Stokes.A.Data * v;
    _rhs*= (_theta-1.);
    _rhs+= (1./_dt)*(_Stokes.M.Data*v + _cplM->Data - _old_cplM->Data)
         +  _theta*_b->Data + (1.-_theta)*_old_b->Data;

    if( typeid(LsetT) == typeid(bool) )
    {
        _solver.Solve( _mat, _Stokes.B.Data, v, p, _rhs, _Stokes.c.Data, _Stokes.vel_idx.GetEx(), _Stokes.pr_idx.GetEx());
    }
    else
    {
        // solve stabilized version for two-phase flow
        _solver.Solve( _mat, _Stokes.B.Data, _Stokes.C.Data, v, p, _rhs, _Stokes.c.Data, _Stokes.vel_idx.GetEx(), _Stokes.pr_idx.GetEx());
    }

    std::swap( _b, _old_b);
    std::swap( _cplM, _old_cplM);
}
*/
}    // end of namespace DROPS

#endif
