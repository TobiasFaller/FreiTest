#pragma once

#include <vector>

#include "SolverProxy/BaseLiteral.hpp"

namespace SolverProxy
{

enum class Value
{
	Positive,
	Negative,
	DontCare,
	Invalid
};

class ISolverProxy
{
public:
	ISolverProxy(void);
	virtual ~ISolverProxy(void);

	virtual void NewClause(void) = 0;
	virtual void CommitClause(void) = 0;
	virtual BaseLiteral NewLiteral(void) = 0;
	virtual void AddLiteral(const BaseLiteral lit) = 0;
	virtual Value GetLiteralValue(const BaseLiteral lit) const = 0;

	// Shorthand methods for convenience
	void CommitClause(BaseLiteral l1);
	void CommitClause(BaseLiteral l1, BaseLiteral l2);
	void CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3);
	void CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4);
	void CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5);
	void CommitClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, BaseLiteral l6);
	void CommitClause(const std::vector< BaseLiteral>& literals);

};

std::string to_string(const Value& value);
std::ostream& operator<<(std::ostream& stream, const Value& value);

};
