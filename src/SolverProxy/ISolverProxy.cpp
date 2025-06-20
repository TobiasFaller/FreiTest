#include "SolverProxy/ISolverProxy.hpp"

namespace SolverProxy
{

ISolverProxy::ISolverProxy(void) = default;
ISolverProxy::~ISolverProxy(void) = default;

void ISolverProxy::CommitClause(BaseLiteral l1)
{
	NewClause();
	AddLiteral(l1);
	CommitClause();
}

void ISolverProxy::CommitClause(BaseLiteral l1, BaseLiteral l2)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	CommitClause();
}

void ISolverProxy::CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	CommitClause();
}

void ISolverProxy::CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	CommitClause();
}

void ISolverProxy::CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	AddLiteral(l5);
	CommitClause();
}

void ISolverProxy::CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, BaseLiteral l6)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	AddLiteral(l5);
	AddLiteral(l6);
	CommitClause();
}

void ISolverProxy::CommitClause(const std::vector< BaseLiteral>& literals)
{
	NewClause();
	for ( BaseLiteral lit : literals )
	{
		AddLiteral(lit);
	}
	CommitClause();
}

std::string to_string(const Value& value)
{
	switch (value)
	{
		case Value::Positive: return "1";
		case Value::Negative: return "0";
		case Value::DontCare: return "X";
		default: return "-";
	}
}

std::ostream& operator<<(std::ostream& stream, const Value& value)
{
	stream << to_string(value);
	return stream;
}

};
