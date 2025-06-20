#pragma once

#include <cstdint>
#include <type_traits>
#include <memory>
#include <vector>

#include "Basic/Logic.hpp"
#include "SolverProxy/ISolverProxy.hpp"
#include "Tpg/Container/LogicContainer.hpp"

namespace FreiTest
{
namespace Tpg
{

/*!
 * \brief Provides an multi-encoding class for logic generation.
 *
 * The methods NewLogicContainer can be used to generate a new logic container that
 * holds the raw literals used by the solver.
 * In most cases a direct access / modification of the raw literals should not be required.
 *
 * The method GetLogicValue can be used to retrieve the resulting logic values after
 * the CNF has been solved by the solver.
 *
 * Requirements and value detectors can be encoded via the Encode...Requirement
 * and Encode...Detector methods and enable to constrain or observe the logic
 * values in the targetet logic containers.
 * A requirement directly constrains the passed logic container.
 * A detector returns a new logic container whose logic value indicates if the
 * desired value / constraint has been met by the target logic container.
 *
 * All other methods are used to encode basic logic gates like buffer, inverter,
 * and, nand, or, nor, xor, xnor and multiplexer gates.
 * Methods accepting a vector of logic containers either encode multiple gates at once
 * (like SIMD processing) or encode trees of the basic logic gates that result in
 * a single output value (used for multi-input gates).
 * See the method description for more detailled information.
 */
class LogicEncoder {
public:
	/**
	 * @brief Construct a new logic encoder which uses the passed solver
	 * to create new variables and to store the generated clauses into.
	 *
	 * @param solver The solver that is used by this logic encoder.
	 */
	LogicEncoder(std::shared_ptr<SolverProxy::ISolverProxy> solver);
	virtual ~LogicEncoder(void);

	/**
	 * @brief Creates a new logic container by using unassigned variables from the solver.
	 *
	 * The logic container has by default no constraints applied to it.
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @return The new logic container with newly created variables.
	 */
	template<typename LogicContainer> LogicContainer NewLogicContainer(void);

	/**
	 * @brief Creates a new logic container with a constant logic value.
	 *
	 * The constant value container is created by using the special literals POS_LIT and NEG_LIT
	 * and does not require method calls to the underlying solver.
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @return The new logic container with a constant value.
	 */
	template<typename LogicContainer> LogicContainer NewLogicContainer(Basic::Logic value) const;

	/**
	 * @brief Creates a new logic container with the raw literals passed as arguments.
	 *
	 * This is a special order-safe method to instantiate a logic container from known literals.
	 * The Literals... parameter is used to encode the order of the parameters into the method call.
	 * Therefore, the internal parameter order of the constructor of the logic container does not
	 * need to be known.
	 *
	 * Usage: (lit, lit1 and lit0 are already created literals)
	 *
	 *     NewLogicContainer<LogicContainer01, Literal0>(lit);
	 *     NewLogicContainer<LogicContainer01X, Literal1, Literal0>(lit1, lit0);
	 *     NewLogicContainer<LogicContainerU01X, Literal1, Literal0>(lit1, lit0);
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @tparam Literals The order of the specified literals.
	 * @param literals The literals to store in the logic container.
	 * @return The new logic container with the specified literals.
	 */
	template<typename LogicContainer, typename... Literals> LogicContainer NewLogicContainer(typename Literals::Type... literals) const;

	/**
	 * @brief Provides an up-conversion for logic containers.
	 *
	 * The literals of the original logic container are reused if possible.
	 *
	 * Usage:
	 *
	 *     LogicContainer01 cont1 = ...;
	 *     LogicContainerU01X cont2 = ConvertLogicContainer<LogicContainerU01X>(cont1);
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @tparam SourceLogicContainer The original logic container type (can be inferred).
	 * @param value The source logic container to up-convert.
	 * @return LogicContainer The new logic container with the same value as the source logic container.
	 */
	template<typename LogicContainer, typename SourceLogicContainer> LogicContainer ConvertLogicContainer(const SourceLogicContainer& value);

	/**
	 * @brief Retrieves the logic value of the literals in the logic container as assigned by the solver.
	 *
	 * Note that this method will panic if the solver has not solved the CNF yet and the logic assignment
	 * of the container is unknown.
	 *
	 * In case of a constant logic container this method will always succeed but the GetConstantLogicValue
	 * should be preferred as it returns LOGIC_INVALID if the logic container does not hold a constant value.
	 * The IsConstantLogicValue method can be used to check beforehand if the logic container holds a constant value.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The logic container to retrieve the logic value from.
	 * @return Logic The constant logic value of the constant logic container.
	 */
	template<typename LogicContainer> Basic::Logic GetSolvedLogicValue(const LogicContainer& target) const;

	/**
	 * @brief Checks if the logic container holds a constant value.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The logic container to examine.
	 * @return true If the logic containers logic value is constant.
	 * @return false If the logic container contains non-constant literals.
	 */
	template<typename LogicContainer> bool IsConstantLogicValue(const LogicContainer& target) const;

	/**
	 * @brief Retrieves the constant logic value from the logic container.
	 *
	 * This method returns LOGIC_INVALID if the logic container does not hold a constant value.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The logic container to retrieve the constant value from.
	 * @return Logic The constant logic value of the container or LOGIC_INVALID if the container is not constant.
	 */
	template<typename LogicContainer> Basic::Logic GetConstantLogicValue(const LogicContainer& target) const;

	/**
	 * @brief Imposes a logic value constraint on the passed logic container.
	 *
	 * This forces the logic container to be assigned the logic value of this constraint during solving.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The logic container to constrain.
	 * @param value The logic value to constrain the container to.
	 */
	template<typename LogicContainer> void EncodeLogicValue(const LogicContainer& target, Basic::Logic value);

	/**
	 * @brief Imposes a logic value constraint on the passed logic container.
	 *
	 * This forces the logic container to be assigned a logic value that is valid
	 * according to this constraint during solving.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The logic container to constrain.
	 * @param constraint The logic constraint to apply to the container.
	 */
	template<typename LogicContainer> void EncodeLogicValueConstraint(const LogicContainer& target, Basic::LogicConstraint constraint);

	/**
	 * @brief Encodes a constraint that forces both logic containers to be equal.
	 *
	 * This constraint is enforced by encoding additional clauses and is only required if both
	 * logic containers have already been created and used in other parts of the code.
	 * If not it might be simpler to reuse a logic container directly
	 * if you want to have the same logic value twice.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container that is forced to be equal to the second logic container.
	 * @param in2 The second logic container that is forced to be equal to the first logic container.
	 */
	template<typename LogicContainer> void EncodeLogicEquivalenceConstraint(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Encodes a constraint that forces both logic containers to show a 01-difference.
	 *
	 * This results in that both logic containers can not be both LOGIC_ZERO or both LOGIC_ONE at the same time.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container that is forced to be 01-unequal to the second logic container.
	 * @param in2 The second logic container that is forced to be 01-unequal to the first logic container.
	 */
	template<typename LogicContainer> void EncodeLogic01DifferenceConstraint(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic container has the
	 * desired logic value, or LOGIC_ZERO if not.
	 *
	 * This method is meant to encode additional logic based on a logic container during solving.
	 * If you meant to retrieve a logic value from a logic container after solving then have a look at the
	 * GetSolvedLogicValue method.
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainer01 cont1IsUnknown = EncodeLogicValueDetector(cont1, Logic::LOGIC_UNKNOWN);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The target logic container to observe the logic value from.
	 * @param value The logic value to detect in the logic container.
	 * @return LogicContainer01 The new logic container that detects the logic value.
	 */
	template<typename LogicContainer> Tpg::LogicContainer01 EncodeLogicValueDetector(const LogicContainer& target, Basic::Logic value);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic container fulfills
	 * the passed constraint, or LOGIC_ZERO if not.
	 *
	 * This method is meant to encode additional logic based on a logic container during solving.
	 * If you meant to retrieve a logic value from a logic container after solving then have a look at the
	 * GetSolvedLogicValue method.
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainer01 cont1IsNotUnknown = EncodeLogicValueConstraintDetector(cont1, LogicConstraint::NOT_LOGIC_UNKNOWN);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The target logic container to observe the logic value from.
	 * @param constraint The logic constraint to detect in the logic container.
	 * @return LogicContainer01 The new logic container that detects the logic constraint.
	 */
	template<typename LogicContainer> Tpg::LogicContainer01 EncodeLogicValueConstraintDetector(const LogicContainer& target, Basic::LogicConstraint constraint);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic containers
	 * are exactly equal, or LOGIC_ZERO if they differ.
	 *
	 * This detector does return LOGIC_ONE if the logic values are exactly the same.
	 *
	 * Logic table:
	 *
	 *     container 1 | container 2 | result
	 *     ------------+-------------+-------
	 *     UNKNOWN     | UNKNOWN     | ONE
	 *     ZERO        | ZERO        | ONE
	 *     ONE         | ONE         | ONE
	 *     DONT_CARE   | DONT_CARE   | ONE
	 *     *           | *           | ZERO
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainerU01X cont2 = ...;
	 *     LogicContainer01 contAreEqual = EncodeLogicEquivalenceDetector(cont1, cont2);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container to compare the value from.
	 * @param in2 The second logic container to compare the value from.
	 * @return LogicContainer01 The new logic container that detects the logic equivalence.
	 */
	template<typename LogicContainer> Tpg::LogicContainer01 EncodeLogicEquivalenceDetector(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic containers
	 * are 0/1-equal, or LOGIC_ZERO if they differ.
	 *
	 * This detector does return LOGIC_ONE if the logic values are either
	 * LOGIC_ZERO or LOGIC_ONE and are not equal.
	 *
	 * Logic table:
	 *
	 *     container 1 | container 2 | result
	 *     ------------+-------------+-------
	 *     ZERO        | ONE         | ONE
	 *     ONE         | ZERO        | ONE
	 *     *           | *           | ZERO
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainerU01X cont2 = ...;
	 *     LogicContainer01 contAre01Equal = EncodeLogic01DifferenceDetector(cont1, cont2);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container to compare the value from.
	 * @param in2 The second logic container to compare the value from.
	 * @return LogicContainer01 The new logic container that detects the logic 0/1-equivalence.
	 */
	template<typename LogicContainer> Tpg::LogicContainer01 EncodeLogic01DifferenceDetector(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic container has the
	 * desired logic value, or LOGIC_ZERO if not.
	 *
	 * This method is meant to encode additional logic based on a logic container during solving.
	 * If you meant to retrieve a logic value from a logic container after solving then have a look at the
	 * GetSolvedLogicValue method.
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainer01 cont1IsUnknown = NewLogicContainer<LogicContainer01>();
	 *     EncodeLogicValueDetector(cont1, Logic::LOGIC_UNKNOWN, cont1IsUnknown);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The target logic container to observe the logic value from.
	 * @param value The logic value to detect in the logic container.
	 * @param out The logic container that observes the logic value.
	 */
	template<typename LogicContainer> void EncodeLogicValueDetector(const LogicContainer& target, Basic::Logic value, const Tpg::LogicContainer01& out);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic container fulfills
	 * the passed constraint, or LOGIC_ZERO if not.
	 *
	 * This method is meant to encode additional logic based on a logic container during solving.
	 * If you meant to retrieve a logic value from a logic container after solving then have a look at the
	 * GetSolvedLogicValue method.
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainer01 cont1IsNotUnknown = NewLogicContainer<LogicContainer01>();
	 *     EncodeLogicValueConstraintDetector(cont1, LogicConstraint::NOT_LOGIC_UNKNOWN, cont1IsNotUnknown);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The target logic container to observe the logic value from.
	 * @param constraint The logic constraint to detect in the logic container.
	 * @param out The logic container that observes the logic value.
	 */
	template<typename LogicContainer> void EncodeLogicValueConstraintDetector(const LogicContainer& target, Basic::LogicConstraint constraint, const Tpg::LogicContainer01& out);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic containers
	 * are exactly equal, or LOGIC_ZERO if they differ.
	 *
	 * This detector does return LOGIC_ONE if the logic values are exactly the same.
	 *
	 * Logic table:
	 *
	 *     container 1 | container 2 | result
	 *     ------------+-------------+-------
	 *     UNKNOWN     | UNKNOWN     | ONE
	 *     ZERO        | ZERO        | ONE
	 *     ONE         | ONE         | ONE
	 *     DONT_CARE   | DONT_CARE   | ONE
	 *     *           | *           | ZERO
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainerU01X cont2 = ...;
	 *     LogicContainer01 contAreEqual = NewLogicContainer<LogicContainer01>();
	 *     EncodeLogicEquivalenceDetector(cont1, cont2, contAreEqual);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container to compare the value from.
	 * @param in2 The second logic container to compare the value from.
	 * @param out The logic container that observes the logic equivalence.
	 */
	template<typename LogicContainer> void EncodeLogicEquivalenceDetector(const LogicContainer& in1, const LogicContainer& in2, const Tpg::LogicContainer01& out);

	/**
	 * @brief Encodes a detector that yields the logic value LOGIC_ONE when the target logic containers
	 * are 0/1-different, or LOGIC_ZERO otherwise.
	 *
	 * This detector does return LOGIC_ONE if the logic values are either
	 * LOGIC_ZERO or LOGIC_ONE and are not equal.
	 *
	 * Logic table:
	 *
	 *     container 1 | container 2 | result
	 *     ------------+-------------+-------
	 *     ZERO        | ONE         | ONE
	 *     ONE         | ZERO        | ONE
	 *     *           | *           | ZERO
	 *
	 * Usage:
	 *
	 *     LogicContainerU01X cont1 = ...;
	 *     LogicContainerU01X cont2 = ...;
	 *     LogicContainer01 contAre01Diff = NewLogicContainer<LogicContainer01>();
	 *     EncodeLogic01DifferenceDetector(cont1, cont2, contAre01Diff);
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container to compare the value from.
	 * @param in2 The second logic container to compare the value from.
	 * @param out The logic container that observes the logic 0/1-difference.
	 */
	template<typename LogicContainer> void EncodeLogic01DifferenceDetector(const LogicContainer& in1, const LogicContainer& in2, const Tpg::LogicContainer01& out);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValue(target, value) method which
	 * encodes multiple requirements at once.
	 * @see EncodeLogicValue(target, value)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param targets The target logic containers.
	 * @param values The target logic values.
	 */
	template<typename LogicContainer> void EncodeLogicValue(const std::vector<LogicContainer>& targets, const std::vector<Basic::Logic>& values);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValueConstraint(target, constraint) method which
	 * encodes multiple constraints at once.
	 * @see EncodeLogicValueConstraint(target, constraint)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param targets The target logic containers.
	 * @param constraints The target logic constraints.
	 */
	template<typename LogicContainer> void EncodeLogicValueConstraint(const std::vector<LogicContainer>& targets, const std::vector<Basic::LogicConstraint>& constraints);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicEquivalenceConstraint(in1, in2) method which
	 * encodes multiple equivalence constraints at once.
	 * @see EncodeLogicEquivalenceConstraint(in1, in2)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 */
	template<typename LogicContainer> void EncodeLogicEquivalenceConstraint(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogic01DifferenceConstraint(in1, in2) method which
	 * encodes multiple 01-difference constraints at once.
	 * @see EncodeLogic01DifferenceConstraint(in1, in2)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 */
	template<typename LogicContainer> void EncodeLogic01DifferenceConstraint(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValueDetector(target, value) method which
	 * encodes multiple value detectors at once.
	 * @see EncodeLogicValueDetector(target, value)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param targets The list of logic containers that are observed.
	 * @param values The list of logic values that are observed in the logic containers.
	 * @return std::vector<LogicContainer01> A list of logic containers that yield a value of LOGIC_ONE if the targeted logic container matches the logic value.
	 */
	template<typename LogicContainer> std::vector<Tpg::LogicContainer01> EncodeLogicValueDetector(const std::vector<LogicContainer>& targets, std::vector<Basic::Logic>& values);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValueConstraintDetector(target, value) method which
	 * encodes multiple value detectors at once.
	 * @see EncodeLogicValueConstraintDetector(target, value)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The list of logic containers that are observed.
	 * @param constraints The list of logic constraints that are observed in the logic containers.
	 * @return std::vector<LogicContainer01> A list of logic containers that yield a value of LOGIC_ONE if the targeted logic container matches the logic constraint.
	 */
	template<typename LogicContainer> std::vector<Tpg::LogicContainer01> EncodeLogicValueConstraintDetector(const std::vector<LogicContainer>& targets, std::vector<Basic::LogicConstraint>& constraints);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicEquivalenceDetector(in1, in2) method which
	 * encodes multiple equivalence detectors at once.
	 * @see EncodeLogicEquivalenceDetector(in1, in2)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 * @return std::vector<LogicContainer01> A list of logic containers that yield a value of LOGIC_ONE if the targeted logic containers are equivalent.
	 */
	template<typename LogicContainer> std::vector<Tpg::LogicContainer01> EncodeLogicEquivalenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogic01DifferenceDetector(in1, in2) method which
	 * encodes multiple 01-difference detectors at once.
	 * @see EncodeLogic01DifferenceDetector(in1, in2)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 * @return std::vector<LogicContainer01> A list of logic containers that yield a value of LOGIC_ONE if the targeted logic containers show a 01-difference.
	 */
	template<typename LogicContainer> std::vector<Tpg::LogicContainer01> EncodeLogic01DifferenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValueDetector(target, value, out) method which
	 * encodes multiple value detectors at once.
	 * @see EncodeLogicValueDetector(target, value, out)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param targets The list of logic containers that are observed.
	 * @param values The list of logic values that are observed in the logic containers.
	 * @param outs The list of logic containers that are constrained to yield a value of LOGIC_ONE if the targeted logic container matches the logic value.
	 */
	template<typename LogicContainer> void EncodeLogicValueDetector(const std::vector<LogicContainer>& targets, const std::vector<Basic::Logic>& values, const std::vector<Tpg::LogicContainer01>& outs);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicValueConstraintDetector(target, value) method which
	 * encodes multiple value detectors at once.
	 * @see EncodeLogicValueConstraintDetector(target, value)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param target The list of logic containers that are observed.
	 * @param constraints The list of logic constraints that are observed in the logic containers.
	 * @param outs The list of logic containers that are constrained yield a value of LOGIC_ONE if the targeted logic container matches the logic constraint.
	 */
	template<typename LogicContainer> void EncodeLogicValueConstraintDetector(const std::vector<LogicContainer>& targets, const std::vector<Basic::LogicConstraint>& constraints, const std::vector<Tpg::LogicContainer01>& outs);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogicEquivalenceDetector(in1, in2, outs) method which
	 * encodes multiple equivalence detectors at once.
	 * @see EncodeLogicEquivalenceDetector(in1, in2, outs)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 * @param outs The list of logic containers that are constrained to yield a value of LOGIC_ONE if the targeted logic containers are equivalent.
	 */
	template<typename LogicContainer> void EncodeLogicEquivalenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<Tpg::LogicContainer01>& outs);

	/**
	 * @brief This is the SIMD-like vector version of the EncodeLogic01DifferenceDetector(in1, in2, out) method which
	 * encodes multiple 01-difference detectors at once.
	 * @see EncodeLogic01DifferenceDetector(in1, in2, out)
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first list of logic containers.
	 * @param in2 The second list of logic containers.
	 * @param outs The list of logic containers that are constrained to yield a value of LOGIC_ONE if the targeted logic containers show a 01-difference.
	 */
	template<typename LogicContainer> void EncodeLogic01DifferenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<Tpg::LogicContainer01>& outs);

	/*
	 * Encoding methods for basic logic gates, like buffers, inverters, and, nand, or, nor, xor, xnor
	 * and multiplexer gates.
	 */

	template<typename LogicContainer> LogicContainer EncodeBuffer(const LogicContainer& in);
	template<typename LogicContainer> LogicContainer EncodeInverter(const LogicContainer& in);
	template<typename LogicContainer> LogicContainer EncodeXor(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeAnd(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeOr(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeXnor(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeNand(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeNor(const LogicContainer& in1, const LogicContainer& in2);
	template<typename LogicContainer> LogicContainer EncodeTristateBuffer(const LogicContainer& in, const LogicContainer& en);
	template<typename LogicContainer> LogicContainer EncodeTristateInverter(const LogicContainer& in, const LogicContainer& en);
	template<typename LogicContainer> LogicContainer EncodeMultiplexer(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& sel);
	template<typename LogicContainer> LogicContainer EncodeSetControl(const LogicContainer& in, const LogicContainer& set);
	template<typename LogicContainer> LogicContainer EncodeResetControl(const LogicContainer& in, const LogicContainer& reset);

	/*
	 * Encoding methods which encode multi-input gates as tree-like structures.
	 *
	 * Example: (cont1...cont5 are pre-existing logic containers)
	 *
	 *     EncodeNand({ cont1, cont2, cont3, cont4, cont5 })
	 *
	 *     cont1  +-----+
	 *     -------+     |
	 *     cont2  | AND +--+
	 *     -------+     |  |  +-----+
	 *            +-----+  +--+     |     +------+
	 *                        | AND +-----+      |
	 *     cont3  +-----+  +--+     |     | NAND +--- out
	 *     -------+     |  |  +-----+  +--+      |
	 *     cont4  | AND +--+           |  +------+
	 *     -------+     |              |
	 *            +-----+              |
	 *     cont5                       |
	 *     ----------------------------+
	 */

	template<typename LogicContainer> LogicContainer EncodeXor(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> LogicContainer EncodeAnd(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> LogicContainer EncodeOr(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> LogicContainer EncodeXnor(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> LogicContainer EncodeNand(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> LogicContainer EncodeNor(const std::vector<LogicContainer>& in);

	/*
	 * SIMD-like encoding methods for basic logic gates, like buffers, inverters, and, nand, or, nor, xor, xnor
	 * and multiplexer gates that encode multiple gates at once.
	 */

	template<typename LogicContainer> std::vector<LogicContainer> EncodeBuffer(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeInverter(const std::vector<LogicContainer>& in);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeXor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeAnd(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeOr(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeXnor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeNand(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeNor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeTristateBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeTristateInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeMultiplexer(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& sel);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeSetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& set);
	template<typename LogicContainer> std::vector<LogicContainer> EncodeResetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& reset);

	/*
	 * Encoding methods for basic logic gates, like buffers, inverters, and, nand, or, nor, xor, xnor
	 * and multiplexer gates. If one of the inputs might be constant then the method variants that return
	 * a logic container should be preferred.
	 */

	template<typename LogicContainer> void EncodeBuffer(const LogicContainer& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeInverter(const LogicContainer& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeXor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeAnd(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeOr(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeXnor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeNand(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeNor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out);
	template<typename LogicContainer> void EncodeTristateBuffer(const LogicContainer& in, const LogicContainer& en, const LogicContainer& out);
	template<typename LogicContainer> void EncodeTristateInverter(const LogicContainer& in, const LogicContainer& en, const LogicContainer& out);
	template<typename LogicContainer> void EncodeMultiplexer(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& sel, const LogicContainer& out);
	template<typename LogicContainer> void EncodeSetControl(const LogicContainer& in, const LogicContainer& set, const LogicContainer& out);
	template<typename LogicContainer> void EncodeResetControl(const LogicContainer& in, const LogicContainer& reset, const LogicContainer& out);

	/*
	 * Encoding methods which encode multi-input gates as tree-like structures.
	 * If one of the inputs might be constant then the method variants that return
	 * a logic container should be preferred.
	 *
	 * Example: (cont1...cont5 are pre-existing logic containers)
	 *
	 *     EncodeNand({ cont1, cont2, cont3, cont4, cont5 })
	 *
	 *     cont1  +-----+
	 *     -------+     |
	 *     cont2  | AND +--+
	 *     -------+     |  |  +-----+
	 *            +-----+  +--+     |     +------+
	 *                        | AND +-----+      |
	 *     cont3  +-----+  +--+     |     | NAND +--- out
	 *     -------+     |  |  +-----+  +--+      |
	 *     cont4  | AND +--+           |  +------+
	 *     -------+     |              |
	 *            +-----+              |
	 *     cont5                       |
	 *     ----------------------------+
	 */

	template<typename LogicContainer> void EncodeXor(const std::vector<LogicContainer>& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeAnd(const std::vector<LogicContainer>& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeOr(const std::vector<LogicContainer>& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeXnor(const std::vector<LogicContainer>& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeNand(const std::vector<LogicContainer>& in, const LogicContainer& out);
	template<typename LogicContainer> void EncodeNor(const std::vector<LogicContainer>& in, const LogicContainer& out);

	/*
	 * SIMD-like encoding methods for basic logic gates, like buffers, inverters, and, nand, or, nor, xor, xnor
	 * and multiplexer gates that encode multiple gates at once.
	 */

	template<typename LogicContainer> void EncodeBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeXor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeAnd(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeOr(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeXnor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeNand(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeNor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeTristateBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeTristateInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeMultiplexer(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& sel, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeSetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& reset, const std::vector<LogicContainer>& out);
	template<typename LogicContainer> void EncodeResetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& reset, const std::vector<LogicContainer>& out);

	/**
	 * @brief Encodes an equality for the passed logic containers.
	 *
	 * Note that this requires additional clauses to be included.
	 * Prefer to reuse an existing logic container directly if you can.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container.
	 * @param in2 The second logic container.
	 */
	template<typename LogicContainer> void EncodeEquality(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Encodes an in-equality for the passed logic containers.
	 *
	 * Note that this requires additional clauses to be included.
	 * Prefer to reuse an existing logic container directly if you can.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param in1 The first logic container.
	 * @param in2 The second logic container.
	 */
	template<typename LogicContainer> void EncodeInequality(const LogicContainer& in1, const LogicContainer& in2);

	/**
	 * @brief Creates a new input logic container by using unassigned variables from the solver.
	 *
	 * The logic container has by default no constraints applied to it.
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @return The new logic container with newly created variables.
	 */
	template<typename LogicContainer> LogicContainer NewInputContainer(void);

	/**
	 * @brief Creates a new state logic container (latch / flip-flop) by using unassigned variables from the solver.
	 *
	 * The logic container has by default no constraints applied to it.
	 *
	 * @tparam LogicContainer The desired logic container type to create.
	 * @return The new logic container with newly created variables.
	 */
	template<typename LogicContainer> LogicContainer NewStateContainer(void);

	/**
	 * @brief Encodes the next state for the output logic container.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param out The output logic container representing a latch / flip-flop.
	 * @param next The next state to apply to the output.
	 */
	template<typename LogicContainer> void EncodeNextState(const LogicContainer& out, const LogicContainer& next);

	/**
	 * @brief Encodes the starting value of the output logic container.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param out The output logic container representing a latch / flip-flop.
	 * @param value The logic value to encode for the initial state.
	 */
	template<typename LogicContainer> void EncodeInitialStateValue(const LogicContainer& out, const Basic::Logic value);

	/**
	 * @brief Encodes the ending value of the output logic container.
	 *
	 * @tparam LogicContainer The logic container type (can be inferred).
	 * @param out The output logic container representing a latch / flip-flop.
	 * @param value The logic value to encode for the target state.
	 */
	template<typename LogicContainer> void EncodeTargetStateValue(const LogicContainer& out, const Basic::Logic value);

	/**
	 * @brief Encodes an implication from the returned logic container to the passed logic container.
	 *
	 * Example: This encodes the implication (this -> that)
	 *
	 *     LogicContainer01 that = ...;
	 *     LogicContainer01 this = EncodeImplication(that).
	 *
	 * @param in The logic container that represents the right side of the implication.
	 * @return LogicContainer01 The logic container that represents the left side of the implication.
	 */
	Tpg::LogicContainer01 EncodeImplication(const Tpg::LogicContainer01& in);

	/**
	 * @brief Encodes multiple implications at the same time.
	 * This is the SIMD-like version of the EncodeImplication(in) method.
	 * @see EncodeImplication(in)
	 *
	 * @param in The logic containers that represents the right sides of the implication.
	 * @return The logic containers that represents the left side of the implication.
	 */
	Tpg::LogicContainer01 EncodeImplication(const std::vector<Tpg::LogicContainer01>& in);

	/**
	 * @brief Encodes an implication from the first logic container to the second logic container.
	 *
	 * Example: This encodes the implication (in1 -> in2)
	 *
	 *     LogicContainer01 in1 = ...;
	 *     LogicContainer01 in2 = ...;
	 *     EncodeImplication(in1, in2).
	 *
	 * @param in1 The logic container that represents the left side of the implication.
	 * @param in2 The logic container that represents the right side of the implication.
	 */
	void EncodeImplication(const Tpg::LogicContainer01& in1, const Tpg::LogicContainer01& in2);

	/**
	 * @brief Encodes an implication from the first logic container to a clause consisting
	 * of the literals from the list of logic containers passed a second parameter.
	 *
	 * Example: This encodes the implication (in1 -> (in2 v in3 v in4))
	 *
	 *     LogicContainer01 in1 = ...; LogicContainer01 in2 = ...;
	 *     LogicContainer01 in3 = ...; LogicContainer01 in4 = ...;
	 *     EncodeImplication(in1, { in2, in3, in4 }).
	 *
	 * @param in1 The logic container that represents the left side of the implication.
	 * @param in2 The logic container that represents the right side of the implication.
	 */
	void EncodeImplication(const Tpg::LogicContainer01& in1, const std::vector<Tpg::LogicContainer01>& in);

	/**
	 * @brief Encodes a single clause based on the literals passed in the logic containers.
	 *
	 * @param in The logic containers holding the literals to create a single clause.
	 */
	void EncodeClause(const std::vector<Tpg::LogicContainer01>& in);

protected:
	std::shared_ptr<SolverProxy::ISolverProxy> _solver;

};

template<typename T> struct Literal
{
	using Type = T;
};
struct Literal0 : Literal<SolverProxy::BaseLiteral> {};
struct Literal1 : Literal<SolverProxy::BaseLiteral> {};

};
};
