/// \file film.cpp
/// \brief flow of falling film
/// \author LNM RWTH Aachen: Patrick Esser, Joerg Grande, Sven Gross; SC RWTH Aachen:

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

//multigrid
#include "geom/multigrid.h"
#include "geom/builder.h"
//time integration
#include "navstokes/instatnavstokes2phase.h"
//output
#include "out/output.h"
#include "out/ensightOut.h"
#include "out/vtkOut.h"
//levelset
#include "levelset/coupling.h"
#include "levelset/adaptriang.h"
#include "levelset/marking_strategy.h"
#include "levelset/mzelle_hdr.h"
#include "levelset/twophaseutils.h"
//surfactants
#include "surfactant/ifacetransp.h"
//function map
#include "misc/funcmap.h"
//solver factory for stokes
#include "num/stokessolverfactory.h"
#ifndef _PAR
#include "num/krylovsolver.h"
#include "num/precond.h"
#else
#include "parallel/loadbal.h"
#include "parallel/parmultigrid.h"
#endif
//general: streams
#include <fstream>
#include <sstream>

#include "misc/dynamicload.h"

DROPS::ParamCL P;

bool periodic_xz( const DROPS::Point3DCL& p, const DROPS::Point3DCL& q)
{ // matching y-z- or x-y-coords, resp.
    const DROPS::Point3DCL d= fabs(p-q),
                           L= fabs(P.get<DROPS::Point3DCL>("MeshSize"));
    return (d[1] + d[2] < 1e-12 && std::abs( d[0] - L[0]) < 1e-12)  // dy=dz=0 and dx=Lx
      ||   (d[0] + d[1] < 1e-12 && std::abs( d[2] - L[2]) < 1e-12)  // dx=dy=0 and dz=Lz
      ||   (d[1] < 1e-12 && std::abs( d[0] - L[0]) < 1e-12 && std::abs( d[2] - L[2]) < 1e-12);  // dy=0 and dx=Lx and dz=Lz
}

double sigma;
double sigmaf (const DROPS::Point3DCL&, double) { return sigma; }

namespace DROPS // for Strategy
{

double GetTimeOffset(){
    double timeoffset = 0.0;
    const std::string restartfilename = P.get<std::string>("InitialFile");
    if (P.get<int>("InitialCond") == -1){
        const std::string timefilename = restartfilename + "time";
        std::ifstream f_(timefilename.c_str());
        f_ >> timeoffset;
        std::cout << "used time offset file is " << timefilename << std::endl;
        std::cout << "time offset is " << timeoffset << std::endl;
    }
    return timeoffset;
}

/// \brief Observes the MultiGridCL-changes by AdapTriangCL to repair the Ensight index.
///
/// The actual work is done in post_refine().
class EnsightIdxRepairCL: public MGObserverCL
{
  private:
    MultiGridCL& mg_;
    IdxDescCL&   idx_;

  public:
    EnsightIdxRepairCL( MultiGridCL& mg, IdxDescCL& idx)
      : mg_(mg), idx_(idx) {}

    void pre_refine  () {}
    void post_refine () { idx_.DeleteNumbering( mg_); idx_.CreateNumbering( mg_.GetLastLevel(), mg_); }

    void pre_refine_sequence  () {}
    void post_refine_sequence () {}
    const IdxDescCL* GetIdxDesc() const { return &idx_; }
};


template<class StokesProblemT>
void Strategy( StokesProblemT& Stokes, LevelsetP2CL& lset, AdapTriangCL& adap, bool is_periodic)
// flow control
{

    DROPS::match_fun periodic_match = DROPS::MatchMap::getInstance()[P.get<std::string>("DomainCond.PeriodicMatching", std::string("periodicxz"))];
    MultiGridCL& MG= Stokes.GetMG();

    MLIdxDescCL* vidx= &Stokes.vel_idx;
    MLIdxDescCL* pidx= &Stokes.pr_idx;
    IdxDescCL ens_idx( P2_FE, NoBndDataCL<>());

    LevelsetRepairCL lsetrepair( lset);
    adap.push_back( &lsetrepair);
    VelocityRepairCL velrepair( Stokes);
    adap.push_back( &velrepair);
    PressureRepairCL prrepair( Stokes, lset);
    adap.push_back( &prrepair);
    EnsightIdxRepairCL ensrepair( MG, ens_idx);
    adap.push_back( &ensrepair);

    lset.CreateNumbering(      MG.GetLastLevel(), periodic_match);
    DROPS::InstatScalarFunction DistanceFct = DROPS::InScaMap::getInstance()[P.get("Exp.InitialLSet", std::string("WavyFilm"))];
    lset.Init( DistanceFct);
    if ( StokesSolverFactoryHelperCL().VelMGUsed(P))
        Stokes.SetNumVelLvl ( Stokes.GetMG().GetNumLevel());
    if ( StokesSolverFactoryHelperCL().PrMGUsed(P))
        Stokes.SetNumPrLvl  ( Stokes.GetMG().GetNumLevel());
    Stokes.CreateNumberingVel( MG.GetLastLevel(), vidx, periodic_match);
    Stokes.CreateNumberingPr(  MG.GetLastLevel(), pidx, periodic_match, &lset);
    ens_idx.CreateNumbering( MG.GetLastLevel(), MG);

    Stokes.b.SetIdx( vidx);
    Stokes.c.SetIdx( pidx);
    Stokes.p.SetIdx( pidx);
    Stokes.v.SetIdx( vidx);
    std::cout << Stokes.p.Data.size() << " pressure unknowns,\n";
    std::cout << Stokes.v.Data.size() << " velocity unknowns,\n";
    std::cout << lset.Phi.Data.size() << " levelset unknowns.\n";
    Stokes.A.SetIdx(vidx, vidx);
    Stokes.B.SetIdx(pidx, vidx);
    Stokes.M.SetIdx(vidx, vidx);
    Stokes.N.SetIdx(vidx, vidx);
    Stokes.prM.SetIdx( pidx, pidx);
    Stokes.prA.SetIdx( pidx, pidx);


    DROPS::InVecMap & vecmap =  DROPS::InVecMap::getInstance();
    DROPS::StokesVelBndDataCL::bnd_val_fun ZeroVel = vecmap["ZeroVel"];
    //DROPS::StokesVelBndDataCL::bnd_val_fun Inflow = vecmap["FilmInflow"];
    DROPS::instat_vector_fun_ptr Nusselt = vecmap["NusseltFilm"];
    switch (P.get<int>("InitialCond"))
    {
      case 1: // stationary flow
      {
        TimerCL time;
        VelVecDescCL curv( vidx);
        time.Reset();
        Stokes.SetupPrMass(  &Stokes.prM, lset/*, P.get<double>("Mat.ViscFluid"), C.mat_ViscGas*/);
        Stokes.SetupSystem1( &Stokes.A, &Stokes.M, &Stokes.b, &Stokes.b, &curv, lset, Stokes.v.t);
        Stokes.SetupSystem2( &Stokes.B, &Stokes.C, &Stokes.c, lset, Stokes.v.t);
        curv.Clear( Stokes.v.t);
        lset.AccumulateBndIntegral( curv);
        time.Stop();
        std::cout << "Discretizing Stokes/Curv for initial velocities took "<<time.GetTime()<<" sec.\n";

        time.Reset();
        SSORPcCL ssorpc;
        PCGSolverCL<SSORPcCL> PCGsolver( ssorpc, P.get<int>("Stokes.PcAIter"), P.get<double>("Stokes.PcATol"), true);
        typedef SolverAsPreCL<PCGSolverCL<SSORPcCL> > PCGPcT;
        PCGPcT apc( PCGsolver);
        StokesSolverBaseCL *ssolver = 0;
        if( Stokes.usesGhostPen() )
        {
            typedef BlockPreCL<ExpensivePreBaseCL, SchurPreBaseCL, LowerBlockPreCL>    LowerBlockPcT;
            ISGhPenPreCL gpispc( &Stokes.prA.Data.GetFinest(), &Stokes.prM.Data.GetFinest(), &Stokes.C.Data.GetFinest(), 0., 1., P.get<double>("Stokes.PcSTol"), P.get<double>("Stokes.PcSTol"), 200);
            GCRSolverCL< LowerBlockPcT > gcrsolver( *(new LowerBlockPcT(apc, gpispc)),P.get<int>("Stokes.OuterIter"),P.get<int>("Stokes.OuterIter"),P.get<int>("Stokes.OuterTol") );
            ssolver = new BlockMatrixSolverCL< GCRSolverCL<LowerBlockPcT> > ( gcrsolver );
        }
        else
        {
            ISBBTPreCL bbtispc( &Stokes.B.Data.GetFinest(), &Stokes.prM.Data.GetFinest(), &Stokes.M.Data.GetFinest(), Stokes.pr_idx.GetFinest(), 0.0, 1.0, P.get<double>("Stokes.PcSTol"), P.get<double>("Stokes.PcSTol"));
            ssolver = new InexactUzawaCL<PCGPcT, ISBBTPreCL, APC_OTHER> ( apc, bbtispc, P.get<int>("Stokes.OuterIter"), P.get<double>("Stokes.OuterTol"), 0.6, 50);
        }
        ssolver->Solve( Stokes.A.Data, Stokes.B.Data, Stokes.C.Data,
            Stokes.v.Data, Stokes.p.Data, Stokes.b.Data, Stokes.c.Data, Stokes.vel_idx.GetEx(), Stokes.pr_idx.GetEx());

        time.Stop();
        std::cout << "Solving Stokes for initial velocities took "<<time.GetTime()<<" sec.\n";
        delete ssolver;
      } break;

      case 2: // Nusselt solution
      {
        std::cout<<"Here"<<std::endl;
        //Stokes.InitVel( &Stokes.v, Inflow);
        Stokes.InitVel( &Stokes.v, Nusselt);
      } break;

      case -1: // read from file
      {
        if(P.get<int>("Ensight.EnsightOut",0))
        {
            ReadEnsightP2SolCL reader( MG);
            reader.ReadVector( P.get<std::string>("InitialFile")+".vel", Stokes.v, Stokes.GetBndData().Vel);
            reader.ReadScalar( P.get<std::string>("InitialFile")+".scl", lset.Phi, lset.GetBndData());
            Stokes.UpdateXNumbering( pidx, lset);
            Stokes.p.SetIdx( pidx); // Zero-vector for now.
            reader.ReadScalar( P.get<std::string>("InitialFile")+".pr",  Stokes.p, Stokes.GetBndData().Pr); // reads the P1-part of the pressure
        }
        else
        {
            ReadFEFromFile( Stokes.v, MG, P.get<std::string>("InitialFile")+"velocity", P.get<int>("Restart.Binary"));
            ReadFEFromFile( lset.Phi, MG, P.get<std::string>("InitialFile")+"levelset", P.get<int>("Restart.Binary"));
            Stokes.UpdateXNumbering( pidx, lset);
            Stokes.p.SetIdx( pidx);
            ReadFEFromFile( Stokes.p, MG, P.get<std::string>("InitialFile")+"pressure", P.get<int>("Restart.Binary"), &lset.Phi); // pass also level set, as p may be extended
        }
      } break;

      default:
        Stokes.InitVel( &Stokes.v, ZeroVel);
    }

    const double Vol= lset.GetVolume(); // approx. P.get<double>("Exp.Thickness") * P.get<DROPS::Point3DCL>("MeshSize")[0] * P.get<DROPS::Point3DCL>("MeshSize")[2];
    std::cout << "rel. Volume: " << lset.GetVolume()/Vol << std::endl;

   // Output-Registrations:
#ifndef _PAR
    Ensight6OutCL* ensight = NULL;
    if (P.get<int>("Ensight.EnsightOut",0)){
        // Initialize Ensight6 output
        std::string ensf( P.get<std::string>("Ensight.EnsDir") + "/" + P.get<std::string>("Ensight.EnsCase"));
        ensight = new Ensight6OutCL( P.get<std::string>("Ensight.EnsCase") + ".case",
                                     P.get<int>("Time.NumSteps")/P.get("Ensight.EnsightOut", 0)+1,
                                     P.get<int>("Ensight.Binary"));
        ensight->Register( make_Ensight6Geom      ( MG, MG.GetLastLevel(), P.get<std::string>("Ensight.GeomName"),
                                                    ensf + ".geo", true));
        ensight->Register( make_Ensight6Scalar    ( lset.GetSolution(),      "Levelset",      ensf + ".scl", true));
        ensight->Register( make_Ensight6Scalar    ( Stokes.GetPrSolution(),  "Pressure",      ensf + ".pr",  true));
        ensight->Register( make_Ensight6Vector    ( Stokes.GetVelSolution(), "Velocity",      ensf + ".vel", true));
        if (Stokes.UsesXFEM())
            ensight->Register( make_Ensight6P1XScalar( MG, lset.Phi, Stokes.p, "XPressure",   ensf + ".pr", true));

        ensight->Write( Stokes.v.t);
    }
#endif

    // writer for vtk-format
    VTKOutCL * vtkwriter = NULL;
    if (P.get<int>("VTK.VTKOut",0)){
        vtkwriter = new VTKOutCL(adap.GetMG(), "DROPS data",
                                 P.get<int>("Time.NumSteps")/P.get("VTK.VTKOut", 0)+1,
                                 P.get<std::string>("VTK.VTKDir"), P.get<std::string>("VTK.VTKName"),
                                 P.get<std::string>("VTK.TimeFileName"),
                                 P.get<int>("VTK.Binary"),
                                 P.get<int>("VTK.UseOnlyP1"),
                                 false,
                                 -1,  /* <- level */
                                 P.get<int>("VTK.ReUseTimeFile"),
                                 P.get<int>("VTK.UseDeformation"));
        vtkwriter->Register( make_VTKVector( Stokes.GetVelSolution(), "velocity") );
        vtkwriter->Register( make_VTKScalar( Stokes.GetPrSolution(), "pressure") );
        if (P.get<int>("VTK.AddP1XPressure",0) && Stokes.UsesXFEM())
            vtkwriter->Register( make_VTKP1XScalar( MG, lset.Phi, Stokes.p, "xpressure"));
        vtkwriter->Register( make_VTKScalar( lset.GetSolution(), "level-set") );

        vtkwriter->Write(Stokes.v.t);
    }

    std::ofstream* infofile = 0;
    IF_MASTER {
        infofile = new std::ofstream ((P.get<std::string>("VTK.VTKName","film")+".info").c_str());
    }
    //IFInfo.Init(infofile);
	FilmInfo.Init(infofile, P.get<DROPS::Point3DCL>("MeshSize")[0]/2., 0.);
    FilmInfo.WriteHeader();

    Stokes.SetupPrMass(  &Stokes.prM, lset);
    Stokes.SetupPrStiff( &Stokes.prA, lset);

    // Stokes-Solver
    StokesSolverFactoryCL<StokesProblemT> stokessolverfactory(Stokes, P);
    StokesSolverBaseCL* stokessolver = stokessolverfactory.CreateStokesSolver();

    // Navier-Stokes-Solver
    typedef NSSolverBaseCL<StokesProblemT> SolverT;
    SolverT * navstokessolver = 0;
    if (P.get("NavierStokes.Nonlinear", 0.0)==0.0)
        navstokessolver = new NSSolverBaseCL<StokesProblemT>(Stokes, *stokessolver);
    else
        navstokessolver = new AdaptFixedPtDefectCorrCL<StokesProblemT>(Stokes, *stokessolver, P.get<int>("NavierStokes.Iter"), P.get<double>("NavierStokes.Tol"), P.get<double>("NavierStokes.Reduction"));

    // Level-Set-Solver
#ifndef _PAR
    typedef GMResSolverCL<SSORPcCL> LsetSolverT; // In twophasedrops.cpp, the Gauss-Seidel preconditioner is used;
    SSORPcCL ssorpc;
    LsetSolverT* gm = new LsetSolverT( ssorpc, 100, P.get<int>("Levelset.Iter"), P.get<double>("Levelset.Tol"));
#else
    typedef ParPreGMResSolverCL<ParJac0CL> LsetSolverT;
    ParJac0CL jacparpc( *lidx);
    LsetSolverT *gm = new LsetSolverT
           (/*restart*/100, P.get<int>("Levelset.Iter"), P.get<double>("Levelset.Tol"), *lidx, jacparpc,/*rel*/true, /*acc*/ true, /*modGS*/false, LeftPreconditioning, /*parmod*/true);
#endif
    LevelsetModifyCL lsetmod( P.get<int>("Reparam.Freq"), P.get<int>("Reparam.Method"), /*rpm_MaxGrad*/ 1.0, /*rpm_MinGrad*/ 1.0, /*periodic*/ is_periodic);
    lset.InitVolume( Vol);

    LinThetaScheme2PhaseCL<LsetSolverT>
        cpl( Stokes, lset, *navstokessolver, *gm, lsetmod, P.get<double>("Time.StepSize"), P.get<double>("Stokes.Theta"), P.get<double>("Levelset.Theta"), P.get("NavierStokes.Nonlinear", 0.0), /*implicitCurv*/ true);

    if (P.get("NavierStokes.Nonlinear", 0.0)!=0.0 || P.get<int>("Time.NumSteps") == 0) {
        stokessolverfactory.SetMatrixA( &navstokessolver->GetAN()->GetFinest());
            //for Stokes-MGM
        stokessolverfactory.SetMatrices( navstokessolver->GetAN(), &Stokes.B.Data,
                                         &Stokes.M.Data, &Stokes.prM.Data, &Stokes.pr_idx);
    }
    else {
        stokessolverfactory.SetMatrixA( &cpl.GetUpperLeftBlock()->GetFinest());
            //for Stokes-MGM
        stokessolverfactory.SetMatrices( cpl.GetUpperLeftBlock(), &Stokes.B.Data,
                                         &Stokes.M.Data, &Stokes.prM.Data, &Stokes.pr_idx);
    }


    UpdateProlongationCL<Point3DCL> PVel( Stokes.GetMG(), stokessolverfactory.GetPVel(), &Stokes.vel_idx, &Stokes.vel_idx);
    adap.push_back( &PVel);
    UpdateProlongationCL<double> PPr ( Stokes.GetMG(), stokessolverfactory.GetPPr(), &Stokes.pr_idx, &Stokes.pr_idx);
    adap.push_back( &PPr);

    TwoPhaseStoreCL<InstatNavierStokes2PhaseP2P1CL> ser(MG, Stokes, lset, NULL,
                                                        P.get<std::string>("Restart.Outputfile"),
                                                        P.get<int>("Restart.Overwrite"),
                                                        P.get<int>("Restart.Binary"));

    Stokes.v.t += GetTimeOffset();

//    stokessolverfactory.GetVankaSmoother().SetRelaxation( 0.8);

    const int nsteps = P.get<int>("Time.NumSteps");
    const double dt = P.get<double>("Time.StepSize");
    DistMarkingStrategyCL marker( lset,
                                  P.get<double>("AdaptRef.Width"),
                                  P.get<int>("AdaptRef.CoarsestLevel"),
                                  P.get<int>("AdaptRef.FinestLevel") );
    adap.set_marking_strategy( &marker );
    for (int step= 1; step<=nsteps; ++step)
    {
        std::cout << "======================================================== Schritt " << step << ":\n";
        const double time_old = Stokes.v.t;
        const double time_new = Stokes.v.t + dt;
        FilmInfo.Update( lset, Stokes.GetVelSolution());
        FilmInfo.Write(time_old);
        cpl.DoStep( P.get<int>("Coupling.Iter"));
        std::cout << "rel. Volume: " << lset.GetVolume()/Vol << std::endl;

        bool doGridMod= P.get<int>("AdaptRef.Freq") && step%P.get<int>("AdaptRef.Freq") == 0;

        // grid modification
        if (doGridMod) {
            adap.UpdateTriang();
            cpl.Update();
        }

        if (ensight && step%10==0)
            ensight->Write(time_new);
        if (vtkwriter && step%1==0)
            vtkwriter->Write(time_new);
        if (P.get("Restart.Serialization", 0) && step%P.get("Restart.Serialization", 0)==0)
            ser.Write();
    }

    FilmInfo.Update( lset, Stokes.GetVelSolution());
    FilmInfo.Write(Stokes.v.t);
    std::cout << std::endl;
    if (ensight ) delete ensight;
    if (vtkwriter) delete vtkwriter;
    delete stokessolver;
    delete navstokessolver;
}

} // end of namespace DROPS


void MarkFilm (DROPS::MultiGridCL& mg, DROPS::InstatScalarFunction distanceFct, DROPS::Uint maxLevel= ~0, double t=0.)
{
    for (DROPS::MultiGridCL::TriangTetraIteratorCL It(mg.GetTriangTetraBegin(maxLevel)),
             ItEnd(mg.GetTriangTetraEnd(maxLevel)); It!=ItEnd; ++It)
    {
        bool ref= false;
        int num_pos= 0;
        for (int i=0; i<4; ++i)
        {
            const double d= distanceFct( It->GetVertex(i)->GetCoord(), t);
            if (d<1e-4)
                ref= true;
            num_pos+= d>0;
        }
        if ( distanceFct( GetBaryCenter(*It), t)<1e-4 )
            ref= true;
        if (num_pos!=4 && num_pos!=0)
            ref= true;
        if (ref)
            It->SetRegRefMark();
    }
}


void MarkLower (DROPS::MultiGridCL& mg, double y_max, DROPS::Uint maxLevel= ~0)
{
    for (DROPS::MultiGridCL::TriangTetraIteratorCL It(mg.GetTriangTetraBegin(maxLevel)),
             ItEnd(mg.GetTriangTetraEnd(maxLevel)); It!=ItEnd; ++It)
    {
        if ( GetBaryCenter(*It)[1] < y_max )
            It->SetRegRefMark();
    }
}



int main (int argc, char** argv)
{
  try
  {
    DROPS::read_parameter_file_from_cmdline( P, argc, argv, "../../param/levelset/film/film.json");
    P.put_if_unset<std::string>("VTK.TimeFileName",P.get<std::string>("VTK.VTKName"));
    std::cout << P << std::endl;

    DROPS::dynamicLoad(P.get<std::string>("General.DynamicLibsPrefix"), P.get<std::vector<std::string> >("General.DynamicLibs") );

    //DIDNT FIND A PARAM WITH PeriodicMatching, so I didnt know the type
    DROPS::match_fun periodic_match = DROPS::MatchMap::getInstance()[P.get<std::string>("DomainCond.PeriodicMatching", std::string("periodicxz"))];

    typedef DROPS::InstatNavierStokes2PhaseP2P1CL MyStokesCL;

    DROPS::Point3DCL orig, e1, e2, e3;
    orig[2]= -P.get<DROPS::Point3DCL>("MeshSize")[2]/2;
    e1[0]= P.get<DROPS::Point3DCL>("MeshSize")[0];
    e2[1]= P.get<DROPS::Point3DCL>("MeshSize")[1];
    e3[2]= P.get<DROPS::Point3DCL>("MeshSize")[2];
    DROPS::BrickBuilderCL builder( orig, e1, e2, e3, int( P.get<DROPS::Point3DCL>("MeshResolution")[0]), int( P.get<DROPS::Point3DCL>("MeshResolution")[1]), int( P.get<DROPS::Point3DCL>("MeshResolution")[2]) );
    DROPS::MultiGridCL* mgp;
    if (P.get<std::string>("Restart.Inputfile") == "none")
        mgp= new DROPS::MultiGridCL( builder);
    else {
        DROPS::FileBuilderCL filebuilder( P.get<std::string>("Restart.Inputfile"), &builder);
        mgp= new DROPS::MultiGridCL( filebuilder);
    }

    if (P.get<std::string>("BndCond").size()!=6)
    {
        std::cerr << "too many/few bnd conditions!\n"; return 1;
    }
    DROPS::BndCondT bc[6], bc_ls[6];
    DROPS::BoundaryCL::BndTypeCont bndType;
    DROPS::StokesVelBndDataCL::bnd_val_fun bnd_fun[6];
    bool is_periodic= false;

    DROPS::InVecMap & vecmap =  DROPS::InVecMap::getInstance();
    DROPS::StokesVelBndDataCL::bnd_val_fun ZeroVel = vecmap["ZeroVel"];
    DROPS::StokesVelBndDataCL::bnd_val_fun Inflow = vecmap["FilmInflow"];
    for (int i=0; i<6; ++i)
    {
        bc_ls[i]= DROPS::Nat0BC;
        switch(P.get<std::string>("BndCond")[i])
        {
            case 'w': case 'W':
                bc[i]= DROPS::WallBC;    bnd_fun[i]= ZeroVel; bndType.push_back( DROPS::BoundaryCL::OtherBnd); break;
            case 'i': case 'I':
                bc[i]= DROPS::DirBC;     bnd_fun[i]= Inflow;         bndType.push_back( DROPS::BoundaryCL::OtherBnd); break;
            case 'o': case 'O':
                bc[i]= DROPS::OutflowBC; bnd_fun[i]= ZeroVel; bndType.push_back( DROPS::BoundaryCL::OtherBnd); break;
            case '1':
                is_periodic= true;
                bc_ls[i]= bc[i]= DROPS::Per1BC;    bnd_fun[i]= ZeroVel; bndType.push_back( DROPS::BoundaryCL::Per1Bnd); break;
            case '2':
                is_periodic= true;
                bc_ls[i]= bc[i]= DROPS::Per2BC;    bnd_fun[i]= ZeroVel; bndType.push_back( DROPS::BoundaryCL::Per2Bnd); break;
            default:
                std::cerr << "Unknown bnd condition \"" << P.get<std::string>("BndCond")[i] << "\"\n";
                return 1;
        }
    }

    MyStokesCL prob( *mgp, P, DROPS::StokesBndDataCL( 6, bc, bnd_fun, bc_ls), DROPS::P1X_FE, P.get<double>("Stokes.XFEMStab"), DROPS::vecP2_FE, P.get<double>("Stokes.epsP",0.0));

    const DROPS::BoundaryCL& bnd= mgp->GetBnd();
    bnd.SetPeriodicBnd( bndType, periodic_match);

    sigma= prob.GetCoeff().SurfTens;
    DROPS::SurfaceTensionCL sf( sigmaf, 0);
    DROPS::LevelsetP2CL & lset( * DROPS::LevelsetP2CL::Create( *mgp, DROPS::LsetBndDataCL( 6, bc_ls), sf, P.get_child("Levelset")) );

    for (DROPS::BndIdxT i=0, num= bnd.GetNumBndSeg(); i<num; ++i)
    {
        std::cout << "Bnd " << i << ": "; BndCondInfo( bc[i], std::cout);
    }

    DROPS::AdapTriangCL adap( *mgp, 0,
                              P.get<std::string>("Restart.Inputfile") == "none" ?  P.get<int>("AdaptRef.LoadBalStrategy") :
                                                                                  -P.get<int>("AdaptRef.LoadBalStrategy") );

    // If we read the Multigrid, it shouldn't be modified;
    // otherwise the pde-solutions from the ensight files might not fit.
    if ( P.get<std::string>("Restart.Inputfile") == "none")
    {
        DROPS::DistMarkingStrategyCL InitialMarker( DROPS::InScaMap::getInstance()[P.get("Ext.InitialLet", std::string("WavyFilm"))],
                                                    P.get<double>("AdaptRef.Width"), P.get<int>("AdaptRef.CoarsestLevel"),
                                                    P.get<int>("AdaptRef.FinestLevel") );
        adap.set_marking_strategy( &InitialMarker );
        adap.MakeInitialTriang();
        adap.set_marking_strategy( 0 );
    }

    std::cout << DROPS::SanityMGOutCL(*mgp) << std::endl;
    mgp->SizeInfo( std::cout);
    std::cout << "Film Reynolds number Re_f = "
              << P.get<double>("Mat.DensFluid")*P.get<double>("Mat.DensFluid")*P.get<DROPS::Point3DCL>("Exp.Gravity")[0]*std::pow(P.get<double>("Exp.Thickness"),3)/P.get<double>("Mat.ViscFluid")/P.get<double>("Mat.ViscFluid")/3 << std::endl;
    std::cout << "max. inflow velocity at film surface = "
              << P.get<double>("Mat.DensFluid")*P.get<DROPS::Point3DCL>("Exp.Gravity")[0]*P.get<double>("Exp.Thickness")*P.get<double>("Exp.Thickness")/P.get<double>("Mat.ViscFluid")/2 << std::endl;
    Strategy( prob, lset, adap, is_periodic);  // do all the stuff

    double min= prob.p.Data.min(),
           max= prob.p.Data.max();
    std::cout << "pressure min/max: "<<min<<", "<<max<<std::endl;
    delete &lset;
    return 0;
  }
  catch (DROPS::DROPSErrCL err) { err.handle(); }
}

