/// \file newton.h
/// \brief The damped Newton method as class template.
/// \author Joerg Grande

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
 * Copyright 2016 Joerg Grande, Aachen, Germany
*/


#include "misc/problem.h"

namespace DROPS
{

namespace NewtonImplNS {

template <typename T>
struct dotCL
{};

template <>
struct dotCL<double>
{
    static double dot (double x, double y) { return x*y; }
};

template <Uint Size>
struct dotCL< SVectorCL<Size> >
{
    static double dot (const SVectorCL<Size>& x, const SVectorCL<Size>& y) { return inner_prod (x, y); }
};

template <typename T>
double dot (T x, T y)
{
    return dotCL<T>::dot (x, y);
}

} // end of namespace NewtonImplNS


/// FunctionT must define: * typedef for value_type,
///     * set_point (x),
///     * value (),
///     * apply_derivative (v),
///     * apply_derivative_inverse (v),
///     * initial_damping_factor (dx, F) (must return a nonnegative value which is further limited to 1 in the algorithm).
template <typename FunctionT>
void newton_solve (FunctionT& fun, typename FunctionT::value_type& x, size_t& maxiter, double& tol, size_t& max_damping_steps, double armijo_c, std::ostream* os= 0)
{
    const double min_step_length= 1e-7; // Minimum value for step-length factor l.

    typedef typename FunctionT::value_type value_type;
    value_type dx, // Newton correction.
               F,  // The function of which we search a root.
               Fnew; // Helper in the step-size computation.
    double l= 1.; // Damping factor for line search.
    size_t total_damping_iter= 0; // Count all rejected steps sizes.
    double normF;

    size_t iter;
    for (iter= 0; true; ++iter) {
        fun.set_point (x);
        F= fun.value ();
        normF= std::sqrt (NewtonImplNS::dot (F, F));
        if (iter >= maxiter || normF < tol)
            break;
        dx= fun.apply_derivative_inverse (F);
//         if (iter == 0) // Initial gradient descent step
//             dx= fun.apply_derivative_transpose (F/normF);
        const double armijo_slope= armijo_c*inner_prod( F, fun.apply_derivative (dx))/normF;
        // Armijo-rule with backtracking
        l= std::min( 1., fun.initial_damping_factor (dx, F));
        Uint j;
        for (j= 0; j < max_damping_steps && l >= min_step_length; ++j, l*= 0.5) {
            if (!fun.set_point (x - l*dx))
                continue;
            Fnew= fun.value ();
            if (std::sqrt (NewtonImplNS::dot (Fnew, Fnew)) < normF + armijo_slope*l)
                break;
        }
        total_damping_iter+= j;
        if (l < min_step_length) {
//             std::cerr << "newton_solve: Too much damping. iter: " << iter << " x: " << x << " dx: " << dx << " l: " << l << " F: " << F << std::endl;
            break;
        }
        if (os)
            (*os) << "iter: " << iter << " x: " << x << " dx: " << dx << " l: " << l << " F: " << F << std::endl;
        x-= l*dx;
    }
    if (iter >= maxiter)
        std::cout << "newton_solve: max iteration number exceeded; \tx: " << x << "\t dx: " << dx << "\tl: " << l << "\t F: " << F << std::endl;
    if (normF >= tol)
        std::cout << "newton_solve: no convergence; tol: " << tol << " normF: " << normF << std::endl;

    maxiter= iter;
    tol= normF;
    max_damping_steps= total_damping_iter;
}

} // end of namespace DROPS
