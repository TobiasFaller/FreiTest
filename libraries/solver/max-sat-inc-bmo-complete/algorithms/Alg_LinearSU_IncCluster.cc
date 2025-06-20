/*!
 * \author Ruben Martins - ruben@sat.inesc-id.pt
 *
 * @section LICENSE
 *
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

#include "Alg_LinearSU_IncCluster.h"
#include "../MaxTypes.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#define MAX_CLAUSES 3000000

using namespace openwbo;

/*_________________________________________________________________________________________________
  |
  |  initializeCluster : [void] -> [void]
  |
  |  Description:
  |
  |    Initializes cluster according to the algorithm specified.
  |
  |  Pre-conditions:
  |    * cluster_algo contains the clustering method
  |
  |  Post-conditions:
  |    * cluster is initialized
  |
  |________________________________________________________________________________________________@*/
void LinearSUIncCluster::initializeCluster() {
  switch (cluster_algo) {
  case ClusterAlg::_DIVISIVE_:
    cluster = new Cluster_DivisiveMaxSeparate(maxsat_formula, cluster_stat);
    break;
  }
}

/*_________________________________________________________________________________________________
  |
  |  computeCostModel : (currentModel : vec<lbool>&) (weight : int) ->
  |                     [uint64_t]
  |
  |  Description:
  |
  |    Computes the cost of 'currentModel'. The cost of a model is the sum of
  |    the weights of the unsatisfied soft clauses.
  |    If a weight is specified, then it only considers the sum of the weights
  |    of the unsatisfied soft clauses with the specified weight.
  |
  |  Pre-conditions:
  |    * Assumes that 'currentModel' is not empty.
  |
  |________________________________________________________________________________________________@*/
uint64_t LinearSUIncCluster::computeCostModel(vec<lbool> &currentModel,
                                       uint64_t weight) {

  assert(currentModel.size() != 0);
  uint64_t currentCost = 0;

  for (int i = 0; i < maxsat_formula->nSoft(); i++) {
    bool unsatisfied = true;
    for (int j = 0; j < maxsat_formula->getSoftClause(i).clause.size(); j++) {

      if (weight != UINT64_MAX &&
          maxsat_formula->getSoftClause(i).weight != weight) {
        unsatisfied = false;
        continue;
      }

      assert(var(maxsat_formula->getSoftClause(i).clause[j]) <
             currentModel.size());
      if ((sign(maxsat_formula->getSoftClause(i).clause[j]) &&
           currentModel[var(maxsat_formula->getSoftClause(i).clause[j])] ==
               BMO_SOLVER_NSPACE::l_False) ||
          (!sign(maxsat_formula->getSoftClause(i).clause[j]) &&
           currentModel[var(maxsat_formula->getSoftClause(i).clause[j])] ==
               BMO_SOLVER_NSPACE::l_True)) {
        unsatisfied = false;
        break;
      }
    }

    if (unsatisfied) {
      currentCost += maxsat_formula->getSoftClause(i).weight;
    }
  }

  return currentCost;
}

/*_________________________________________________________________________________________________
  |
  |  computeOriginalCost : (currentModel : vec<lbool>&) (weight : uint64_t) ->
  |                     [uint64_t]
  |
  |  Description:
  |
  |    Computes the cost of 'currentModel' as per the original weights.
  |    The cost of a model is the sum of the weights of the unsatisfied soft clauses.
  |    If a weight is specified, then it only considers the sum of the weights
  |    of the unsatisfied soft clauses with the specified weight.
  |
  |  Pre-conditions:
  |    * Assumes that 'currentModel' is not empty.
  |
  |________________________________________________________________________________________________@*/
uint64_t LinearSUIncCluster::computeOriginalCost(vec<lbool> &currentModel,
                                          uint64_t weight) {

  assert(currentModel.size() != 0);
  uint64_t currentCost = 0;

  for (int i = 0; i < maxsat_formula->nSoft(); i++) {
    bool unsatisfied = true;
    for (int j = 0; j < maxsat_formula->getSoftClause(i).clause.size(); j++) {

      if (weight != UINT64_MAX &&
          maxsat_formula->getSoftClause(i).weight != weight) {
        unsatisfied = false;
        continue;
      }

      assert(var(maxsat_formula->getSoftClause(i).clause[j]) <
             currentModel.size());
      if ((sign(maxsat_formula->getSoftClause(i).clause[j]) &&
           currentModel[var(maxsat_formula->getSoftClause(i).clause[j])] ==
               BMO_SOLVER_NSPACE::l_False) ||
          (!sign(maxsat_formula->getSoftClause(i).clause[j]) &&
           currentModel[var(maxsat_formula->getSoftClause(i).clause[j])] ==
               BMO_SOLVER_NSPACE::l_True)) {
        unsatisfied = false;
        break;
      }
    }

    if (unsatisfied) {
      currentCost += cluster->getOriginalWeight(i);
    }
  }

  return currentCost;
}

/************************************************************************************************
 //
 // Linear Search Algorithm with Boolean Multilevel Optimization (BMO)
 //
 ************************************************************************************************/

/*_________________________________________________________________________________________________
  |
  |  bmoSearch : [void] ->  [void]
  |
  |  Description:
  |
  |    Linear search algorithm with lexicographical optimization.
  |
  |  For further details see:
  |    * Joao Marques-Silva, Josep Argelich, Ana Graça, Ines Lynce: Boolean
  |      lexicographic optimization: algorithms & applications. Ann. Math.
  |      Artif. Intell. 62(3-4): 317-343 (2011)
  |
  |  Post-conditions:
  |    * 'lbCost' is updated.
  |    * 'ubCost' is updated.
  |    * 'nbSatisfiable' is updated.
  |    * 'nbCores' is updated.
  |
  |________________________________________________________________________________________________@*/
void LinearSUIncCluster::bmoSearch() {
  assert(orderWeights.size() > 0);
  lbool res = BMO_SOLVER_NSPACE::l_True;

  initRelaxation();

  uint64_t currentWeight = orderWeights[0];
  uint64_t minWeight = orderWeights[orderWeights.size() - 1];
  int posWeight = 0;

  vec<vec<Lit>> functions;
  vec<int> weights;

  solver = rebuildBMO(functions, weights, currentWeight);

  uint64_t localCost = 0;
  ubCost = 0;

  for (;;) {

    vec<Lit> dummy;
    // Do not use preprocessing for linear search algorithm.
    // NOTE: When preprocessing is enabled the SAT solver simplifies the
    // relaxation variables which leads to incorrect results.
    res = searchSATSolver(solver, dummy);

    if (res == BMO_SOLVER_NSPACE::l_True) {
      nbSatisfiable++;

      uint64_t newCost = computeCostModel(solver->model, currentWeight);
      if (currentWeight == minWeight) {
        // If current weight is the same as the minimum weight, then we are in
        // the last lexicographical function.
        uint64_t originalCost = computeOriginalCost(solver->model);
        if (best_cost >= originalCost) {
          saveModel(solver->model);
          solver->model.copyTo(best_model);
          best_cost = originalCost;
          printf("o %" PRId64 "\n", originalCost);
        }
        ubCost = newCost + lbCost;
      } else {
        if (verbosity > 0)
          printf("c BMO-UB : %-12" PRIu64 "\t (Function %d/%d)\n", newCost,
                 posWeight + 1, (int)orderWeights.size());
      }

      if (newCost == 0 && currentWeight == minWeight) {
        // Optimum value has been found.
        printAnswer(_OPTIMUM_);
        exit(_OPTIMUM_);
      } else {

        if (newCost == 0) {

          functions.push();
          new (&functions[functions.size() - 1]) vec<Lit>();
          objFunction.copyTo(functions[functions.size() - 1]);

          localCost = newCost;
          weights.push(localCost / currentWeight);

          posWeight++;
          currentWeight = orderWeights[posWeight];
          localCost = 0;

          delete solver;
          solver = rebuildBMO(functions, weights, currentWeight);

          if (verbosity > 0)
            printf("c LB : %-12" PRIu64 "\n", lbCost);
        } else {

          // Optimization of the current lexicographical function.
          if (localCost == 0)
            encoder->encodeCardinality(solver, objFunction,
                                      newCost / currentWeight - 1);
          else
            encoder->updateCardinality(solver, newCost / currentWeight - 1);

          localCost = newCost;
        }
      }
    } else {
      nbCores++;

      if (currentWeight == minWeight) {
        // There are no more functions to be optimized.

        if (model.size() == 0) {
          assert(nbSatisfiable == 0);
          // If no model was found then the MaxSAT formula is unsatisfiable
          printAnswer(_UNSATISFIABLE_);
          exit(_UNSATISFIABLE_);
        } else {
          printAnswer(_OPTIMUM_);
          exit(_OPTIMUM_);
        }
      } else {

        // The current lexicographical function has been optimize. Go to the
        // next lexicographical function.
        functions.push();
        new (&functions[functions.size() - 1]) vec<Lit>();
        objFunction.copyTo(functions[functions.size() - 1]);

        weights.push(localCost / currentWeight);
        lbCost += localCost;

        posWeight++;
        currentWeight = orderWeights[posWeight];
        localCost = 0;

        delete solver;
        solver = rebuildBMO(functions, weights, currentWeight);

        if (verbosity > 0)
          printf("c LB : %-12" PRIu64 "\n", lbCost);
      }
    }
  }
}

/*_________________________________________________________________________________________________
  |
  |  normalSearch : [void] ->  [void]
  |
  |  Description:
  |
  |    Linear search algorithm.
  |
  |  For further details see:
  |    *  Daniel Le Berre, Anne Parrain: The Sat4j library, release 2.2. JSAT
  |       7(2-3): 59-6 (2010)
  |    *  Miyuki Koshimura, Tong Zhang, Hiroshi Fujita, Ryuzo Hasegawa: QMaxSAT:
  |       A Partial Max-SAT Solver. JSAT 8(1/2): 95-100 (2012)
  |
  |  Post-conditions:
  |    * 'ubCost' is updated.
  |    * 'nbSatisfiable' is updated.
  |    * 'nbCores' is updated.
  |
  |________________________________________________________________________________________________@*/
void LinearSUIncCluster::normalSearch() {

  lbool res = BMO_SOLVER_NSPACE::l_True;

  initRelaxation();
  solver = rebuildSolver();
  while (res == BMO_SOLVER_NSPACE::l_True) {

    vec<Lit> dummy;
    // Do not use preprocessing for linear search algorithm.
    // NOTE: When preprocessing is enabled the SAT solver simplifies the
    // relaxation variables which leads to incorrect results.
    res = searchSATSolver(solver, dummy);

    if (res == BMO_SOLVER_NSPACE::l_True) {
      nbSatisfiable++;
      uint64_t newCost = computeCostModel(solver->model);
      uint64_t originalCost = computeOriginalCost(solver->model);
      if (complete) newCost = originalCost;
      if (best_cost >= originalCost) {
        saveModel(solver->model);
        solver->model.copyTo(best_model);
        best_cost = originalCost;

        if (maxsat_formula->getFormat() == _FORMAT_PB_) {
          // optimization problem
          if (maxsat_formula->getObjFunction() != NULL) {
            printf("o %" PRId64 "\n", originalCost);
          }
        } else {
          printf("o %" PRId64 "\n", originalCost);
        }
      }

      if (newCost == 0) {
        // If there is a model with value 0 then it is an optimal model
        ubCost = newCost;

        if (maxsat_formula->getFormat() == _FORMAT_PB_ &&
            maxsat_formula->getObjFunction() == NULL) {
          printAnswer(_SATISFIABLE_);
          exit(_SATISFIABLE_);
        } else {
          printAnswer(_OPTIMUM_);
          exit(_OPTIMUM_);
        }

      } else {
        if (maxsat_formula->getProblemType() == _WEIGHTED_) {
          if (!encoder->hasPBEncoding())
            encoder->encodePB(solver, objFunction, coeffs, newCost - 1);
          else{
            encoder->updatePB(solver, newCost - 1);
          }
        } else {
          // Unweighted.
          if (!encoder->hasCardEncoding())
            encoder->encodeCardinality(solver, objFunction, newCost - 1);
          else
            encoder->updateCardinality(solver, newCost - 1);
        }

        ubCost = newCost;
      }
    } else {
      nbCores++;
      if (model.size() == 0) {
        assert(nbSatisfiable == 0);
        // If no model was found then the MaxSAT formula is unsatisfiable
        printAnswer(_UNSATISFIABLE_);
        exit(_UNSATISFIABLE_);
      } else {
        if (!complete){
          delete solver; // possible memory leak?
          solver = rebuildSolver();
          complete = true;
          coeffs.clear();
          for (int i = 0; i < maxsat_formula->nSoft(); i++){
            coeffs.push(cluster->getOriginalWeight(i));
          }
          printf("c Warn: changing to LSU algorithm.\n");
          delete encoder;
          encoder = new Encoder(_INCREMENTAL_NONE_, _CARD_MTOTALIZER_,_AMO_LADDER_, _PB_GTE_);
          int expected_clauses = encoder->predictPB(solver, objFunction, coeffs, best_cost-1);
          printf("c GTE auxiliary #clauses = %d\n",expected_clauses);
          if (expected_clauses >= MAX_CLAUSES) {
            printf("c Warn: changing to Adder encoding.\n");
            encoder->setPBEncoding(_PB_ADDER_);
          }
          if (!encoder->hasPBEncoding())
            encoder->encodePB(solver, objFunction, coeffs, best_cost - 1);
          else
            encoder->updatePB(solver, ubCost - 1);
          res = BMO_SOLVER_NSPACE::l_True;
        } else {
          printAnswer(_OPTIMUM_);
          exit(_OPTIMUM_);
        }
      }
    }
  }
}

// Public search method
void LinearSUIncCluster::search() {

  cluster->clusterWeights(maxsat_formula, num_clusters);

  if (maxsat_formula->getProblemType() == _WEIGHTED_)
    is_bmo = isBMO();

  printConfiguration(is_bmo, maxsat_formula->getProblemType());

  // if (maxsat_formula->getProblemType() == _WEIGHTED_) {
  //   if (bmoMode && is_bmo)
  //     bmoSearch();
  //   else
  //     normalSearch();
  // } else
  //   normalSearch();
  normalSearch();
}

/************************************************************************************************
 //
 // Rebuild MaxSAT solver
 //
 ************************************************************************************************/

/*_________________________________________________________________________________________________
  |
  |  rebuildSolver : (minWeight : int)  ->  [Solver *]
  |
  |  Description:
  |
  |    Rebuilds a SAT solver with the current MaxSAT formula.
  |    If a weight is specified, then it only considers soft clauses with weight
  |    smaller than the specified weight.
  |    NOTE: a weight is specified in the 'bmo' approach.
  |
  |________________________________________________________________________________________________@*/
Solver *LinearSUIncCluster::rebuildSolver(uint64_t min_weight) {

  vec<bool> seen;
  seen.growTo(maxsat_formula->nVars(), false);

  Solver *S = newSATSolver();

  for (int i = 0; i < maxsat_formula->nVars(); i++)
    newSATVariable(S);

  for (int i = 0; i < maxsat_formula->nHard(); i++)
    S->addClause(maxsat_formula->getHardClause(i).clause);

  for (int i = 0; i < maxsat_formula->nPB(); i++) {
    Encoder *enc = new Encoder(_INCREMENTAL_NONE_, _CARD_MTOTALIZER_,
                               _AMO_LADDER_, _PB_GTE_);

    // Make sure the PB is on the form <=
    // if (maxsat_formula->getPBConstraint(i)->_sign)
    //  maxsat_formula->getPBConstraint(i)->changeSign();
    assert(maxsat_formula->getPBConstraint(i)->_sign);

    enc->encodePB(S, maxsat_formula->getPBConstraint(i)->_lits,
                  maxsat_formula->getPBConstraint(i)->_coeffs,
                  maxsat_formula->getPBConstraint(i)->_rhs);

    delete enc;
  }

  for (int i = 0; i < maxsat_formula->nCard(); i++) {
    Encoder *enc = new Encoder(_INCREMENTAL_NONE_, _CARD_MTOTALIZER_,
                               _AMO_LADDER_, _PB_GTE_);

    if (maxsat_formula->getCardinalityConstraint(i)->_rhs == 1) {
      enc->encodeAMO(S, maxsat_formula->getCardinalityConstraint(i)->_lits);
    } else {

      enc->encodeCardinality(S,
                             maxsat_formula->getCardinalityConstraint(i)->_lits,
                             maxsat_formula->getCardinalityConstraint(i)->_rhs);
    }

    delete enc;
  }

  vec<Lit> clause;
  for (int i = 0; i < maxsat_formula->nSoft(); i++) {
    if (maxsat_formula->getSoftClause(i).weight < min_weight)
      continue;

    clause.clear();
    maxsat_formula->getSoftClause(i).clause.copyTo(clause);

    for (int j = 0; j < maxsat_formula->getSoftClause(i).relaxation_vars.size();
         j++) {
      clause.push(maxsat_formula->getSoftClause(i).relaxation_vars[j]);
    }

    S->addClause(clause);
  }

  return S;
}

/*_________________________________________________________________________________________________
  |
  |  rebuildBMO : (functions : int)  ->  [Solver *]
  |
  |  Description:
  |
  |    Rebuilds a SAT solver with the current MaxSAT formula.
  |    Only considers soft clauses with the weight of the current
  |    lexicographical optimization weight ('currentWeight')
  |    For each function already computed in the BMO algorithm it encodes the
  |    respective cardinality constraint.
  |
  |________________________________________________________________________________________________@*/
Solver *LinearSUIncCluster::rebuildBMO(vec<vec<Lit>> &functions, vec<int> &rhs,
                                uint64_t currentWeight) {

  assert(functions.size() == rhs.size());

  Solver *S = rebuildSolver(currentWeight);

  objFunction.clear();
  coeffs.clear();
  for (int i = 0; i < maxsat_formula->nSoft(); i++) {
    if (maxsat_formula->getSoftClause(i).weight == currentWeight) {
      objFunction.push(maxsat_formula->getSoftClause(i).relaxation_vars[0]);
      coeffs.push(maxsat_formula->getSoftClause(i).weight);
    }
  }

  for (int i = 0; i < functions.size(); i++)
    encoder->encodeCardinality(S, functions[i], rhs[i]);

  return S;
}

/************************************************************************************************
 //
 // Other protected methods
 //
 ************************************************************************************************/

/*_________________________________________________________________________________________________
  |
  |  initRelaxation : (objective : vec<Lit>&) (weights : vec<int>&)  ->  [void]
  |
  |  Description:
  |
  |    Initializes the relaxation variables by adding a fresh variable to the
  |    'relaxationVars' of each soft clause.
  |
  |  Post-conditions:
  |    * 'objFunction' contains all relaxation variables that were added to soft
  |       clauses.
  |    * 'coeffs' contains the weights of all soft clauses.
  |
  |________________________________________________________________________________________________@*/
void LinearSUIncCluster::initRelaxation() {
  for (int i = 0; i < maxsat_formula->nSoft(); i++) {
    Lit l = maxsat_formula->newLiteral();
    maxsat_formula->getSoftClause(i).relaxation_vars.push(l);
    objFunction.push(l);
    coeffs.push(maxsat_formula->getSoftClause(i).weight);
  }
}

// Print LinearSU configuration.
void LinearSUIncCluster::print_LinearSU_configuration() {
  printf("c |  Algorithm: %23s                                             "
         "                      |\n",
         "Cluster");

  printf("c |  Number of clusters:   %12" PRId64 "                                 "
           "                                  |\n", num_clusters);

  // if (maxsat_formula->getProblemType() == _WEIGHTED_) {
  //   if (bmoMode)
  //     printf("c |  BMO strategy: %20s                      "
  //            "                                             |\n",
  //            "On");
  //   else
  //     printf("c |  BMO strategy: %20s                      "
  //            "                                             |\n",
  //            "Off");

  //   if (bmoMode) {
  //     if (is_bmo)
  //       printf("c |  BMO search: %22s                      "
  //              "                                             |\n",
  //              "Yes");
  //     else
  //       printf("c |  BMO search: %22s                      "
  //              "                                             |\n",
  //              "No");
  //   }
  // }
}
