/*

Copyright (C) 2010   Lukas F. Reichlin

This file is part of LTI Syncope.

LTI Syncope is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LTI Syncope is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Solution of generalized Lyapunov equations.
Uses SLICOT SG03AD by courtesy of NICONET e.V.
<http://www.slicot.org>

Author: Lukas Reichlin <lukas.reichlin@gmail.com>
Created: January 2010
Version: 0.1

*/

#include <octave/oct.h>
#include <f77-fcn.h>

extern "C"
{ 
    int F77_FUNC (sg03ad, SG03AD)
                 (char& DICO, char& JOB,
                  char& FACT, char& TRANS,
                  char& UPLO,
                  int& N,
                  double* A, int& LDA,
                  double* E, int& LDE,
                  double* Q, int& LDQ,
                  double* Z, int& LDZ,
                  double* X, int& LDX,
                  double& SCALE,
                  double& SEP, double& FERR,
                  double* ALPHAR, double* ALPHAI,
                  double* BETA,
                  int* IWORK,
                  double* DWORK, int& LDWORK,
                  int& INFO);
}

int max (int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}
     
DEFUN_DLD (slsg03ad, args, nargout, "Slicot SG03AD Release 5.0")
{
    int nargin = args.length ();
    octave_value_list retval;
    
    if (nargin != 4)
    {
        print_usage ();
    }
    else
    {
        // arguments in
        char dico;
        char job = 'X';
        char fact = 'N';
        char trans = 'T';
        char uplo = 'U';    // ?!?
        
        NDArray a = args(0).array_value ();
        NDArray e = args(1).array_value ();
        NDArray x = args(2).array_value ();
        int dt = args(3).int_value ();
        
        if (dt == 0)
          dico = 'C';
        else
          dico = 'D';
        
        int n = a.rows ();      // n: number of states
        
        int lda = max (1, n);
        int lde = max (1, n);
        int ldq = max (1, n);
        int ldz = max (1, n);
        int ldx = max (1, n);
                
        // arguments out
        double scale;
        double sep = 0;
        double ferr = 0;
        
        dim_vector dv_q (2);
        dv_q(0) = ldq;
        dv_q(1) = n;
        
        dim_vector dv_z (2);
        dv_z(0) = ldz;
        dv_z(1) = n;
        
        dim_vector dv (1);
        dv(0) = n;
        
        NDArray q (dv_q);
        NDArray z (dv_z);
        NDArray alphar (dv);
        NDArray alphai (dv);
        NDArray beta (dv);
        
        // workspace
        int* iwork = 0;  // not referenced because job = X
        
        int ldwork = max (1, 4*n);
        OCTAVE_LOCAL_BUFFER (double, dwork, ldwork);

        // error indicator
        int info;
        

        // SLICOT routine SG03AD
        F77_XFCN (sg03ad, SG03AD,
                 (dico, job,
                  fact, trans,
                  uplo,
                  n,
                  a.fortran_vec (), lda,
                  e.fortran_vec (), lde,
                  q.fortran_vec (), ldq,
                  z.fortran_vec (), ldz,
                  x.fortran_vec (), ldx,
                  scale,
                  sep, ferr,
                  alphar.fortran_vec (), alphai.fortran_vec (),
                  beta.fortran_vec (),
                  iwork,
                  dwork, ldwork,
                  info));

        if (f77_exception_encountered)
            error ("lyap: slsg03ad: exception in SLICOT subroutine SG03AD");

        if (info != 0)
            error ("lyap: slsg03ad: SG03AD returned info = %d", info);
        
        // return values
        retval(0) = x;
        retval(1) = octave_value (scale);
    }
    
    return retval;
}