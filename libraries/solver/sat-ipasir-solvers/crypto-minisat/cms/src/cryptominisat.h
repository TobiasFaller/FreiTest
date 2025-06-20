/******************************************
Copyright (C) 2009-2020 Authors of CryptoMiniSat, see AUTHORS file

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***********************************************/

#ifndef __CRYPTOMINISAT5_H__
#define __CRYPTOMINISAT5_H__

#define CRYPTOMINISAT_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define CRYPTOMINISAT_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define CRYPTOMINISAT_VERSION_PATCH @PROJECT_VERSION_PATCH@

#include <atomic>
#include <vector>
#include <iostream>
#include <utility>
#include <string>
#include "solvertypesmini.h"

namespace CMSat {
    struct CMSatPrivateData;
    #ifdef _WIN32
    class __declspec(dllexport) SATSolver
    #else
    class SATSolver
    #endif
    {
    public:
        ////////////////////////////
        // You can pass in a variable that if set to TRUE, will abort the
        // solver as soon as possible. This bool can be set through a timer,
        // or through a thread, etc. This gives you the possiblity to abort
        // the solver any time you like, depending on some external factor
        // such as time, or your own code's inner workings.
        SATSolver(void* config = NULL
        , std::atomic<bool>* interrupt_asap = NULL
        );
        ~SATSolver();

        ////////////////////////////
        // Adding variables and clauses
        ////////////////////////////

        void new_var(); //add a new variable to the solver
        void new_vars(const size_t n); //and many new variables to the solver -- much faster
        unsigned nVars() const; //get number of variables inside the solver
        bool add_clause(const std::vector<Lit>& lits);
        bool add_xor_clause(const std::vector<unsigned>& vars, bool rhs);
        void set_var_weight(Lit lit, double weight);

        ////////////////////////////
        // Solving and simplifying
        // You can call solve() multiple times: incremental mode is supported!
        ////////////////////////////

        lbool solve(const std::vector<Lit>* assumptions = 0, bool only_indep_solution = false); //solve the problem, optionally with assumptions. If only_indep_solution is set, only the independent variables set with set_independent_vars() are returned in the solution
        lbool simplify(const std::vector<Lit>* assumptions = 0); //simplify the problem, optionally with assumptions
        const std::vector<lbool>& get_model() const; //get model that satisfies the problem. Only makes sense if previous solve()/simplify() call was l_True
        const std::vector<Lit>& get_conflict() const; //get conflict in terms of the assumptions given in case the previous call to solve() was l_False
        bool okay() const; //the problem is still solveable, i.e. the empty clause hasn't been derived
        const std::vector<Lit>& get_decisions_reaching_model() const; //get decisions that lead to model. may NOT work, in case the decisions needed were internal, extended variables. exit(-1)'s in case of such a case. you MUST check decisions_reaching_computed().

        ////////////////////////////
        // Debug all calls for later replay with --debuglit FILENAME
        ////////////////////////////
        void log_to_file(std::string filename);

        ////////////////////////////
        // SQLite for statistics gathering
        ////////////////////////////
        void set_sqlite(std::string filename);
        void add_sql_tag(const std::string& tagname, const std::string& tag);
        unsigned long get_sql_id() const;

        ////////////////////////////
        // Configuration
        // -- Note that nothing else can be changed, only these.
        // -- The main.cpp has access to the internal config, but it changes
        // -- all the time and hence exposing it to the outside world would
        // -- be very brittle.
        ////////////////////////////

        void set_num_threads(unsigned n); //Number of threads to use. Must be set before any vars/clauses are added
        void set_allow_otf_gauss(); //allow on-the-fly gaussian elimination
        /**
         * CPU time (in seconds) that can be consumed before the next call to solve() must return
         *
         * Because the elapsed CPU time depends on both the number of
         * threads, and the activity of these threads, the elapsed time
         * can wildly differ from wall clock time.
         *
         * \pre max_time >= 0
         */
        void set_max_time(double max_time);
        /**
         * Conflicts that can be consumed before the next call to solve() must return
         *
         * \pre max_confl >= 0
         */
        void set_max_confl(int64_t max_confl);
        void set_verbosity(unsigned verbosity = 0); //default is 0, silent
        void set_verbosity_detach_warning(bool verb); //default is 0, silent
        void set_default_polarity(bool polarity); //default polarity when branching for all vars
        void set_polarity_auto(); //set polarity to polarity caching
        void set_no_simplify(); //never simplify
        void set_no_simplify_at_startup(); //doesn't simplify at start, faster startup time
        void set_no_equivalent_lit_replacement(); //don't replace equivalent literals
        void set_no_bva(); //No bounded variable addition
        void set_no_bve(); //No bounded variable elimination
        void set_yes_comphandler(); //Allow component handler to work
        void set_greedy_undef(); //Try to set variables to l_Undef in solution
        void set_sampling_vars(std::vector<uint32_t>* sampl_vars);
        void set_timeout_all_calls(double secs); //max timeout on all subsequent solve() or simplify
        void set_up_for_scalmc(); //used to set the solver up for ScalMC configuration
        void set_single_run(); //we promise to call solve() EXACTLY once
        void set_intree_probe(int val);
        void set_sls(int val);
        void set_full_bve(int val);
        void set_full_bve_iter_ratio(double val);
        void set_scc(int val);
        void set_bva(int val);
        void set_distill(int val);
        void reset_vsids();
        void set_no_confl_needed(); //assumptions-based conflict will NOT be calculated for next solve run
        void set_xor_detach(bool val);


        ////////////////////////////
        // Get generic info
        ////////////////////////////
        static const char* get_version(); //get solver version in string format
        static const char* get_version_sha1(); //get SHA1 version string of the solver
        static const char* get_compilation_env(); //get compilation environment string
        std::string get_text_version_info();  //get printable version and copyright text


        ////////////////////////////
        // Get info about only the last solve() OR simplify() call
        // summed for all threads
        ////////////////////////////
        uint64_t get_last_conflicts(); //get total number of conflicts of last solve() or simplify() call of all threads
        uint64_t get_last_propagations();  //get total number of propagations of last solve() or simplify() call made by all threads
        uint64_t get_last_decisions(); //get total number of decisions of last solve() or simplify() call made by all threads


        ////////////////////////////
        //Get info about total sum of all time of all threads
        ////////////////////////////

        uint64_t get_sum_conflicts(); //get total number of conflicts of all time of all threads
        uint64_t get_sum_conflicts() const; //!< Return sum of all conflicts since construction across all the threads
        uint64_t get_sum_propagations();  //get total number of propagations of all time made by all threads
        uint64_t get_sum_propagations() const; //!< Returns sum of all propagations since construction across all the threads
        uint64_t get_sum_decisions(); //get total number of decisions of all time made by all threads
        uint64_t get_sum_decisions() const; //!< Returns sum of all decisions since construction across all the threads

        void print_stats() const; //print solving stats. Call after solve()/simplify()
        void set_drat(std::ostream* os, bool set_ID); //set drat to ostream, e.g. stdout or a file
        void add_empty_cl_to_drat(); // allows to treat SAT as UNSAT and perform learning
        void interrupt_asap(); //call this asynchronously, and the solver will try to cleanly abort asap
        void dump_irred_clauses(std::ostream *out) const; //dump irredundant clauses to this stream when solving finishes
        void dump_red_clauses(std::ostream *out) const; //dump redundant ("learnt") clauses to this stream when solving finishes
        void open_file_and_dump_irred_clauses(std::string fname) const; //dump irredundant clauses to this file when solving finishes
        void open_file_and_dump_red_clauses(std::string fname) const; //dump redundant ("learnt") clauses to this file when solving finishes
        void add_in_partial_solving_stats(); //used only by Ctrl+C handler. Ignore.

        ////////////////////////////
        // Extract useful information from the solver
        // This can be used in the theory solver

        ////////////////////////////
        std::vector<Lit> get_zero_assigned_lits() const; //get literals of fixed value
        std::vector<std::pair<Lit, Lit> > get_all_binary_xors() const; //get all binary XORs that are = 0

        //////////////////////
        // EXPERIMENTAL
        std::vector<std::pair<std::vector<uint32_t>, bool> > get_recovered_xors(bool xor_together_xors) const; //get XORs recovered. If "xor_together_xors" is TRUE, then xors that share a variable (and ONLY they share them) will be XORed together
        std::vector<uint32_t> get_var_incidence();
        std::vector<uint32_t> get_var_incidence_also_red();
        std::vector<double> get_vsids_scores();

        //Given a set of literals to enqueue, returns:
        // 1) Whether they imply SAT/UNSAT. If "true": SAT. If "false": UNSAT
        // 2) into "out_implied" the set of literals they imply, including the literals themselves
        // NOTES:
        // * You may get back some of the literals you gave
        // * Order is not guaranteed: literals you gave as input may end up at the end or may not end up at all
        // * It only returns literals that are newly implied. So you must call get_zero_assigned_lits() before to be sure you know what literals are implied at decision level 0

        bool implied_by(
            const std::vector<Lit>& lits, std::vector<Lit>& out_implied);

        //////////////////////
        //Below must be done in-order. Multi-threading not allowed.
        void start_getting_small_clauses(uint32_t max_len, uint32_t max_glue);
        bool get_next_small_clause(std::vector<Lit>& ret); //returns FALSE if no more
        void end_getting_small_clauses();

    private:

        ////////////////////////////
        // Do not bother with this, it's private
        ////////////////////////////

        CMSatPrivateData *data;
    };
}

#endif //__CRYPTOMINISAT5_H__
