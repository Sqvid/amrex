#include <iostream>

#include <BoxLib.H>
#include <MultiFab.H>
#include <MultiFabUtil.H>
#include <BLFort.H>
#include <FMultiGrid.H>

void 
solve_with_f90(PArray<MultiFab>& rhs, PArray<MultiFab>& phi, 
               Array< PArray<MultiFab> >& grad_phi_edge, 
               const Array<Geometry>& geom, int base_level, int finest_level, Real tol, Real abs_tol)
{
    int nlevs = finest_level - base_level + 1;

    int mg_bc[2*BL_SPACEDIM];

    // This tells the solver that we are using periodic bc's
    if (Geometry::isAllPeriodic())
    {
        for (int dir = 0; dir < BL_SPACEDIM; ++dir)
        {
            mg_bc[2*dir + 0] = 0;
            mg_bc[2*dir + 1] = 0;
        }
    } else {
	BoxLib::Abort("non periodic boundraies not supported here");
    }

    // Have to do some packing because these arrays does not always start with base_level
    PArray<Geometry> geom_p(nlevs);
    PArray<MultiFab> rhs_p(nlevs);
    PArray<MultiFab> phi_p(nlevs);
    for (int ilev = 0; ilev < nlevs; ++ilev) {
	geom_p.set(ilev, &geom[ilev+base_level]);
	rhs_p.set (ilev,  &rhs[ilev+base_level]);
	phi_p.set (ilev,  &phi[ilev+base_level]);
    }
    
    // Refinement ratio is hardwired to 2 here.
    IntVect crse_ratio = (base_level == 0) ? 
	IntVect::TheZeroVector() : IntVect::TheUnitVector() * 2;

    FMultiGrid fmg(geom_p, base_level, crse_ratio);

    if (base_level == 0) {
	fmg.set_bc(mg_bc, phi[base_level]);
    } else {
	fmg.set_bc(mg_bc, phi[base_level-1], phi[base_level]);
    }

    fmg.set_const_gravity_coeffs();

    int always_use_bnorm = 0;
    int need_grad_phi = 1;
    fmg.set_verbose(0);
    fmg.solve(phi_p, rhs_p, tol, abs_tol, always_use_bnorm, need_grad_phi);
   
    for (int ilev = 0; ilev < nlevs; ++ilev)
    {
	int amr_level = ilev + base_level;
	fmg.get_fluxes(grad_phi_edge[amr_level], ilev);
    }
}
