/*

Copyright (C) 2009-2016   Lukas F. Reichlin

This file is part of LTI Syncope.

LTI Syncope is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LTI Syncope is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LTI Syncope.  If not, see <http://www.gnu.org/licenses/>.

Pole assignment for a given matrix pair (A,B).
Uses SLICOT SB01BD by courtesy of NICONET e.V.
<http://www.slicot.org>

Author: Lukas Reichlin <lukas.reichlin@gmail.com>
Created: November 2009
Version: 0.6

*/

#include <octave/oct.h>
#include <f77-fcn.h>
#include "common.h"

extern "C"
{ 
    int F77_FUNC (sb01bd, SB01BD)
                 (char& DICO,
                  octave_idx_type& N, octave_idx_type& M, octave_idx_type& NP,
                  double& ALPHA,
                  double* A, octave_idx_type& LDA,
                  double* B, octave_idx_type& LDB,
                  double* WR, double* WI,
                  octave_idx_type& NFP, octave_idx_type& NAP, octave_idx_type& NUP,
                  double* F, octave_idx_type& LDF,
                  double* Z, octave_idx_type& LDZ,
                  double& TOL,
                  double* DWORK, octave_idx_type& LDWORK,
                  octave_idx_type& IWARN, octave_idx_type& INFO);
}

// PKG_ADD: autoload ("__sl_sb01bd__", "__control_slicot_functions__.oct");     
DEFUN_DLD (__sl_sb01bd__, args, nargout,
   "-*- texinfo -*-\n\
Slicot SB01BD Release 5.0\n\
No argument checking.\n\
For internal use only.")
{
    octave_idx_type nargin = args.length ();
    octave_value_list retval;
    
    if (nargin != 7)
    {
        print_usage ();
    }
    else
    {
        // arguments in
        char dico;
        
        Matrix a = args(0).matrix_value ();
        Matrix b = args(1).matrix_value ();
        ColumnVector wr = args(2).column_vector_value ();
        ColumnVector wi = args(3).column_vector_value ();
        octave_idx_type discrete = args(4).int_value ();
        double alpha = args(5).double_value ();
        double tol = args(6).double_value ();
        
        if (discrete == 1)
            dico = 'D';
        else
            dico = 'C';
        
        octave_idx_type n = a.rows ();      // n: number of states
        octave_idx_type m = b.columns ();   // m: number of inputs
        octave_idx_type np = wr.rows ();
        
        octave_idx_type lda = max (1, a.rows ());
        octave_idx_type ldb = max (1, b.rows ());
        octave_idx_type ldf = max (1, m);
        octave_idx_type ldz = max (1, n);
        
        // arguments out
        octave_idx_type nfp;
        octave_idx_type nap;
        octave_idx_type nup;
        
        Matrix f (ldf, n);
        Matrix z (ldz, n);
        
        // workspace
        octave_idx_type ldwork = max (1, 5*m, 5*n, 2*n+4*m);
        
        OCTAVE_LOCAL_BUFFER (double, dwork, ldwork);
        
        // error indicators
        octave_idx_type iwarn;
        octave_idx_type info;


        // SLICOT routine SB01BD
        F77_XFCN (sb01bd, SB01BD,
                 (dico,
                  n, m, np,
                  alpha,
                  a.fortran_vec (), lda,
                  b.fortran_vec (), ldb,
                  wr.fortran_vec (), wi.fortran_vec (),
                  nfp, nap, nup,
                  f.fortran_vec (), ldf,
                  z.fortran_vec (), ldz,
                  tol,
                  dwork, ldwork,
                  iwarn, info));

        if (f77_exception_encountered)
            error ("place: __sl_sb01bd__: exception in SLICOT subroutine SB01BD");
            
        static const char* err_msg[] = {
            "0: OK",
            "1: the reduction of A to a real Schur form failed.",
            "2: a failure was detected during the ordering of the "
                "real Schur form of A, or in the iterative process "
                "for reordering the eigenvalues of Z'*(A + B*F)*Z "
                "along the diagonal.",
            "3: the number of eigenvalues to be assigned is less "
                "than the number of possibly assignable eigenvalues; "
                "NAP eigenvalues have been properly assigned, "
                "but some assignable eigenvalues remain unmodified.",
            "4: an attempt is made to place a complex conjugate "
                "pair on the location of a real eigenvalue. This "
                "situation can only appear when N-NFP is odd, "
                "NP > N-NFP-NUP is even, and for the last real "
                "eigenvalue to be modified there exists no available "
                "real eigenvalue to be assigned. However, NAP "
                "eigenvalues have been already properly assigned."};

        static const char* warn_msg[] = {
            "0: OK",
/* 0+%d: %d */  "violations of the numerical stability condition "
                "NORM(F) <= 100*NORM(A)/NORM(B) occured during the "
                "assignment of eigenvalues."};

        error_msg ("place", info, 4, err_msg);
        warning_msg ("place", iwarn, 0, warn_msg, 0);


        // return values
        retval(0) = f;
        retval(1) = octave_value (nfp);
        retval(2) = octave_value (nap);
        retval(3) = octave_value (nup);
        retval(4) = z;
    }
    
    return retval;
}
