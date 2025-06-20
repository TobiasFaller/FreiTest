This repository holds the tools to compile shared object files (.so), that will be used in the Freitest project as insertable sat-solvers.

The target of the project was to enable the exchanging of the used solver of freitest, without the need of recompilating the source.

The compilation of the shared object files are handled in the adjacent Makefile.


The solvers have to be handled via the ipasir-api, to be plugged into the freitest project.


Clone with flag --recurse-submodules.
