/*!
 * \author Ruben Martins - ruben@sat.inesc-id.pt
 *
 * @section LICENSE
 *
 * MiniSat,  Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
 *           Copyright (c) 2007-2010, Niklas Sorensson
 * Open-WBO, Copyright (c) 2013-2017, Ruben Martins, Vasco Manquinho, Ines Lynce
 *           Copyright (c) 2018  Prateek Kumar, Sukrut Rao
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "bmo-glucose/utils/Options.h"
#include "bmo-glucose/utils/ParseUtils.h"
#include "bmo-glucose/utils/System.h"
#include <errno.h>
#include <signal.h>
#include <zlib.h>

#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef SIMP
#include "bmo-glucose/simp/SimpSolver.h"
#else
#include "bmo-glucose/core/Solver.h"
#endif

#include "MaxSAT.h"
#include "MaxTypes.h"
#include "ParserMaxSAT.h"
#include "ParserPB.h"

// Algorithms
#include "algorithms/Alg_LinearSU.h"
#include "algorithms/Alg_LinearSU_IncBMO.h"
#include "algorithms/Alg_LinearSU_IncCluster.h"
#include "algorithms/Alg_MSU3.h"
#include "algorithms/Alg_OLL.h"
#include "algorithms/Alg_OLL_IncCluster.h"
#include "algorithms/Alg_PartMSU3.h"
#include "algorithms/Alg_WBO.h"
#include "algorithms/Alg_OBV.h"
#include "algorithms/Alg_BLS.h"

#define VER1_(x) #x
#define VER_(x) VER1_(x)
#define SATVER VER_(SOLVERNAME)
#define VER VER_(VERSION)

using BMO_SOLVER_NSPACE::BoolOption;
using BMO_SOLVER_NSPACE::IntOption;
using BMO_SOLVER_NSPACE::IntRange;
using BMO_SOLVER_NSPACE::OutOfMemoryException;
using BMO_SOLVER_NSPACE::StringOption;
using BMO_SOLVER_NSPACE::cpuTime;
using BMO_SOLVER_NSPACE::parseOptions;
using namespace openwbo;

//=================================================================================================

static MaxSAT *mxsolver;

static void SIGINT_exit(int signum) {
  mxsolver->printAnswer(_UNKNOWN_);
  exit(_UNKNOWN_);
}

//=================================================================================================
// Main:

int main(int argc, char **argv) {
  printf(
      "c\nc Open-WBO:\t a Modular MaxSAT Solver -- based on %s (%s version)\n",
      SATVER, VER);
  printf("c Version:\t Inc-BMO-Complete -- MaxSAT Evaluation 2019\n");
  printf("c Authors:\t Saurabh Joshi, Prateek Kumar, Ruben Martins, Sukrut Rao\n");
  printf("c Contributors:\t Alexander Nadel, Vasco Manquinho\n");
  printf("c Contact:\t open-wbo@sat.inesc-id.pt -- "
         "http://sat.inesc-id.pt/open-wbo/\nc\n");
  try {
    BMO_SOLVER_NSPACE::setUsageHelp("c USAGE: %s [options] <input-file>\n\n");

#if defined(__linux__)
    fpu_control_t oldcw, newcw;
    _FPU_GETCW(oldcw);
    newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE;
    _FPU_SETCW(newcw);
    printf(
        "c WARNING: for repeatability, setting FPU to use double precision\n");
#endif

    BoolOption printmodel("Open-WBO", "print-model", "Print model.\n", true);

    IntOption verbosity("Open-WBO", "verbosity",
                        "Verbosity level (0=minimal, 1=more).\n", 0,
                        IntRange(0, 1));

    IntOption algorithm("Open-WBO", "algorithm",
                        "Search algorithm "
                        "(0=wbo,1=linear-su,2=msu3,3=part-msu3,4=oll,5=best,6="
                        "bmo,7=obv,8=mcs)\n",
                        6, IntRange(0, 8));

    IntOption partition_strategy("PartMSU3", "partition-strategy",
                                 "Partition strategy (0=sequential, "
                                 "1=sequential-sorted, 2=binary)"
                                 "(only for unsat-based partition algorithms).",
                                 2, IntRange(0, 2));

    IntOption graph_type("PartMSU3", "graph-type",
                         "Graph type (0=vig, 1=cvig, 2=res) (only for unsat-"
                         "based partition algorithms).",
                         2, IntRange(0, 2));

    BoolOption bmo("Open-WBO", "bmo", "BMO search.\n", true);

    BoolOption compact("Open-WBO","compact","Compact model printing.\n", false);

    BoolOption complete("Open-WBO-Inc-BMO","complete","Switch to complete algorithm when Inc-BMO terminates.\n",true);

    IntOption cardinality("Encodings", "cardinality",
                          "Cardinality encoding (0=cardinality networks, "
                          "1=totalizer, 2=modulo totalizer).\n",
                          1, IntRange(0, 2));

    IntOption amo("Encodings", "amo", "AMO encoding (0=Ladder).\n", 0,
                  IntRange(0, 0));

    IntOption pb("Encodings", "pb", "PB encoding (0=SWC,1=GTE,2=GTECluster).\n",
                 1, IntRange(0, 2));

    IntOption formula("Open-WBO", "formula",
                      "Type of formula (0=WCNF, 1=OPB).\n", 0, IntRange(0, 1));

    IntOption weight(
        "WBO", "weight-strategy",
        "Weight strategy (0=none, 1=weight-based, 2=diversity-based).\n", 2,
        IntRange(0, 2));

    BoolOption symmetry("WBO", "symmetry", "Symmetry breaking.\n", true);

    IntOption symmetry_lim(
        "WBO", "symmetry-limit",
        "Limit on the number of symmetry breaking clauses.\n", 500000,
        IntRange(0, INT32_MAX));

    IntOption cluster_algorithm("Clustering", "ca",
                                "Clustering algorithm "
                                "(0=none, 1=DivisiveMaxSeparate)",
                                1, IntRange(0, 1));
    IntOption num_clusters("Clustering", "c", "Number of agglomerated clusters",
                           100000, IntRange(1, INT_MAX));

    IntOption rounding_strategy(
        "Clustering", "rs",
        "Statistic used to select"
        " common weights in a cluster (0=Mean, 1=Median, 2=Min)",
        0, IntRange(0, 2));

    IntOption num_conflicts(
      "Incomplete","conflicts","Limit on the number of conflicts.\n", 10000,
      IntRange(0, INT32_MAX));

    IntOption num_iterations(
      "Incomplete","iterations","Limit on the number of iterations.\n", 100000,
      IntRange(0, INT32_MAX));

    BoolOption local("Incomplete", "local", "Local limit on the number of conflicts.\n", false);

    parseOptions(argc, argv, true);

    double initial_time = cpuTime();
    MaxSAT *S = NULL;

    Statistics rounding_statistic =
        static_cast<Statistics>((int)rounding_strategy);

    switch ((int)algorithm) {
    case _ALGORITHM_WBO_:
      S = new WBO(verbosity, weight, symmetry, symmetry_lim);
      break;

    case _ALGORITHM_LINEAR_SU_:
      if ((int)(cluster_algorithm) == 1) {
        S = new LinearSUIncCluster(verbosity, bmo, cardinality, pb,
                            ClusterAlg::_DIVISIVE_, rounding_statistic,
                            (int)(num_clusters));
      } else {
        S = new LinearSU(verbosity, bmo, cardinality, pb);
      }
      break;

    case _ALGORITHM_PART_MSU3_:
      S = new PartMSU3(verbosity, partition_strategy, graph_type, cardinality);
      break;

    case _ALGORITHM_MSU3_:
      S = new MSU3(verbosity);
      break;

    case _ALGORITHM_LSU_INCBMO_:
      S = new LinearSUIncBMO(verbosity, bmo, cardinality, pb,
                                 ClusterAlg::_DIVISIVE_, rounding_statistic,
                                 (int)(num_clusters), complete);
      break;

    case _ALGORITHM_LSU_MRSBEAVER_:
      S = new OBV(verbosity, cardinality, num_conflicts, num_iterations, local);
      break;

    case _ALGORITHM_LSU_MCS_:
      S = new BLS(verbosity, cardinality, num_conflicts, num_iterations, local);
      break;

    case _ALGORITHM_OLL_:
      if ((int)(cluster_algorithm) == 1) {
        S = new OLLIncCluster(verbosity, cardinality, ClusterAlg::_DIVISIVE_,
                       rounding_statistic, (int)(num_clusters));
      } else {
        S = new OLL(verbosity, cardinality);
      }
      break;

    case _ALGORITHM_BEST_:
      break;

    default:
      printf("c Error: Invalid MaxSAT algorithm.\n");
      printf("s UNKNOWN\n");
      exit(_ERROR_);
    }

    signal(SIGXCPU, SIGINT_exit);
    signal(SIGTERM, SIGINT_exit);

    if (argc == 1) {
      printf("c Error: no filename.\n");
      printf("s UNKNOWN\n");
      exit(_ERROR_);
    }

    gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
    if (in == NULL)
      printf("c ERROR! Could not open file: %s\n",
             argc == 1 ? "<stdin>" : argv[1]),
          printf("s UNKNOWN\n"), exit(_ERROR_);

    MaxSATFormula *maxsat_formula = new MaxSATFormula();

    if ((int)formula == _FORMAT_MAXSAT_) {
      parseMaxSATFormula(in, maxsat_formula);
      maxsat_formula->setFormat(_FORMAT_MAXSAT_);
    } else {
      ParserPB *parser_pb = new ParserPB();
      parser_pb->parsePBFormula(argv[1], maxsat_formula);
      maxsat_formula->setFormat(_FORMAT_PB_);
    }
    gzclose(in);

    printf("c |                                                                "
           "                                       |\n");
    printf("c ========================================[ Problem Statistics "
           "]===========================================\n");
    printf("c |                                                                "
           "                                       |\n");

    if (maxsat_formula->getFormat() == _FORMAT_MAXSAT_)
      printf(
          "c |  Problem Format:  %17s                                         "
          "                          |\n",
          "MaxSAT");
    else
      printf(
          "c |  Problem Format:  %17s                                         "
          "                          |\n",
          "PB");

    if (maxsat_formula->getProblemType() == _UNWEIGHTED_)
      printf("c |  Problem Type:  %19s                                         "
             "                          |\n",
             "Unweighted");
    else
      printf("c |  Problem Type:  %19s                                         "
             "                          |\n",
             "Weighted");

    printf("c |  Number of variables:  %12d                                    "
           "                               |\n",
           maxsat_formula->nVars());
    printf("c |  Number of hard clauses:    %7d                                "
           "                                   |\n",
           maxsat_formula->nHard());
    printf("c |  Number of soft clauses:    %7d                                "
           "                                   |\n",
           maxsat_formula->nSoft());
    printf("c |  Number of cardinality:     %7d                                "
           "                                   |\n",
           maxsat_formula->nCard());
    printf("c |  Number of PB :             %7d                                "
           "                                   |\n",
           maxsat_formula->nPB());
    double parsed_time = cpuTime();

    printf("c |  Parse time:           %12.2f s                                "
           "                                 |\n",
           parsed_time - initial_time);
    printf("c |                                                                "
           "                                       |\n");

    if (algorithm == _ALGORITHM_BEST_) {
      assert(S == NULL);

      if (maxsat_formula->getProblemType() == _UNWEIGHTED_) {
        // Unweighted
        S = new PartMSU3(_VERBOSITY_MINIMAL_, _PART_BINARY_, RES_GRAPH,
                         cardinality);
        S->loadFormula(maxsat_formula);

        if (((PartMSU3 *)S)->chooseAlgorithm() == _ALGORITHM_MSU3_) {
          // FIXME: possible memory leak
          S = new MSU3(_VERBOSITY_MINIMAL_);
        }

      } else {
        // Weighted
        S = new OLL(_VERBOSITY_MINIMAL_, cardinality);
      }
    }

    if (S->getMaxSATFormula() == NULL) {
      S->loadFormula(maxsat_formula);
      if ((int)(cluster_algorithm) == 1) {
        switch ((int)algorithm) {
        case _ALGORITHM_LINEAR_SU_:
          static_cast<LinearSUIncCluster *>(S)->initializeCluster();
          break;
        case _ALGORITHM_OLL_:
          static_cast<OLLIncCluster *>(S)->initializeCluster();
          break;
        case _ALGORITHM_LSU_INCBMO_:
          static_cast<LinearSUIncBMO *>(S)->initializeCluster();
          break;
        }
      }
    }
    S->setPrintModel(printmodel);
    S->setInitialTime(initial_time);
    S->compact_model = compact;
    mxsolver = S;
    mxsolver->search();

  } catch (OutOfMemoryException &) {
    sleep(1);
    printf("c Error: Out of memory.\n");
    printf("s UNKNOWN\n");
    exit(_ERROR_);
  }
}
