[Index](../../../README.md) - [Next Topic](2_LogicContainer.md)

# SAT-Solver Proxy

The SAT solver proxy provides methods to create literals which then can be used to form clauses passed to the solver.
The three essential methods `NewClause`, `AddLiteral` and `CommitClause` are used to create these clauses.
The `CommitClause` overloads allow for directly creating a clause from its parameters.

By calling the method `Solve` on the solver proxy the problem is passed to the solver and solved.
The method returns the status of the solve attempt, indicating if it found a solution for the problem.
If a solution was found, the assigned value for each literal can be retrieved by calling the method `GetLiteralValue`.
The assigned value is either `Value::Positive`, `Value::Negative` or `Value::DontCare` for each literal.

```cpp
#include "Basic/Logging.h"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy;
using namespace SolverProxy::SAT;

auto proxy { SatSolverProxy::CreateSatSolver(SatSolver::PROD_CADICAL) };
proxy->SetSolverTimeout(5.0);

auto literal1 { proxy->NewLiteral() };
auto literal2 { proxy->NewLiteral() };
proxy->CommitClause(literal1, -literal2);
proxy->CommitClause(-literal1, literal2);

ASSERT(proxy->Solve() == SatResult::SAT);
auto assignment1 { proxy->GetLiteralValue(literal1) };
auto assignment2 { proxy->GetLiteralValue(literal2) };
LOG(INFO) << "Result: value1=" << assignment1 << ", value2=" << assignment2;
```

## Max-SAT Solver Proxy

The Max-SAT solver proxy provides an interface for solving weighted Max-SAT problems.
The `CommitSoftClause` method accepts the weight of the clause as parameter.

```cpp
#include "Basic/Logging.h"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"

using namespace SolverProxy;
using namespace SolverProxy::SAT;

auto proxy { MaxSatSolverProxy::CreateMaxSatSolver(SatSolver::PROD_PACOSE) };
proxy->SetSolverTimeout(5.0);

auto literal1 { proxy->NewLiteral() };
auto literal2 { proxy->NewLiteral() };
proxy->CommitClause(literal1, -literal2);
proxy->CommitClause(-literal1, literal2);
proxy->CommitSoftClause(literal1, 5u);
proxy->CommitSoftClause(-literal1, -literal2, 2u);

ASSERT(proxy->MaxSolve() == SatResult::SAT);
auto assignment1 { proxy->GetLiteralValue(literal1) };
auto assignment2 { proxy->GetLiteralValue(literal2) };
LOG(INFO) << "Result: value1=" << assignment1 << ", value2=" << assignment2;
```

## Count-SAT Solver Proxy

The Count-SAT (#SAT) solver proxy provides an interface to counting the number of possible solutions and is mostly equivalent to the SAT interface.

```cpp
#include "Basic/Logging.h"
#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"

using namespace SolverProxy;
using namespace SolverProxy::SAT;

auto proxy { CountSatSolverProxy::CreateCountSatSolver(...) };
proxy->SetSolverTimeout(5.0);

...

ASSERT(proxy->CountSolve() == SatResult::SAT);
LOG(INFO) << "Result: solutions=" << proxy->GetLastModelCount();
```

# BMC-Solver Proxy

The BMC solver proxy shares the interface with the SAT solver proxy.
However, clauses are encoded into the three clause groups (Init, Trans, Target) that form the BMC problem.
The target clause group can be selected via the `SetTargetClauseType` method.
For variables the `SetTargetVariableType` is setting the assigned variable type (Input, Output, Latch, Auxiliary) for subsequent `NewLiteral` calls.
The `SetTargetTimeframe` method set the assigned timeframe for literals, which is required for extracting timeframes beyond the first one.

```cpp
#include "Basic/Logging.h"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace SolverProxy;
using namespace SolverProxy::BMC;

auto proxy { BmcSolverProxy::CreateBmcSolver(BmcSolver::PROD_NCIP) };
proxy->SetSolverTimeout(5.0);

proxy->SetTargetVariableType(VariableType::Latch);
auto literal1 { proxy->NewLiteral() };
proxy->SetTargetClauseType(ClauseType::Initial);
proxy->SetTargetTimeframe(0u);
proxy->CommitClause(-literal1);

proxy->SetTargetClauseType(ClauseType::Transition);
proxy->CommitTimeframeClause(literal1, 0u, literal1, 1u);
proxy->CommitTimeframeClause(-literal1, 0u, -literal1, 1u);

proxy->SetTargetClauseType(ClauseType::Target);
proxy->SetTargetTimeframe(0u);
proxy->CommitClause(literal1);

ASSERT(proxy->Solve() == BmcResult::Reachable);
proxy->SetTargetTimeframe(0u);
auto assignment1 { proxy->GetLiteralValue(literal1) };
LOG(INFO) << "Result: value1=" << assignment1 << ", value2=" << assignment2;
```

# BaseLiteral

The `BaseLiteral` class represents one literal of the problem.
The `-` operator can be used to negate the literal.
A new literal can be created by calling the `NewLiteral` method of the solver proxy.
The following three predefined literals exist:

- `UNDEFINED_LIT`:
  A literal which has a variable index of 0 which **is considered invalid**.
  This constant can be used to represent unused literals.
  When this literal is negated it will throw an exception.
- `NEG_LIT`: A literal which has a variable index of 1 and is inverted.
  Represents a **constant value of false** which can be used to model constant values.
- `POS_LIT`: A literal which has a variable index of 1 and is not inverted.
  Represents a **constant value of true** which can be used to model constant values.

The constants `NEG_LIT` and `POS_LIT` are modeled by adding a unit-clause with only one literal (`POS_LIT`, having variable index 1).
Therefore, the solver has to assign **true** to this literal.
The constant `NEG_LIT` is the negation of the `POS_LIT` constant.

```cpp
auto literalA { proxy->NewLiteral() };
auto literalB { proxy->NewLiteral() };

// Inverting a literal
proxy->CommitClause(-literalA);

// Constant literal in clause (clause is always satisfied)
proxy->CommitClause(POS_LIT, -literalB, literalA);
```

[Index](../../../README.md) - [Next Topic](2_LogicContainer.md)
