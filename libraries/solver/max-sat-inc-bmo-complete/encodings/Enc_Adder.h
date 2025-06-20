#ifndef Enc_Adder_h
#define Enc_Adder_h

#include "../IncludeSolver.h"
#include "Encodings.h"
#include <map>
#include <utility>
#include <vector>
#include <queue>

namespace openwbo {
class Adder : public Encodings {

public:
  Adder() {
    hasEncoding = false;
  }
  ~Adder() {}

  // Encode constraint.
  void encode(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs, uint64_t rhs);

  // Update constraint.
  void update(Solver *S, uint64_t rhs);

  // Returns true if the encoding was built, otherwise returns false;
  bool hasCreatedEncoding() { return hasEncoding; }

  void encodeInc(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs, uint64_t rhs, vec<Lit> &assumptions);
  void updateInc(Solver *S, uint64_t rhs, vec<Lit>& assumptions);


protected:

  vec<Lit> _output;
  vec<Lit> clause;
  std::vector<std::queue<Lit> > _buckets;

  void FA_extra ( Solver *S, Lit xc, Lit xs, Lit a, Lit b, Lit c );
  Lit FA_carry ( Solver *S, Lit a, Lit b, Lit c );
  Lit FA_sum ( Solver *S, Lit a, Lit b, Lit c );
  Lit HA_carry ( Solver *S, Lit a, Lit b);
  Lit HA_sum ( Solver *S, Lit a, Lit b );
  void adderTree (Solver *S, std::vector< std::queue< Lit > > & buckets, vec< Lit >& result );
  void lessThanOrEqual (Solver *S, vec< Lit > & xs, std::vector< uint64_t > & ys);
  void numToBits ( std::vector<uint64_t> & bits, uint64_t n, uint64_t number );
  uint64_t ld64(const uint64_t x);

  void lessThanOrEqualInc (Solver *S, vec< Lit > & xs, std::vector< uint64_t > & ys, vec<Lit>& assumptions);


	#define wbsplit(half,wL,wR, ws,bs, wsL,bsL, wsR,bsR) \
  wsL.clear(); bsL.clear(); wsR.clear(); bsR.clear(); \
  int ii = 0; \
  int wsSizeHalf = ws.size()/2; \
  for(; ii < wsSizeHalf; ii++) { \
    wsL.push(ws[ii]); \
    bsL.push(bs[ii]); \
    wL += ws[ii]; \
  } \
  for(; ii < ws.size(); ii++) { \
    wsR.push(ws[ii]); \
    bsR.push(bs[ii]); \
    wR += ws[ii]; \
  }

	void genWarnersFull(Lit& a, Lit& b, Lit& c, Lit& carry, Lit& sum, int comp,
		       Solver* S, vec<Lit>& lits);

	void genWarnersHalf(Lit& a, Lit& b, Lit& carry, Lit& sum, int comp,
		       Solver* S, vec<Lit>& lits);

	void genWarners(vec<uint64_t>& weights, vec<Lit>& blockings,
		uint64_t max, int k,
		int comp, Solver* S, const Lit zero,
		vec<Lit>& lits, vec<Lit>& linkingVar);

	void genWarners0(vec<uint64_t>& weights, vec<Lit>& blockings,
		 uint64_t max,uint64_t k, int comp, Solver *S,
		  vec<Lit>& lits, vec<Lit>& linkingVar);

	void lessthan(vec<Lit>& linking, uint64_t k, vec<uint64_t>& cc, Solver* S, vec<Lit>& lits);

	vec<uint64_t> cc;
	vec<Lit> linkingVar;

	void wbSort(vec<uint64_t>& weights, vec<Lit>& blockings,
	    vec<uint64_t>& sweights, vec<Lit>& sblockings) {
  sweights.clear(); sblockings.clear();
  for(int i = 0; i < weights.size(); i++) {
    sweights.push(weights[i]);
    sblockings.push(blockings[i]);
  }
}

// koshi 20140121
void wbFilter(uint64_t UB, Solver* S,vec<Lit>& lits,
	      vec<uint64_t>& weights, vec<Lit>& blockings,
	      vec<uint64_t>& sweights, vec<Lit>& sblockings) {
  sweights.clear(); sblockings.clear();

  for(int i = 0; i < weights.size(); i++) {
    if (weights[i] < UB) {
      sweights.push(weights[i]);
      sblockings.push(blockings[i]);
    } else {
      lits.clear();
      lits.push(~blockings[i]);
      S->addClause(lits);
    }
  }
}


};
}

#endif
