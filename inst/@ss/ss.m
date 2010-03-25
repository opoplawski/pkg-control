## Copyright (C) 2009   Lukas F. Reichlin
##
## This file is part of LTI Syncope.
##
## LTI Syncope is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## LTI Syncope is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program. If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {Function File} {@var{sys} =} ss (@var{sys})
## @deftypefnx {Function File} {@var{sys} =} ss (@var{a}, @var{b}, @var{c})
## @deftypefnx {Function File} {@var{sys} =} ss (@var{a}, @var{b}, @var{c}, @var{d})
## @deftypefnx {Function File} {@var{sys} =} ss (@var{a}, @var{b}, @var{c}, @var{d}, @var{tsam})
## Create or convert to state-space model.
## @end deftypefn

## Author: Lukas Reichlin <lukas.reichlin@gmail.com>
## Created: September 2009
## Version: 0.1

function sys = ss (a, b, c, d, varargin)

  ## model precedence: frd > ss > zpk > tf > double
  %inferiorto ("frd");
  superiorto ("zpk", "tf", "double");

  argc = 0;

  switch (nargin)
    case 0
      a = [];
      b = [];
      c = [];
      d = [];
      tsam = -1;

    case 1
      if (isa (a, "ss"))  # already in ss form
        sys = a;
        return;
      elseif (isa (a, "lti"))  # another lti object
        [sys, alti] = __sys2ss__ (a);
        sys.lti = alti;  # preserve lti properties
        return;
      elseif (isnumeric (a))  # static gain
        d = a;
        a = [];
        b = zeros (0, columns (d));
        c = zeros (rows (d), 0);
        tsam = -1;
      else
        print_usage ();
      endif

    case 2
      print_usage ();

    case 3  # a, b, c without d
      d = zeros (rows (c), columns (b));
      tsam = 0;

    case 4  # continuous system
      tsam = 0;
      [b, c] = __gaincheck__ (b, c, d);

    otherwise  # default case
      [b, c] = __gaincheck__ (b, c, d);
      argc = numel (varargin);

      if (issample (varargin{1}, 1))  # sys = ss (a, b, c, d, tsam, "prop1, "val1", ...)
        tsam = varargin{1};
        argc--;

        if (argc > 0)
          varargin = varargin(2:end);
        endif
      else  # sys = ss (a, b, c, d, "prop1, "val1", ...)
        tsam = 0;
      endif

  endswitch


  if (isempty (a))  # static system
    tsam = -1;
  endif

  [nu, nx, ny] = __ssmatdim__ (a, b, c, d);

  stname = repmat ({""}, nx, 1);

  ssdata = struct ("a", a,
                   "b", b,
                   "c", c,
                   "d", d,
                   "stname", {stname});

  ltisys = lti (ny, nu, tsam);

  sys = class (ssdata, "ss", ltisys);

  if (argc > 0)
    sys = set (sys, varargin{:});
  endif

endfunction


function [b, c] = __gaincheck__ (b, c, d)

  ## catch the case sys = ss ([], [], [], d)
  if (isempty (b) && isempty (c))
    b = zeros (0, columns (d));
    c = zeros (rows(d), 0);
  endif

endfunction