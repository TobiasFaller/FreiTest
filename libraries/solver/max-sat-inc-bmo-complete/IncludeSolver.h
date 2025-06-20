#ifndef INCLUDE_SOLVER_H
#define INCLUDE_SOLVER_H

#if __has_include(<glucose-4.0/core/SolverTypes.h>)
#include "glucose-4.0/core/SolverTypes.h"
#include "glucose-4.0/core/Solver.h"
#include "glucose-4.0/utils/System.h"
#ifdef SIMP
# include "glucose-4.0/simp/SimpSolver.h"
#endif
#endif

#if __has_include(<glucose-4.1/core/SolverTypes.h>)
#include "glucose-4.1/core/SolverTypes.h"
#include "glucose-4.1/core/Solver.h"
#include "glucose-4.1/utils/System.h"
#ifdef SIMP
# include "glucose-4.1/simp/SimpSolver.h"
#endif
#endif

#if __has_include(<glucose-4.2.1/core/SolverTypes.h>)
#include "glucose-4.2.1/core/SolverTypes.h"
#include "glucose-4.2.1/core/Solver.h"
#include "glucose-4.2.1/utils/System.h"
#ifdef SIMP
# include "glucose-4.2.1/simp/SimpSolver.h"
#endif
#endif

#if __has_include(<minisat-2.2.0/core/SolverTypes.h>)
#include "minisat-2.2.0/core/SolverTypes.h"
#include "minisat-2.2.0/core/Solver.h"
#include "minisat-2.2.0/utils/System.h"
#ifdef SIMP
# include "minisat-2.2.0/simp/SimpSolver.h"
#endif
#endif

#endif
