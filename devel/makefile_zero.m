## ==============================================================================
## Developer Makefile for OCT-files
## ==============================================================================
## USAGE: * fetch control from Octave-Forge by svn
##        * add control/inst, control/src and control/devel to your Octave path
##        * run makefile_*
## NOTES: * The option "-Wl,-framework" "-Wl,vecLib" is needed for MacPorts'
##          octave-devel @3.3.52_1+gcc44 on MacOS X 10.6.4. However, this option
##          breaks other platforms. See MacPorts Ticket #26640.
## ==============================================================================

homedir = pwd ();
develdir = fileparts (which ("makefile_zero"));
srcdir = [develdir, "/../src"];
cd (srcdir);

## transmission zeros of state-space models
mkoctfile "-Wl,-framework" "-Wl,vecLib" \
          slab08nd.cc \
          AB08ND.f AB08NX.f TB01ID.f MB03OY.f MB03PY.f

## transmission zeros of descriptor state-space models
mkoctfile "-Wl,-framework" "-Wl,vecLib" \
          slag08bd.cc \
          AG08BD.f AG08BY.f TG01AD.f TB01XD.f MA02CD.f \
          TG01FD.f MA02BD.f MB03OY.f

cd (homedir);