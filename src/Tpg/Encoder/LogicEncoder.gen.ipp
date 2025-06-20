// Automatic generated file - do not edit!
// Use the gate logic optimization workflow to re-generate it.

#include "Tpg/Encoder/LogicEncoder.hpp"

#ifdef BASIC_LOGIC_ENCODER_GEN

namespace FreiTest
{
namespace Tpg
{

template<typename LogicContainer>
void LogicEncoder::EncodeBuffer(const LogicContainer& in, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUF
			Input Encodings: 01
			Output Encodings: 01

			    in    | out
			    l0    | l0
			----------+--------
			 0  0 (0) | 0 (0)
			 1  1 (1) | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [1]
			initial neg: [0]
			initial nc:  []
			initial cost: 2 terms with a total of 2 literals

			  out   |   in    |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 1  |
			        +----+----+

			pos: [1]
			neg: [0]
			cost: 2 terms with a total of 2 literals

			( in.l0) ->  out.l0
			(-in.l0) -> -out.l0
		*/
		_solver->CommitClause(-in.l0,  out.l0);
		_solver->CommitClause( in.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUF
			Input Encodings: 01X
			Output Encodings: 01X

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (X) | 0  0 (X)
			 1  0  1 (0) | 0  1 (0)
			 2  1  0 (1) | 1  0 (1)
			 3  *  * (*) | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [10]
			initial neg: [00, 01]
			initial nc:  [11]
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |      out   |   in    |
			  l0    |    | l1 |      l0    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 0  |            | 0  | 0  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 1  | *  |         l0 | 1  | 1  |
			--------+----+----+    --------+----+----+

			pos: [1*]
			neg: [0*]
			cost: 2 terms with a total of 2 literals

			( in.l0         ) ->  out.l0
			(-in.l0         ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [01]
			initial neg: [00, 10]
			initial nc:  [11]
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |      out   |   in    |
			  l1    |    | l1 |      l1    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 1  |            | 0  | 1  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 0  | *  |         l0 | 0  | 1  |
			--------+----+----+    --------+----+----+

			pos: [*1]
			neg: [*0]
			cost: 2 terms with a total of 2 literals

			(          in.l1) ->  out.l1
			(         -in.l1) -> -out.l1
		*/
		_solver->CommitClause(-in.l0,          out.l0);
		_solver->CommitClause( in.l0,         -out.l0);
		_solver->CommitClause(        -in.l1,  out.l1);
		_solver->CommitClause(         in.l1, -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUF
			Input Encodings: U01X
			Output Encodings: U01X

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (U) | 0  0 (U)
			 1  0  1 (0) | 0  1 (0)
			 2  1  0 (1) | 1  0 (1)
			 3  1  1 (X) | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 11]
			initial neg: [00, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |
			  l0    |    | l1 |
			--------+----+----+
			        | 0  | 0  |
			 in ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [1*]
			neg: [0*]
			cost: 2 terms with a total of 2 literals

			( in.l0         ) ->  out.l0
			(-in.l0         ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [01, 11]
			initial neg: [00, 10]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |
			  l1    |    | l1 |
			--------+----+----+
			        | 0  | 1  |
			 in ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [*1]
			neg: [*0]
			cost: 2 terms with a total of 2 literals

			(          in.l1) ->  out.l1
			(         -in.l1) -> -out.l1
		*/
		_solver->CommitClause(-in.l0,          out.l0);
		_solver->CommitClause( in.l0,         -out.l0);
		_solver->CommitClause(        -in.l1,  out.l1);
		_solver->CommitClause(         in.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUF
			Input Encodings: 01F
			Output Encodings: 01F

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (X) | 0  0 (X)
			 1  0  F (0) | 0  F (0)
			 2  F  0 (1) | F  0 (1)
			 3  *  * (*) | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [00, 01]
			initial nc:  [11]
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |      out   |   in    |
			  l0    |    | l1 |      l0    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 0  |            | 0  | 0  |
			 in ----+----+----+     in ----+----+----+
			     l0 | F  | *  |         l0 | *  | *  |
			--------+----+----+    --------+----+----+

			pos: []
			neg: [0*]
			cost: 1 terms with a total of 1 literals

			(-in.l0         ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [00, 10]
			initial nc:  [11]
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |      out   |   in    |
			  l1    |    | l1 |      l1    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | F  |            | 0  | *  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 0  | *  |         l0 | 0  | *  |
			--------+----+----+    --------+----+----+

			pos: []
			neg: [*0]
			cost: 1 terms with a total of 1 literals

			(         -in.l1) -> -out.l1
		*/
		_solver->CommitClause( in.l0,         -out.l0);
		_solver->CommitClause(         in.l1, -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUF
			Input Encodings: U01F
			Output Encodings: U01F

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (U) | 0  0 (U)
			 1  F  1 (0) | F  1 (0)
			 2  1  F (1) | 1  F (1)
			 3  1  1 (X) | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |
			  l0    |    | l1 |
			--------+----+----+
			        | 0  | F  |
			 in ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [1*]
			neg: [00]
			cost: 2 terms with a total of 3 literals

			( in.l0         ) ->  out.l0
			(-in.l0 & -in.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [01, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |
			  l1    |    | l1 |
			--------+----+----+
			        | 0  | 1  |
			 in ----+----+----+
			     l0 | F  | 1  |
			--------+----+----+

			pos: [*1]
			neg: [00]
			cost: 2 terms with a total of 3 literals

			(          in.l1) ->  out.l1
			(-in.l0 & -in.l1) -> -out.l1
		*/
		_solver->CommitClause(-in.l0,          out.l0);
		_solver->CommitClause( in.l0,  in.l1, -out.l0);
		_solver->CommitClause(        -in.l1,  out.l1);
		_solver->CommitClause( in.l0,  in.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeInverter(const LogicContainer& in, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: INV
			Input Encodings: 01
			Output Encodings: 01

			    in    | out
			    l0    | l0
			----------+--------
			 0  0 (0) | 1 (1)
			 1  1 (1) | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [0]
			initial neg: [1]
			initial nc:  []
			initial cost: 2 terms with a total of 2 literals

			  out   |   in    |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 0  |
			        +----+----+

			pos: [0]
			neg: [1]
			cost: 2 terms with a total of 2 literals

			(-in.l0) ->  out.l0
			( in.l0) -> -out.l0
		*/
		_solver->CommitClause( in.l0,  out.l0);
		_solver->CommitClause(-in.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: INV
			Input Encodings: 01X
			Output Encodings: 01X

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (X) | 0  0 (X)
			 1  0  1 (0) | 1  0 (1)
			 2  1  0 (1) | 0  1 (0)
			 3  *  * (*) | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [01]
			initial neg: [00, 10]
			initial nc:  [11]
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |      out   |   in    |
			  l0    |    | l1 |      l0    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 1  |            | 0  | 1  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 0  | *  |         l0 | 0  | 1  |
			--------+----+----+    --------+----+----+

			pos: [*1]
			neg: [*0]
			cost: 2 terms with a total of 2 literals

			(          in.l1) ->  out.l0
			(         -in.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [10]
			initial neg: [00, 01]
			initial nc:  [11]
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |      out   |   in    |
			  l1    |    | l1 |      l1    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 0  |            | 0  | 0  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 1  | *  |         l0 | 1  | 1  |
			--------+----+----+    --------+----+----+

			pos: [1*]
			neg: [0*]
			cost: 2 terms with a total of 2 literals

			( in.l0         ) ->  out.l1
			(-in.l0         ) -> -out.l1
		*/
		_solver->CommitClause(        -in.l1,  out.l0);
		_solver->CommitClause(         in.l1, -out.l0);
		_solver->CommitClause(-in.l0,          out.l1);
		_solver->CommitClause( in.l0,         -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: INV
			Input Encodings: U01X
			Output Encodings: U01X

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (U) | 0  0 (U)
			 1  0  1 (0) | 1  0 (1)
			 2  1  0 (1) | 0  1 (0)
			 3  1  1 (X) | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [01, 11]
			initial neg: [00, 10]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |
			  l0    |    | l1 |
			--------+----+----+
			        | 0  | 1  |
			 in ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [*1]
			neg: [*0]
			cost: 2 terms with a total of 2 literals

			(          in.l1) ->  out.l0
			(         -in.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [10, 11]
			initial neg: [00, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in    |
			  l1    |    | l1 |
			--------+----+----+
			        | 0  | 0  |
			 in ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [1*]
			neg: [0*]
			cost: 2 terms with a total of 2 literals

			( in.l0         ) ->  out.l1
			(-in.l0         ) -> -out.l1
		*/
		_solver->CommitClause(        -in.l1,  out.l0);
		_solver->CommitClause(         in.l1, -out.l0);
		_solver->CommitClause(-in.l0,          out.l1);
		_solver->CommitClause( in.l0,         -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: INV
			Input Encodings: 01F
			Output Encodings: 01F

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (X) | 0  0 (X)
			 1  0  F (0) | F  0 (1)
			 2  F  0 (1) | 0  F (0)
			 3  *  * (*) | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [00, 10]
			initial nc:  [11]
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |      out   |   in    |
			  l0    |    | l1 |      l0    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | F  |            | 0  | *  |
			 in ----+----+----+     in ----+----+----+
			     l0 | 0  | *  |         l0 | 0  | *  |
			--------+----+----+    --------+----+----+

			pos: []
			neg: [*0]
			cost: 1 terms with a total of 1 literals

			(         -in.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [00, 01]
			initial nc:  [11]
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |      out   |   in    |
			  l1    |    | l1 |      l1    |    | l1 |
			--------+----+----+    --------+----+----+
			        | 0  | 0  |            | 0  | 0  |
			 in ----+----+----+     in ----+----+----+
			     l0 | F  | *  |         l0 | *  | *  |
			--------+----+----+    --------+----+----+

			pos: []
			neg: [0*]
			cost: 1 terms with a total of 1 literals

			(-in.l0         ) -> -out.l1
		*/
		_solver->CommitClause(         in.l1, -out.l0);
		_solver->CommitClause( in.l0,         -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: INV
			Input Encodings: U01F
			Output Encodings: U01F

			    in       | out
			    l1 l0    | l1 l0
			-------------+-----------
			 0  0  0 (U) | 0  0 (U)
			 1  F  1 (0) | 1  F (1)
			 2  1  F (1) | F  1 (0)
			 3  1  1 (X) | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [01, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |
			  l0    |    | l1 |
			--------+----+----+
			        | 0  | 1  |
			 in ----+----+----+
			     l0 | F  | 1  |
			--------+----+----+

			pos: [*1]
			neg: [00]
			cost: 2 terms with a total of 3 literals

			(          in.l1) ->  out.l0
			(-in.l0 & -in.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [10, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 3 terms with a total of 6 literals

			  out   |   in    |
			  l1    |    | l1 |
			--------+----+----+
			        | 0  | F  |
			 in ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [1*]
			neg: [00]
			cost: 2 terms with a total of 3 literals

			( in.l0         ) ->  out.l1
			(-in.l0 & -in.l1) -> -out.l1
		*/
		_solver->CommitClause(        -in.l1,  out.l0);
		_solver->CommitClause( in.l0,  in.l1, -out.l0);
		_solver->CommitClause(-in.l0,          out.l1);
		_solver->CommitClause( in.l0,  in.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeXor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XOR
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 01]
			initial neg: [00, 11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 1  |
			in1 ----+----+----+
			     l0 | 1  | 0  |
			--------+----+----+

			pos: [01, 10]
			neg: [00, 11]
			cost: 4 terms with a total of 8 literals

			(-in1.l0 &  in2.l0) ->  out.l0
			( in1.l0 & -in2.l0) ->  out.l0
			(-in1.l0 & -in2.l0) -> -out.l0
			( in1.l0 &  in2.l0) -> -out.l0
		*/
		_solver->CommitClause( in1.l0, -in2.l0,  out.l0);
		_solver->CommitClause(-in1.l0,  in2.l0,  out.l0);
		_solver->CommitClause( in1.l0,  in2.l0, -out.l0);
		_solver->CommitClause(-in1.l0, -in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XOR
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 0101]
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1, 1*1*]
			neg: [*00*, 0**0]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			( in1.l0 &            in2.l0          ) ->  out.l0
			(          -in1.l1 & -in2.l0          ) -> -out.l0
			(-in1.l0 &                     -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*, 1**1]
			neg: [*0*0, 0*0*]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			(          -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 &           -in2.l0          ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XOR
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110, 0001, 1001, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1, 1*1*]
			neg: [**00, *00*, 0**0, 00**]
			cost: 6 terms with a total of 12 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			( in1.l0 &            in2.l0          ) ->  out.l0
			(                    -in2.l0 & -in2.l1) -> -out.l0
			(          -in1.l1 & -in2.l0          ) -> -out.l0
			(-in1.l0 &                     -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 0001, 0101, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*11*, 1**1]
			neg: [**00, *0*0, 0*0*, 00**]
			cost: 6 terms with a total of 12 literals

			(           in1.l1 &  in2.l0          ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			(                    -in2.l0 & -in2.l1) -> -out.l1
			(          -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 &           -in2.l0          ) -> -out.l1
			(-in1.l0 & -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XOR
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  F (0)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | F  0 (1)
			 10  F  0 (1) F  0 (1)  | 0  F (0)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*00*, 0**0]
			cost: 2 terms with a total of 4 literals

			(          -in1.l1 & -in2.l0          ) -> -out.l0
			(-in1.l0 &                     -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*0*0, 0*0*]
			cost: 2 terms with a total of 4 literals

			(          -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 &           -in2.l0          ) -> -out.l1
		*/
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XOR
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | F  1 (0)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | 1  F (1)
			 10  1  F (1) 1  F (1)  | F  1 (0)
			 11  1  F (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0001, 0011]
			initial nc:  []
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1, 1*1*]
			neg: [**00, 00**]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			( in1.l0 &            in2.l0          ) ->  out.l0
			(                    -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0001, 0011]
			initial nc:  []
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*11*, 1**1]
			neg: [**00, 00**]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			(                    -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeAnd(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: AND
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 0 (0)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [11]
			initial neg: [00, 10, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 0  |
			in1 ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [11]
			neg: [*0, 0*]
			cost: 3 terms with a total of 4 literals

			( in1.l0 &  in2.l0) ->  out.l0
			(          -in2.l0) -> -out.l0
			(-in1.l0          ) -> -out.l0
		*/
		_solver->CommitClause(-in1.l0, -in2.l0,  out.l0);
		_solver->CommitClause(          in2.l0, -out.l0);
		_solver->CommitClause( in1.l0,          -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: AND
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  1 (0)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  1 (0)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1001]
			initial neg: [0000, 0100, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 1  | 1  | *  | 1  |                | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 1  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [**1*, 1***]
			neg: [0*0*]
			cost: 3 terms with a total of 4 literals

			(                     in2.l0          ) ->  out.l0
			( in1.l0                              ) ->  out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0101]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			(                              -in2.l1) -> -out.l1
			(          -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(                  -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                             out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(                            in2.l1, -out.l1);
		_solver->CommitClause(          in1.l1,                   -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: AND
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  1 (0)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  1 (0)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) 1  1 (X)  | 0  1 (0)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 0  1 (0)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0101, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 1  | 0  | 0  |
			        +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**10, *11*, 1**1, 10**]
			neg: [*100, 0*0*, 00*1]
			cost: 7 terms with a total of 16 literals

			(                     in2.l0 & -in2.l1) ->  out.l0
			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			( in1.l0 & -in1.l1                    ) ->  out.l0
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0101, 1101, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 0110, 1110, 0001, 1001, 0011, 1011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			(                              -in2.l1) -> -out.l1
			(          -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(                  -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,                    out.l0);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(                            in2.l1, -out.l1);
		_solver->CommitClause(          in1.l1,                   -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: AND
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  F (0)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  F (0)
			  5  0  F (0) 0  F (0)  | 0  F (0)
			  6  0  F (0) F  0 (1)  | 0  F (0)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | 0  F (0)
			 10  F  0 (1) F  0 (1)  | F  0 (1)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 0100, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 11 terms with a total of 44 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | F  | F  | *  | F  |                | *  | *  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [0*0*]
			cost: 1 terms with a total of 2 literals

			(-in1.l0 &           -in2.l0          ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [***0, *0**]
			cost: 2 terms with a total of 2 literals

			(                              -in2.l1) -> -out.l1
			(          -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(                            in2.l1, -out.l1);
		_solver->CommitClause(          in1.l1,                   -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: AND
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | F  1 (0)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | F  1 (0)
			  5  F  1 (0) F  1 (0)  | F  1 (0)
			  6  F  1 (0) 1  F (1)  | F  1 (0)
			  7  F  1 (0) 1  1 (X)  | F  1 (0)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | F  1 (0)
			 10  1  F (1) 1  F (1)  | 1  F (1)
			 11  1  F (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | F  1 (0)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0011]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 1  | 0  | 0  |
			        +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**10, *11*, 1**1, 10**]
			neg: [*100, 00*1, 000*]
			cost: 7 terms with a total of 17 literals

			(                     in2.l0 & -in2.l1) ->  out.l0
			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			( in1.l0 & -in1.l1                    ) ->  out.l0
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 & -in2.l0          ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0101, 1101, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0011]
			initial nc:  []
			initial cost: 9 terms with a total of 36 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | F  | 0  | 0  |
			        +----+----+----+----+----
			        | F  | F  | F  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | F  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1]
			neg: [*100, 0*00, 00*1]
			cost: 4 terms with a total of 11 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 &           -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l1
		*/
		_solver->CommitClause(                  -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,                    out.l0);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,          -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeOr(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: OR
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 01, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 1  |
			in1 ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [*1, 1*]
			neg: [00]
			cost: 3 terms with a total of 4 literals

			(           in2.l0) ->  out.l0
			( in1.l0          ) ->  out.l0
			(-in1.l0 & -in2.l0) -> -out.l0
		*/
		_solver->CommitClause(         -in2.l0,  out.l0);
		_solver->CommitClause(-in1.l0,           out.l0);
		_solver->CommitClause( in1.l0,  in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: OR
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 1  0 (1)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 1  0 (1)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010]
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [1*1*]
			neg: [**0*, 0***]
			cost: 3 terms with a total of 4 literals

			( in1.l0 &            in2.l0          ) ->  out.l0
			(                    -in2.l0          ) -> -out.l0
			(-in1.l0                              ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0110, 0001, 1001, 0101]
			initial neg: [0000, 1000, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 1  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 1  | 1  | *  | 1  |                | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [***1, *1**]
			neg: [*0*0]
			cost: 3 terms with a total of 4 literals

			(                               in2.l1) ->  out.l1
			(           in1.l1                    ) ->  out.l1
			(          -in1.l1 &           -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause(                   in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                            -out.l0);
		_solver->CommitClause(                           -in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1,                    out.l1);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: OR
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 1  0 (1)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 1  0 (1)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) 1  1 (X)  | 1  0 (1)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 1  0 (1)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 1011, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110, 0001, 1001, 0101, 1101, 0011, 0111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [1*1*]
			neg: [**0*, 0***]
			cost: 3 terms with a total of 4 literals

			( in1.l0 &            in2.l0          ) ->  out.l0
			(                    -in2.l0          ) -> -out.l0
			(-in1.l0                              ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0110, 1110, 0001, 1001, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 1100, 0010, 1010, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 1  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**01, *11*, 01**, 1**1]
			neg: [*0*0, 001*, 1*00]
			cost: 7 terms with a total of 16 literals

			(                    -in2.l0 &  in2.l1) ->  out.l1
			(           in1.l1 &  in2.l0          ) ->  out.l1
			(-in1.l0 &  in1.l1                    ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			(          -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l1
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause(                   in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                            -out.l0);
		_solver->CommitClause(                   in2.l0, -in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause( in1.l0, -in1.l1,                    out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l1);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: OR
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | F  0 (1)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  F (0)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | F  0 (1)
			  9  F  0 (1) 0  F (0)  | F  0 (1)
			 10  F  0 (1) F  0 (1)  | F  0 (1)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, 0***]
			cost: 2 terms with a total of 2 literals

			(                    -in2.l0          ) -> -out.l0
			(-in1.l0                              ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 11 terms with a total of 44 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | F  | F  | *  | F  |                | *  | *  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*0*0]
			cost: 1 terms with a total of 2 literals

			(          -in1.l1 &           -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(                   in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                            -out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: OR
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 1  F (1)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | F  1 (0)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | 1  F (1)
			  9  1  F (1) F  1 (0)  | 1  F (1)
			 10  1  F (1) 1  F (1)  | 1  F (1)
			 11  1  F (1) 1  1 (X)  | 1  F (1)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | 1  F (1)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 1011, 1111]
			initial neg: [0000, 1000, 1100, 0010, 0011]
			initial nc:  []
			initial cost: 9 terms with a total of 36 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | F  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | F  |
			     l1 +----+----+----+----+----
			        | F  | F  | F  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [1*1*]
			neg: [00*0, 001*, 1*00]
			cost: 4 terms with a total of 11 literals

			( in1.l0 &            in2.l0          ) ->  out.l0
			(-in1.l0 & -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l0
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0110, 1110, 0001, 1001, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 1100, 0010, 0011]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 1  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**01, *11*, 01**, 1**1]
			neg: [00*0, 001*, 1*00]
			cost: 7 terms with a total of 17 literals

			(                    -in2.l0 &  in2.l1) ->  out.l1
			(           in1.l1 &  in2.l0          ) ->  out.l1
			(-in1.l0 &  in1.l1                    ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			(-in1.l0 & -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l1
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause(                   in2.l0, -in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause( in1.l0, -in1.l1,                    out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l1);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeXnor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XNOR
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 1 (1)
			 1  0 (0) 1 (1)  | 0 (0)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [00, 11]
			initial neg: [10, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 0  |
			in1 ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [00, 11]
			neg: [01, 10]
			cost: 4 terms with a total of 8 literals

			(-in1.l0 & -in2.l0) ->  out.l0
			( in1.l0 &  in2.l0) ->  out.l0
			(-in1.l0 &  in2.l0) -> -out.l0
			( in1.l0 & -in2.l0) -> -out.l0
		*/
		_solver->CommitClause( in1.l0,  in2.l0,  out.l0);
		_solver->CommitClause(-in1.l0, -in2.l0,  out.l0);
		_solver->CommitClause( in1.l0, -in2.l0, -out.l0);
		_solver->CommitClause(-in1.l0,  in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XNOR
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*, 1**1]
			neg: [*0*0, 0*0*]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010, 0101]
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1, 1*1*]
			neg: [*00*, 0**0]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			( in1.l0 &            in2.l0          ) ->  out.l1
			(          -in1.l1 & -in2.l0          ) -> -out.l1
			(-in1.l0 &                     -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XNOR
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 0001, 0101, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*11*, 1**1]
			neg: [**00, *0*0, 0*0*, 00**]
			cost: 6 terms with a total of 12 literals

			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(                    -in2.l0 & -in2.l1) -> -out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
			(-in1.l0 & -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010, 1110, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110, 0001, 1001, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1, 1*1*]
			neg: [**00, *00*, 0**0, 00**]
			cost: 6 terms with a total of 12 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			( in1.l0 &            in2.l0          ) ->  out.l1
			(                    -in2.l0 & -in2.l1) -> -out.l1
			(          -in1.l1 & -in2.l0          ) -> -out.l1
			(-in1.l0 &                     -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XNOR
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | F  0 (1)
			  6  0  F (0) F  0 (1)  | 0  F (0)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | 0  F (0)
			 10  F  0 (1) F  0 (1)  | F  0 (1)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*0*0, 0*0*]
			cost: 2 terms with a total of 4 literals

			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*00*, 0**0]
			cost: 2 terms with a total of 4 literals

			(          -in1.l1 & -in2.l0          ) -> -out.l1
			(-in1.l0 &                     -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(          in1.l1,  in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                    in2.l1, -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: XNOR
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | 1  F (1)
			  6  F  1 (0) 1  F (1)  | F  1 (0)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | F  1 (0)
			 10  1  F (1) 1  F (1)  | 1  F (1)
			 11  1  F (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0001, 0011]
			initial nc:  []
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*11*, 1**1]
			neg: [**00, 00**]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(                    -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010, 1110, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0001, 0011]
			initial nc:  []
			initial cost: 14 terms with a total of 56 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1, 1*1*]
			neg: [**00, 00**]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &            in2.l1) ->  out.l1
			( in1.l0 &            in2.l0          ) ->  out.l1
			(                    -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1                    ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l0);
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l1);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause(                   in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,                   -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeNand(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NAND
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 1 (1)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [00, 10, 01]
			initial neg: [11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 1  |
			in1 ----+----+----+
			     l0 | 1  | 0  |
			--------+----+----+

			pos: [*0, 0*]
			neg: [11]
			cost: 3 terms with a total of 4 literals

			(          -in2.l0) ->  out.l0
			(-in1.l0          ) ->  out.l0
			( in1.l0 &  in2.l0) -> -out.l0
		*/
		_solver->CommitClause(          in2.l0,  out.l0);
		_solver->CommitClause( in1.l0,           out.l0);
		_solver->CommitClause(-in1.l0, -in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NAND
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 1  0 (1)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 1  0 (1)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0101]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			(                              -in2.l1) -> -out.l0
			(          -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1001]
			initial neg: [0000, 0100, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 1  | 1  | *  | 1  |                | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 1  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [**1*, 1***]
			neg: [0*0*]
			cost: 3 terms with a total of 4 literals

			(                     in2.l0          ) ->  out.l1
			( in1.l0                              ) ->  out.l1
			(-in1.l0 &           -in2.l0          ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(                            in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,                   -out.l0);
		_solver->CommitClause(                  -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                             out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NAND
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 1  0 (1)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 1  0 (1)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 1  0 (1)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 1  0 (1)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0101, 1101, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 0110, 1110, 0001, 1001, 0011, 1011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			(                              -in2.l1) -> -out.l0
			(          -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0101, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 1  | 0  | 0  |
			        +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**10, *11*, 1**1, 10**]
			neg: [*100, 0*0*, 00*1]
			cost: 7 terms with a total of 16 literals

			(                     in2.l0 & -in2.l1) ->  out.l1
			(           in1.l1 &  in2.l0          ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			( in1.l0 & -in1.l1                    ) ->  out.l1
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 &           -in2.l0          ) -> -out.l1
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(                            in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,                   -out.l0);
		_solver->CommitClause(                  -in2.l0,  in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(-in1.l0,  in1.l1,                    out.l1);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NAND
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | F  0 (1)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | F  0 (1)
			  5  0  F (0) 0  F (0)  | F  0 (1)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | F  0 (1)
			 10  F  0 (1) F  0 (1)  | 0  F (0)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [***0, *0**]
			cost: 2 terms with a total of 2 literals

			(                              -in2.l1) -> -out.l0
			(          -in1.l1                    ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 0100, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 11 terms with a total of 44 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | F  | F  | *  | F  |                | *  | *  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [0*0*]
			cost: 1 terms with a total of 2 literals

			(-in1.l0 &           -in2.l0          ) -> -out.l1
		*/
		_solver->CommitClause(                            in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,                   -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NAND
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 1  F (1)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 1  F (1)
			  5  F  1 (0) F  1 (0)  | 1  F (1)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 1  F (1)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | 1  F (1)
			 10  1  F (1) 1  F (1)  | F  1 (0)
			 11  1  F (1) 1  1 (X)  | 1  1 (X)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 1  F (1)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0101, 1101, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0011]
			initial nc:  []
			initial cost: 9 terms with a total of 36 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | F  | 0  | 0  |
			        +----+----+----+----+----
			        | F  | F  | F  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | F  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [*1*1]
			neg: [*100, 0*00, 00*1]
			cost: 4 terms with a total of 11 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0 & -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1000, 0010, 1010, 0110, 1110, 1001, 1101, 1011, 0111, 1111]
			initial neg: [0000, 0100, 1100, 0001, 0011]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 1  | 0  | 0  |
			        +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**10, *11*, 1**1, 10**]
			neg: [*100, 00*1, 000*]
			cost: 7 terms with a total of 17 literals

			(                     in2.l0 & -in2.l1) ->  out.l1
			(           in1.l1 &  in2.l0          ) ->  out.l1
			( in1.l0 &                      in2.l1) ->  out.l1
			( in1.l0 & -in1.l1                    ) ->  out.l1
			(           in1.l1 & -in2.l0 & -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 & -in2.l0          ) -> -out.l1
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause(                  -in2.l0,  in2.l1,  out.l1);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l1);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l1);
		_solver->CommitClause(-in1.l0,  in1.l1,                    out.l1);
		_solver->CommitClause(         -in1.l1,  in2.l0,  in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,          -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeNor(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOR
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 1 (1)
			 1  0 (0) 1 (1)  | 0 (0)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [00]
			initial neg: [10, 01, 11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 0  |
			in1 ----+----+----+
			     l0 | 0  | 0  |
			--------+----+----+

			pos: [00]
			neg: [*1, 1*]
			cost: 3 terms with a total of 4 literals

			(-in1.l0 & -in2.l0) ->  out.l0
			(           in2.l0) -> -out.l0
			( in1.l0          ) -> -out.l0
		*/
		_solver->CommitClause( in1.l0,  in2.l0,  out.l0);
		_solver->CommitClause(         -in2.l0, -out.l0);
		_solver->CommitClause(-in1.l0,          -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOR
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  1 (0)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  1 (0)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0110, 0001, 1001, 0101]
			initial neg: [0000, 1000, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 1  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 1  | 1  | *  | 1  |                | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [***1, *1**]
			neg: [*0*0]
			cost: 3 terms with a total of 4 literals

			(                               in2.l1) ->  out.l0
			(           in1.l1                    ) ->  out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010]
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [1*1*]
			neg: [**0*, 0***]
			cost: 3 terms with a total of 4 literals

			( in1.l0 &            in2.l0          ) ->  out.l1
			(                    -in2.l0          ) -> -out.l1
			(-in1.l0                              ) -> -out.l1
		*/
		_solver->CommitClause(                           -in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1,                    out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause(                   in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                            -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOR
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 0  1 (0)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 1  0 (1)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  1 (0)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) 1  1 (X)  | 0  1 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 0  1 (0)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0110, 1110, 0001, 1001, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 1100, 0010, 1010, 0011]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 1  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**01, *11*, 01**, 1**1]
			neg: [*0*0, 001*, 1*00]
			cost: 7 terms with a total of 16 literals

			(                    -in2.l0 &  in2.l1) ->  out.l0
			(           in1.l1 &  in2.l0          ) ->  out.l0
			(-in1.l0 &  in1.l1                    ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l0
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010, 1110, 1011, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110, 0001, 1001, 0101, 1101, 0011, 0111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [1*1*]
			neg: [**0*, 0***]
			cost: 3 terms with a total of 4 literals

			( in1.l0 &            in2.l0          ) ->  out.l1
			(                    -in2.l0          ) -> -out.l1
			(-in1.l0                              ) -> -out.l1
		*/
		_solver->CommitClause(                   in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,                    out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause(                   in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                            -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOR
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  F (0)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | F  0 (1)
			  6  0  F (0) F  0 (1)  | 0  F (0)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  F (0)
			  9  F  0 (1) 0  F (0)  | 0  F (0)
			 10  F  0 (1) F  0 (1)  | 0  F (0)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 11 terms with a total of 44 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | F  | F  | *  | F  |                | *  | *  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [*0*0]
			cost: 1 terms with a total of 2 literals

			(          -in1.l1 &           -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |          out   |        in2        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, 0***]
			cost: 2 terms with a total of 2 literals

			(                    -in2.l0          ) -> -out.l1
			(-in1.l0                              ) -> -out.l1
		*/
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause(                   in2.l0,          -out.l1);
		_solver->CommitClause( in1.l0,                            -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOR
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     in2      in1       | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | F  1 (0)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | 1  F (1)
			  6  F  1 (0) 1  F (1)  | F  1 (0)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | F  1 (0)
			  9  1  F (1) F  1 (0)  | F  1 (0)
			 10  1  F (1) 1  F (1)  | F  1 (0)
			 11  1  F (1) 1  1 (X)  | F  1 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | F  1 (0)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0110, 1110, 0001, 1001, 0101, 1101, 1011, 0111, 1111]
			initial neg: [0000, 1000, 1100, 0010, 0011]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 1  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**01, *11*, 01**, 1**1]
			neg: [00*0, 001*, 1*00]
			cost: 7 terms with a total of 17 literals

			(                    -in2.l0 &  in2.l1) ->  out.l0
			(           in1.l1 &  in2.l0          ) ->  out.l0
			(-in1.l0 &  in1.l1                    ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(-in1.l0 & -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l0
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [1010, 1110, 1011, 1111]
			initial neg: [0000, 1000, 1100, 0010, 0011]
			initial nc:  []
			initial cost: 9 terms with a total of 36 literals

			  out   |        in2        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | F  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			in1----+----+----+----+----+ l0
			        | 0  | 1  | 1  | F  |
			     l1 +----+----+----+----+----
			        | F  | F  | F  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [1*1*]
			neg: [00*0, 001*, 1*00]
			cost: 4 terms with a total of 11 literals

			( in1.l0 &            in2.l0          ) ->  out.l1
			(-in1.l0 & -in1.l1 &           -in2.l1) -> -out.l1
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l1
			( in1.l0 &           -in2.l0 & -in2.l1) -> -out.l1
		*/
		_solver->CommitClause(                   in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,                    out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,           in2.l1, -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l1);
		_solver->CommitClause(-in1.l0,           in2.l0,  in2.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeTristateBuffer(const LogicContainer& in, const LogicContainer& en, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(en);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUFIF
			Input Encodings: 01, 01
			Output Encodings: 01

			    en    in     | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 0 (0)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [11]
			initial neg: [00, 10, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   en    |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 0  |
			 in ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [11]
			neg: [*0, 0*]
			cost: 3 terms with a total of 4 literals

			( in.l0 &  en.l0) ->  out.l0
			(         -en.l0) -> -out.l0
			(-in.l0         ) -> -out.l0
		*/
		_solver->CommitClause(-in.l0, -en.l0,  out.l0);
		_solver->CommitClause(         en.l0, -out.l0);
		_solver->CommitClause( in.l0,         -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUFIF
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  0 (X)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 0  0 (X)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |          out   |        en         |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*]
			neg: [**0*, *0**]
			cost: 3 terms with a total of 4 literals

			(          in.l1 &  en.l0         ) ->  out.l0
			(                  -en.l0         ) -> -out.l0
			(         -in.l1                  ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0101]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |          out   |        en         |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(          in.l1 &           en.l1) ->  out.l1
			(                           -en.l1) -> -out.l1
			(         -in.l1                  ) -> -out.l1
		*/
		_solver->CommitClause(        -in.l1, -en.l0,          out.l0);
		_solver->CommitClause(                 en.l0,         -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(        -in.l1,         -en.l1,  out.l1);
		_solver->CommitClause(                         en.l1, -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUFIF
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 1  1 (X)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  0 (U)
			  6  0  1 (0) 1  0 (1)  | 0  1 (0)
			  7  0  1 (0) 1  1 (X)  | 0  0 (U)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 0  0 (U)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) 1  1 (X)  | 0  0 (U)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 0  0 (U)
			 14  1  1 (X) 1  0 (1)  | 0  0 (U)
			 15  1  1 (X) 1  1 (X)  | 0  0 (U)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0110]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [01*0]
			neg: [***1, *0**, 1***]
			cost: 4 terms with a total of 6 literals

			(-in.l0 &  in.l1 &          -en.l1) ->  out.l0
			(                            en.l1) -> -out.l0
			(         -in.l1                  ) -> -out.l0
			( in.l0                           ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0101]
			initial neg: [0000, 1000, 1100, 0010, 1010, 0110, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 0  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [010*]
			neg: [**1*, *0**, 1***]
			cost: 4 terms with a total of 6 literals

			(-in.l0 &  in.l1 & -en.l0         ) ->  out.l1
			(                   en.l0         ) -> -out.l1
			(         -in.l1                  ) -> -out.l1
			( in.l0                           ) -> -out.l1
		*/
		_solver->CommitClause( in.l0, -in.l1,          en.l1,  out.l0);
		_solver->CommitClause(                        -en.l1, -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(-in.l0,                         -out.l0);
		_solver->CommitClause( in.l0, -in.l1,  en.l0,          out.l1);
		_solver->CommitClause(                -en.l0,         -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
		_solver->CommitClause(-in.l0,                         -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUFIF
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  0 (X)
			  6  0  F (0) F  0 (1)  | 0  F (0)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | 0  0 (X)
			 10  F  0 (1) F  0 (1)  | F  0 (1)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |          out   |        en         |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, *0**]
			cost: 2 terms with a total of 2 literals

			(                  -en.l0         ) -> -out.l0
			(         -in.l1                  ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |          out   |        en         |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [***0, *0**]
			cost: 2 terms with a total of 2 literals

			(                           -en.l1) -> -out.l1
			(         -in.l1                  ) -> -out.l1
		*/
		_solver->CommitClause(                 en.l0,         -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(                         en.l1, -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: BUFIF
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 1  1 (X)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | 0  0 (U)
			  6  F  1 (0) 1  F (1)  | F  1 (0)
			  7  F  1 (0) 1  1 (X)  | 0  0 (U)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | 0  0 (U)
			 10  1  F (1) 1  F (1)  | 1  F (1)
			 11  1  F (1) 1  1 (X)  | 0  0 (U)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 0  0 (U)
			 14  1  1 (X) 1  F (1)  | 0  0 (U)
			 15  1  1 (X) 1  1 (X)  | 0  0 (U)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0110]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 0  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [01*0]
			neg: [**11, *0**, 1***]
			cost: 4 terms with a total of 7 literals

			(-in.l0 &  in.l1 &          -en.l1) ->  out.l0
			(                   en.l0 &  en.l1) -> -out.l0
			(         -in.l1                  ) -> -out.l0
			( in.l0                           ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0101]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | F  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [010*]
			neg: [**11, *0**, 1***]
			cost: 4 terms with a total of 7 literals

			(-in.l0 &  in.l1 & -en.l0         ) ->  out.l1
			(                   en.l0 &  en.l1) -> -out.l1
			(         -in.l1                  ) -> -out.l1
			( in.l0                           ) -> -out.l1
		*/
		_solver->CommitClause( in.l0, -in.l1,          en.l1,  out.l0);
		_solver->CommitClause(                -en.l0, -en.l1, -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(-in.l0,                         -out.l0);
		_solver->CommitClause( in.l0, -in.l1,  en.l0,          out.l1);
		_solver->CommitClause(                -en.l0, -en.l1, -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
		_solver->CommitClause(-in.l0,                         -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeTristateInverter(const LogicContainer& in, const LogicContainer& en, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(en);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOTIF
			Input Encodings: 01, 01
			Output Encodings: 01

			    en    in     | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 1 (1)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [00, 10, 01]
			initial neg: [11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   en    |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 1  |
			 in ----+----+----+
			     l0 | 1  | 0  |
			--------+----+----+

			pos: [*0, 0*]
			neg: [11]
			cost: 3 terms with a total of 4 literals

			(         -en.l0) ->  out.l0
			(-in.l0         ) ->  out.l0
			( in.l0 &  en.l0) -> -out.l0
		*/
		_solver->CommitClause(         en.l0,  out.l0);
		_solver->CommitClause( in.l0,          out.l0);
		_solver->CommitClause(-in.l0, -en.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOTIF
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  0 (X)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  0 (X)
			  9  1  0 (1) 0  1 (0)  | 0  0 (X)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0101]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |          out   |        en         |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1]
			neg: [***0, *0**]
			cost: 3 terms with a total of 4 literals

			(          in.l1 &           en.l1) ->  out.l0
			(                           -en.l1) -> -out.l0
			(         -in.l1                  ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |          out   |        en         |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*]
			neg: [**0*, *0**]
			cost: 3 terms with a total of 4 literals

			(          in.l1 &  en.l0         ) ->  out.l1
			(                  -en.l0         ) -> -out.l1
			(         -in.l1                  ) -> -out.l1
		*/
		_solver->CommitClause(        -in.l1,         -en.l1,  out.l0);
		_solver->CommitClause(                         en.l1, -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(        -in.l1, -en.l0,          out.l1);
		_solver->CommitClause(                 en.l0,         -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOTIF
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 1  1 (X)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  0 (U)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 0  0 (U)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  0 (U)
			  9  1  0 (1) 0  1 (0)  | 0  0 (U)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) 1  1 (X)  | 0  0 (U)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) 0  1 (0)  | 0  0 (U)
			 14  1  1 (X) 1  0 (1)  | 0  0 (U)
			 15  1  1 (X) 1  1 (X)  | 0  0 (U)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0101]
			initial neg: [0000, 1000, 1100, 0010, 1010, 0110, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 0  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [010*]
			neg: [**1*, *0**, 1***]
			cost: 4 terms with a total of 6 literals

			(-in.l0 &  in.l1 & -en.l0         ) ->  out.l0
			(                   en.l0         ) -> -out.l0
			(         -in.l1                  ) -> -out.l0
			( in.l0                           ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0110]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        en         |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [01*0]
			neg: [***1, *0**, 1***]
			cost: 4 terms with a total of 6 literals

			(-in.l0 &  in.l1 &          -en.l1) ->  out.l1
			(                            en.l1) -> -out.l1
			(         -in.l1                  ) -> -out.l1
			( in.l0                           ) -> -out.l1
		*/
		_solver->CommitClause( in.l0, -in.l1,  en.l0,          out.l0);
		_solver->CommitClause(                -en.l0,         -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(-in.l0,                         -out.l0);
		_solver->CommitClause( in.l0, -in.l1,          en.l1,  out.l1);
		_solver->CommitClause(                        -en.l1, -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
		_solver->CommitClause(-in.l0,                         -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOTIF
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  0 (X)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  0 (X)
			  9  F  0 (1) 0  F (0)  | 0  0 (X)
			 10  F  0 (1) F  0 (1)  | 0  F (0)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |          out   |        en         |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 0  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [***0, *0**]
			cost: 2 terms with a total of 2 literals

			(                           -en.l1) -> -out.l0
			(         -in.l1                  ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |          out   |        en         |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, *0**]
			cost: 2 terms with a total of 2 literals

			(                  -en.l0         ) -> -out.l1
			(         -in.l1                  ) -> -out.l1
		*/
		_solver->CommitClause(                         en.l1, -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(                 en.l0,         -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: NOTIF
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     en       in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 1  1 (X)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | 0  0 (U)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 0  0 (U)
			                        |
			  8  1  F (1) 0  0 (U)  | 0  0 (U)
			  9  1  F (1) F  1 (0)  | 0  0 (U)
			 10  1  F (1) 1  F (1)  | F  1 (0)
			 11  1  F (1) 1  1 (X)  | 0  0 (U)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0  0 (U)
			 13  1  1 (X) F  1 (0)  | 0  0 (U)
			 14  1  1 (X) 1  F (1)  | 0  0 (U)
			 15  1  1 (X) 1  1 (X)  | 0  0 (U)


			Output out, Literal l0
			-------------------------------

			initial pos: [0100, 0101]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | F  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [010*]
			neg: [**11, *0**, 1***]
			cost: 4 terms with a total of 7 literals

			(-in.l0 &  in.l1 & -en.l0         ) ->  out.l0
			(                   en.l0 &  en.l1) -> -out.l0
			(         -in.l1                  ) -> -out.l0
			( in.l0                           ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0100, 0110]
			initial neg: [0000, 1000, 1100, 0010, 1010, 1110, 0001, 1001, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        en         |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 1  | 1  | 0  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [01*0]
			neg: [**11, *0**, 1***]
			cost: 4 terms with a total of 7 literals

			(-in.l0 &  in.l1 &          -en.l1) ->  out.l1
			(                   en.l0 &  en.l1) -> -out.l1
			(         -in.l1                  ) -> -out.l1
			( in.l0                           ) -> -out.l1
		*/
		_solver->CommitClause( in.l0, -in.l1,  en.l0,          out.l0);
		_solver->CommitClause(                -en.l0, -en.l1, -out.l0);
		_solver->CommitClause(         in.l1,                 -out.l0);
		_solver->CommitClause(-in.l0,                         -out.l0);
		_solver->CommitClause( in.l0, -in.l1,          en.l1,  out.l1);
		_solver->CommitClause(                -en.l0, -en.l1, -out.l1);
		_solver->CommitClause(         in.l1,                 -out.l1);
		_solver->CommitClause(-in.l0,                         -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeMultiplexer(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& sel, const LogicContainer& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(sel);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: MUX
			Input Encodings: 01, 01, 01
			Output Encodings: 01

			    sel   in2    in1    | out
			    l0    l0     l0     | l0
			------------------------+--------
			 0  0 (0) 0 (0)  0 (0)  | 0 (0)
			 1  0 (0) 0 (0)  1 (1)  | 1 (1)
			 2  0 (0) 1 (1)  0 (0)  | 0 (0)
			 3  0 (0) 1 (1)  1 (1)  | 1 (1)
			                        |
			 4  1 (1) 0 (0)  0 (0)  | 0 (0)
			 5  1 (1) 0 (0)  1 (1)  | 0 (0)
			 6  1 (1) 1 (1)  0 (0)  | 1 (1)
			 7  1 (1) 1 (1)  1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [100, 110, 011, 111]
			initial neg: [000, 010, 001, 101]
			initial nc:  []
			initial cost: 8 terms with a total of 24 literals

			  out   |   sel   |
			  l0    |    | l0 |
			--------+----+----+--------
			        | 0  | 0  |
			        +----+----+----
			        | 1  | 0  |
			in2 ----+----+----+ l0 in1
			        | 1  | 1  |
			     l0 +----+----+----
			        | 0  | 1  |
			--------+----+----+--------

			pos: [*11, 1*0]
			neg: [*01, 0*0]
			cost: 4 terms with a total of 8 literals

			(           in2.l0 &  sel.l0) ->  out.l0
			( in1.l0 &           -sel.l0) ->  out.l0
			(          -in2.l0 &  sel.l0) -> -out.l0
			(-in1.l0 &           -sel.l0) -> -out.l0
		*/
		_solver->CommitClause(         -in2.l0, -sel.l0,  out.l0);
		_solver->CommitClause(-in1.l0,           sel.l0,  out.l0);
		_solver->CommitClause(          in2.l0, -sel.l0, -out.l0);
		_solver->CommitClause( in1.l0,           sel.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: MUX
			Input Encodings: 01X, 01X, 01X
			Output Encodings: 01X

			     sel      in2       in1       | out
			     l1 l0    l1 l0     l1 l0     | l1 l0
			----------------------------------+-----------
			  0  0  0 (X) 0  0 (X)  0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  0 (X)  0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 0  0 (X)  1  0 (1)  | 0  0 (X)
			  3  0  0 (X) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			  4  0  0 (X) 0  1 (0)  0  0 (X)  | 0  0 (X)
			  5  0  0 (X) 0  1 (0)  0  1 (0)  | 0  0 (X)
			  6  0  0 (X) 0  1 (0)  1  0 (1)  | 0  0 (X)
			  7  0  0 (X) 0  1 (0)  *  * (*)  | *  * (*)
			                                  |
			  8  0  0 (X) 1  0 (1)  0  0 (X)  | 0  0 (X)
			  9  0  0 (X) 1  0 (1)  0  1 (0)  | 0  0 (X)
			 10  0  0 (X) 1  0 (1)  1  0 (1)  | 0  0 (X)
			 11  0  0 (X) 1  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 12  0  0 (X) *  * (*)  0  0 (X)  | *  * (*)
			 13  0  0 (X) *  * (*)  0  1 (0)  | *  * (*)
			 14  0  0 (X) *  * (*)  1  0 (1)  | *  * (*)
			 15  0  0 (X) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 16  0  1 (0) 0  0 (X)  0  0 (X)  | 0  0 (X)
			 17  0  1 (0) 0  0 (X)  0  1 (0)  | 0  1 (0)
			 18  0  1 (0) 0  0 (X)  1  0 (1)  | 1  0 (1)
			 19  0  1 (0) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 20  0  1 (0) 0  1 (0)  0  0 (X)  | 0  0 (X)
			 21  0  1 (0) 0  1 (0)  0  1 (0)  | 0  1 (0)
			 22  0  1 (0) 0  1 (0)  1  0 (1)  | 1  0 (1)
			 23  0  1 (0) 0  1 (0)  *  * (*)  | *  * (*)
			                                  |
			 24  0  1 (0) 1  0 (1)  0  0 (X)  | 0  0 (X)
			 25  0  1 (0) 1  0 (1)  0  1 (0)  | 0  1 (0)
			 26  0  1 (0) 1  0 (1)  1  0 (1)  | 1  0 (1)
			 27  0  1 (0) 1  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 28  0  1 (0) *  * (*)  0  0 (X)  | *  * (*)
			 29  0  1 (0) *  * (*)  0  1 (0)  | *  * (*)
			 30  0  1 (0) *  * (*)  1  0 (1)  | *  * (*)
			 31  0  1 (0) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 32  1  0 (1) 0  0 (X)  0  0 (X)  | 0  0 (X)
			 33  1  0 (1) 0  0 (X)  0  1 (0)  | 0  0 (X)
			 34  1  0 (1) 0  0 (X)  1  0 (1)  | 0  0 (X)
			 35  1  0 (1) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 36  1  0 (1) 0  1 (0)  0  0 (X)  | 0  1 (0)
			 37  1  0 (1) 0  1 (0)  0  1 (0)  | 0  1 (0)
			 38  1  0 (1) 0  1 (0)  1  0 (1)  | 0  1 (0)
			 39  1  0 (1) 0  1 (0)  *  * (*)  | *  * (*)
			                                  |
			 40  1  0 (1) 1  0 (1)  0  0 (X)  | 1  0 (1)
			 41  1  0 (1) 1  0 (1)  0  1 (0)  | 1  0 (1)
			 42  1  0 (1) 1  0 (1)  1  0 (1)  | 1  0 (1)
			 43  1  0 (1) 1  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 44  1  0 (1) *  * (*)  0  0 (X)  | *  * (*)
			 45  1  0 (1) *  * (*)  0  1 (0)  | *  * (*)
			 46  1  0 (1) *  * (*)  1  0 (1)  | *  * (*)
			 47  1  0 (1) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 48  *  * (*) 0  0 (X)  0  0 (X)  | *  * (*)
			 49  *  * (*) 0  0 (X)  0  1 (0)  | *  * (*)
			 50  *  * (*) 0  0 (X)  1  0 (1)  | *  * (*)
			 51  *  * (*) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 52  *  * (*) 0  1 (0)  0  0 (X)  | *  * (*)
			 53  *  * (*) 0  1 (0)  0  1 (0)  | *  * (*)
			 54  *  * (*) 0  1 (0)  1  0 (1)  | *  * (*)
			 55  *  * (*) 0  1 (0)  *  * (*)  | *  * (*)
			                                  |
			 56  *  * (*) 1  0 (1)  0  0 (X)  | *  * (*)
			 57  *  * (*) 1  0 (1)  0  1 (0)  | *  * (*)
			 58  *  * (*) 1  0 (1)  1  0 (1)  | *  * (*)
			 59  *  * (*) 1  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 60  *  * (*) *  * (*)  0  0 (X)  | *  * (*)
			 61  *  * (*) *  * (*)  0  1 (0)  | *  * (*)
			 62  *  * (*) *  * (*)  1  0 (1)  | *  * (*)
			 63  *  * (*) *  * (*)  *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [100010, 101010, 100110, 001001, 101001, 011001]
			initial neg: [000000, 100000, 010000, 001000, 101000, 011000, 000100, 100100, 010100, 000010, 010010, 001010, 011010, 000110, 010110, 000001, 100001, 010001, 000101, 100101, 010101]
			initial nc:  [110000, 111000, 110100, 001100, 101100, 011100, 111100, 110010, 111010, 110110, 001110, 101110, 011110, 111110, 110001, 111001, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial cost: 64 terms with a total of 384 literals

			                |                  sel                  |                            |                  sel                  |
			      out       |                   |        l1         |                  out       |                   |        l1         |
			       l0       |                   |                   |                   l0       |                   |                   |
			                |        in2        |        in2                                     |        in2        |        in2
			                |         |   l1    |   l1    |         |                            |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | 0  | *  | 1  | 0  |                            | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 0  | *  | 1  | 0  |                            | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1            in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 0  | *  | 1  | 0  |                            | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------    sel ------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | *  | 1  | 1  | *  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1     l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 1  | 1  | *  | 1  | *  | *  | *  | *  |                            | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | *  | 1  | 1  | *  |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |                            |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |                            |        in2        |        in2        |

			pos: [**1**1, 1***1*]
			neg: [****00, **0*0*, 0****0]
			cost: 5 terms with a total of 10 literals

			(                     in2.l0 &                      sel.l1) ->  out.l0
			( in1.l0 &                                sel.l0          ) ->  out.l0
			(                                        -sel.l0 & -sel.l1) -> -out.l0
			(                    -in2.l0 &           -sel.l0          ) -> -out.l0
			(-in1.l0 &                                         -sel.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [010010, 011010, 010110, 000101, 100101, 010101]
			initial neg: [000000, 100000, 010000, 001000, 101000, 011000, 000100, 100100, 010100, 000010, 100010, 001010, 101010, 000110, 100110, 000001, 100001, 010001, 001001, 101001, 011001]
			initial nc:  [110000, 111000, 110100, 001100, 101100, 011100, 111100, 110010, 111010, 110110, 001110, 101110, 011110, 111110, 110001, 111001, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial cost: 64 terms with a total of 384 literals

			                |                  sel                  |                            |                  sel                  |
			      out       |                   |        l1         |                  out       |                   |        l1         |
			       l1       |                   |                   |                   l1       |                   |                   |
			                |        in2        |        in2                                     |        in2        |        in2
			                |         |   l1    |   l1    |         |                            |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | 1  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 1  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1            in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 1  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------    sel ------------+----+----+----+----+----+----+----+----+--------
			                | 1  | 1  | *  | 1  | *  | *  | *  | *  |                            | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1     l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 1  | 1  | *  | *  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 1  | 1  | *  | *  |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |                            |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |                            |        in2        |        in2        |

			pos: [***1*1, *1**1*]
			neg: [****00, ***00*, *0***0]
			cost: 5 terms with a total of 10 literals

			(                               in2.l1 &            sel.l1) ->  out.l1
			(           in1.l1 &                      sel.l0          ) ->  out.l1
			(                                        -sel.l0 & -sel.l1) -> -out.l1
			(                              -in2.l1 & -sel.l0          ) -> -out.l1
			(          -in1.l1 &                               -sel.l1) -> -out.l1
		*/
		_solver->CommitClause(                  -in2.l0,                   -sel.l1,  out.l0);
		_solver->CommitClause(-in1.l0,                            -sel.l0,           out.l0);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l0);
		_solver->CommitClause(                   in2.l0,           sel.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                                      sel.l1, -out.l0);
		_solver->CommitClause(                           -in2.l1,          -sel.l1,  out.l1);
		_solver->CommitClause(         -in1.l1,                   -sel.l0,           out.l1);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l1);
		_solver->CommitClause(                            in2.l1,  sel.l0,          -out.l1);
		_solver->CommitClause(          in1.l1,                             sel.l1, -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: MUX
			Input Encodings: U01X, U01X, U01X
			Output Encodings: U01X

			     sel      in2       in1       | out
			     l1 l0    l1 l0     l1 l0     | l1 l0
			----------------------------------+-----------
			  0  0  0 (U) 0  0 (U)  0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  0 (U)  0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 0  0 (U)  1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 0  0 (U)  1  1 (X)  | 0  0 (U)
			                                  |
			  4  0  0 (U) 0  1 (0)  0  0 (U)  | 0  0 (U)
			  5  0  0 (U) 0  1 (0)  0  1 (0)  | 0  0 (U)
			  6  0  0 (U) 0  1 (0)  1  0 (1)  | 0  0 (U)
			  7  0  0 (U) 0  1 (0)  1  1 (X)  | 0  0 (U)
			                                  |
			  8  0  0 (U) 1  0 (1)  0  0 (U)  | 0  0 (U)
			  9  0  0 (U) 1  0 (1)  0  1 (0)  | 0  0 (U)
			 10  0  0 (U) 1  0 (1)  1  0 (1)  | 0  0 (U)
			 11  0  0 (U) 1  0 (1)  1  1 (X)  | 0  0 (U)
			                                  |
			 12  0  0 (U) 1  1 (X)  0  0 (U)  | 0  0 (U)
			 13  0  0 (U) 1  1 (X)  0  1 (0)  | 0  0 (U)
			 14  0  0 (U) 1  1 (X)  1  0 (1)  | 0  0 (U)
			 15  0  0 (U) 1  1 (X)  1  1 (X)  | 0  0 (U)
			                                  |
			 16  0  1 (0) 0  0 (U)  0  0 (U)  | 0  0 (U)
			 17  0  1 (0) 0  0 (U)  0  1 (0)  | 0  1 (0)
			 18  0  1 (0) 0  0 (U)  1  0 (1)  | 1  0 (1)
			 19  0  1 (0) 0  0 (U)  1  1 (X)  | 1  1 (X)
			                                  |
			 20  0  1 (0) 0  1 (0)  0  0 (U)  | 0  0 (U)
			 21  0  1 (0) 0  1 (0)  0  1 (0)  | 0  1 (0)
			 22  0  1 (0) 0  1 (0)  1  0 (1)  | 1  0 (1)
			 23  0  1 (0) 0  1 (0)  1  1 (X)  | 1  1 (X)
			                                  |
			 24  0  1 (0) 1  0 (1)  0  0 (U)  | 0  0 (U)
			 25  0  1 (0) 1  0 (1)  0  1 (0)  | 0  1 (0)
			 26  0  1 (0) 1  0 (1)  1  0 (1)  | 1  0 (1)
			 27  0  1 (0) 1  0 (1)  1  1 (X)  | 1  1 (X)
			                                  |
			 28  0  1 (0) 1  1 (X)  0  0 (U)  | 0  0 (U)
			 29  0  1 (0) 1  1 (X)  0  1 (0)  | 0  1 (0)
			 30  0  1 (0) 1  1 (X)  1  0 (1)  | 1  0 (1)
			 31  0  1 (0) 1  1 (X)  1  1 (X)  | 1  1 (X)
			                                  |
			 32  1  0 (1) 0  0 (U)  0  0 (U)  | 0  0 (U)
			 33  1  0 (1) 0  0 (U)  0  1 (0)  | 0  0 (U)
			 34  1  0 (1) 0  0 (U)  1  0 (1)  | 0  0 (U)
			 35  1  0 (1) 0  0 (U)  1  1 (X)  | 0  0 (U)
			                                  |
			 36  1  0 (1) 0  1 (0)  0  0 (U)  | 0  1 (0)
			 37  1  0 (1) 0  1 (0)  0  1 (0)  | 0  1 (0)
			 38  1  0 (1) 0  1 (0)  1  0 (1)  | 0  1 (0)
			 39  1  0 (1) 0  1 (0)  1  1 (X)  | 0  1 (0)
			                                  |
			 40  1  0 (1) 1  0 (1)  0  0 (U)  | 1  0 (1)
			 41  1  0 (1) 1  0 (1)  0  1 (0)  | 1  0 (1)
			 42  1  0 (1) 1  0 (1)  1  0 (1)  | 1  0 (1)
			 43  1  0 (1) 1  0 (1)  1  1 (X)  | 1  0 (1)
			                                  |
			 44  1  0 (1) 1  1 (X)  0  0 (U)  | 1  1 (X)
			 45  1  0 (1) 1  1 (X)  0  1 (0)  | 1  1 (X)
			 46  1  0 (1) 1  1 (X)  1  0 (1)  | 1  1 (X)
			 47  1  0 (1) 1  1 (X)  1  1 (X)  | 1  1 (X)
			                                  |
			 48  1  1 (X) 0  0 (U)  0  0 (U)  | 1  1 (X)
			 49  1  1 (X) 0  0 (U)  0  1 (0)  | 1  1 (X)
			 50  1  1 (X) 0  0 (U)  1  0 (1)  | 1  1 (X)
			 51  1  1 (X) 0  0 (U)  1  1 (X)  | 1  1 (X)
			                                  |
			 52  1  1 (X) 0  1 (0)  0  0 (U)  | 1  1 (X)
			 53  1  1 (X) 0  1 (0)  0  1 (0)  | 1  1 (X)
			 54  1  1 (X) 0  1 (0)  1  0 (1)  | 1  1 (X)
			 55  1  1 (X) 0  1 (0)  1  1 (X)  | 1  1 (X)
			                                  |
			 56  1  1 (X) 1  0 (1)  0  0 (U)  | 1  1 (X)
			 57  1  1 (X) 1  0 (1)  0  1 (0)  | 1  1 (X)
			 58  1  1 (X) 1  0 (1)  1  0 (1)  | 1  1 (X)
			 59  1  1 (X) 1  0 (1)  1  1 (X)  | 1  1 (X)
			                                  |
			 60  1  1 (X) 1  1 (X)  0  0 (U)  | 1  1 (X)
			 61  1  1 (X) 1  1 (X)  0  1 (0)  | 1  1 (X)
			 62  1  1 (X) 1  1 (X)  1  0 (1)  | 1  1 (X)
			 63  1  1 (X) 1  1 (X)  1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [100010, 110010, 101010, 111010, 100110, 110110, 101110, 111110, 001001, 101001, 011001, 111001, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial neg: [000000, 100000, 010000, 110000, 001000, 101000, 011000, 111000, 000100, 100100, 010100, 110100, 001100, 101100, 011100, 111100, 000010, 010010, 001010, 011010, 000110, 010110, 001110, 011110, 000001, 100001, 010001, 110001, 000101, 100101, 010101, 110101]
			initial nc:  []
			initial cost: 64 terms with a total of 384 literals

			                |                  sel                  |
			      out       |                   |        l1         |
			       l0       |                   |                   |
			                |        in2        |        in2
			                |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 0  | 1  | 1  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |

			pos: [****11, **1**1, 1***1*]
			neg: [****00, **0*0*, 0****0]
			cost: 6 terms with a total of 12 literals

			(                                         sel.l0 &  sel.l1) ->  out.l0
			(                     in2.l0 &                      sel.l1) ->  out.l0
			( in1.l0 &                                sel.l0          ) ->  out.l0
			(                                        -sel.l0 & -sel.l1) -> -out.l0
			(                    -in2.l0 &           -sel.l0          ) -> -out.l0
			(-in1.l0 &                                         -sel.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [010010, 110010, 011010, 111010, 010110, 110110, 011110, 111110, 000101, 100101, 010101, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial neg: [000000, 100000, 010000, 110000, 001000, 101000, 011000, 111000, 000100, 100100, 010100, 110100, 001100, 101100, 011100, 111100, 000010, 100010, 001010, 101010, 000110, 100110, 001110, 101110, 000001, 100001, 010001, 110001, 001001, 101001, 011001, 111001]
			initial nc:  []
			initial cost: 64 terms with a total of 384 literals

			                |                  sel                  |
			      out       |                   |        l1         |
			       l1       |                   |                   |
			                |        in2        |        in2
			                |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |

			pos: [****11, ***1*1, *1**1*]
			neg: [****00, ***00*, *0***0]
			cost: 6 terms with a total of 12 literals

			(                                         sel.l0 &  sel.l1) ->  out.l1
			(                               in2.l1 &            sel.l1) ->  out.l1
			(           in1.l1 &                      sel.l0          ) ->  out.l1
			(                                        -sel.l0 & -sel.l1) -> -out.l1
			(                              -in2.l1 & -sel.l0          ) -> -out.l1
			(          -in1.l1 &                               -sel.l1) -> -out.l1
		*/
		_solver->CommitClause(                                    -sel.l0, -sel.l1,  out.l0);
		_solver->CommitClause(                  -in2.l0,                   -sel.l1,  out.l0);
		_solver->CommitClause(-in1.l0,                            -sel.l0,           out.l0);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l0);
		_solver->CommitClause(                   in2.l0,           sel.l0,          -out.l0);
		_solver->CommitClause( in1.l0,                                      sel.l1, -out.l0);
		_solver->CommitClause(                                    -sel.l0, -sel.l1,  out.l1);
		_solver->CommitClause(                           -in2.l1,          -sel.l1,  out.l1);
		_solver->CommitClause(         -in1.l1,                   -sel.l0,           out.l1);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l1);
		_solver->CommitClause(                            in2.l1,  sel.l0,          -out.l1);
		_solver->CommitClause(          in1.l1,                             sel.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: MUX
			Input Encodings: 01F, 01F, 01F
			Output Encodings: 01F

			     sel      in2       in1       | out
			     l1 l0    l1 l0     l1 l0     | l1 l0
			----------------------------------+-----------
			  0  0  0 (X) 0  0 (X)  0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  0 (X)  0  F (0)  | 0  0 (X)
			  2  0  0 (X) 0  0 (X)  F  0 (1)  | 0  0 (X)
			  3  0  0 (X) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			  4  0  0 (X) 0  F (0)  0  0 (X)  | 0  0 (X)
			  5  0  0 (X) 0  F (0)  0  F (0)  | 0  0 (X)
			  6  0  0 (X) 0  F (0)  F  0 (1)  | 0  0 (X)
			  7  0  0 (X) 0  F (0)  *  * (*)  | *  * (*)
			                                  |
			  8  0  0 (X) F  0 (1)  0  0 (X)  | 0  0 (X)
			  9  0  0 (X) F  0 (1)  0  F (0)  | 0  0 (X)
			 10  0  0 (X) F  0 (1)  F  0 (1)  | 0  0 (X)
			 11  0  0 (X) F  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 12  0  0 (X) *  * (*)  0  0 (X)  | *  * (*)
			 13  0  0 (X) *  * (*)  0  F (0)  | *  * (*)
			 14  0  0 (X) *  * (*)  F  0 (1)  | *  * (*)
			 15  0  0 (X) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 16  0  F (0) 0  0 (X)  0  0 (X)  | 0  0 (X)
			 17  0  F (0) 0  0 (X)  0  F (0)  | 0  F (0)
			 18  0  F (0) 0  0 (X)  F  0 (1)  | F  0 (1)
			 19  0  F (0) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 20  0  F (0) 0  F (0)  0  0 (X)  | 0  0 (X)
			 21  0  F (0) 0  F (0)  0  F (0)  | 0  F (0)
			 22  0  F (0) 0  F (0)  F  0 (1)  | F  0 (1)
			 23  0  F (0) 0  F (0)  *  * (*)  | *  * (*)
			                                  |
			 24  0  F (0) F  0 (1)  0  0 (X)  | 0  0 (X)
			 25  0  F (0) F  0 (1)  0  F (0)  | 0  F (0)
			 26  0  F (0) F  0 (1)  F  0 (1)  | F  0 (1)
			 27  0  F (0) F  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 28  0  F (0) *  * (*)  0  0 (X)  | *  * (*)
			 29  0  F (0) *  * (*)  0  F (0)  | *  * (*)
			 30  0  F (0) *  * (*)  F  0 (1)  | *  * (*)
			 31  0  F (0) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 32  F  0 (1) 0  0 (X)  0  0 (X)  | 0  0 (X)
			 33  F  0 (1) 0  0 (X)  0  F (0)  | 0  0 (X)
			 34  F  0 (1) 0  0 (X)  F  0 (1)  | 0  0 (X)
			 35  F  0 (1) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 36  F  0 (1) 0  F (0)  0  0 (X)  | 0  F (0)
			 37  F  0 (1) 0  F (0)  0  F (0)  | 0  F (0)
			 38  F  0 (1) 0  F (0)  F  0 (1)  | 0  F (0)
			 39  F  0 (1) 0  F (0)  *  * (*)  | *  * (*)
			                                  |
			 40  F  0 (1) F  0 (1)  0  0 (X)  | F  0 (1)
			 41  F  0 (1) F  0 (1)  0  F (0)  | F  0 (1)
			 42  F  0 (1) F  0 (1)  F  0 (1)  | F  0 (1)
			 43  F  0 (1) F  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 44  F  0 (1) *  * (*)  0  0 (X)  | *  * (*)
			 45  F  0 (1) *  * (*)  0  F (0)  | *  * (*)
			 46  F  0 (1) *  * (*)  F  0 (1)  | *  * (*)
			 47  F  0 (1) *  * (*)  *  * (*)  | *  * (*)
			                                  |
			 48  *  * (*) 0  0 (X)  0  0 (X)  | *  * (*)
			 49  *  * (*) 0  0 (X)  0  F (0)  | *  * (*)
			 50  *  * (*) 0  0 (X)  F  0 (1)  | *  * (*)
			 51  *  * (*) 0  0 (X)  *  * (*)  | *  * (*)
			                                  |
			 52  *  * (*) 0  F (0)  0  0 (X)  | *  * (*)
			 53  *  * (*) 0  F (0)  0  F (0)  | *  * (*)
			 54  *  * (*) 0  F (0)  F  0 (1)  | *  * (*)
			 55  *  * (*) 0  F (0)  *  * (*)  | *  * (*)
			                                  |
			 56  *  * (*) F  0 (1)  0  0 (X)  | *  * (*)
			 57  *  * (*) F  0 (1)  0  F (0)  | *  * (*)
			 58  *  * (*) F  0 (1)  F  0 (1)  | *  * (*)
			 59  *  * (*) F  0 (1)  *  * (*)  | *  * (*)
			                                  |
			 60  *  * (*) *  * (*)  0  0 (X)  | *  * (*)
			 61  *  * (*) *  * (*)  0  F (0)  | *  * (*)
			 62  *  * (*) *  * (*)  F  0 (1)  | *  * (*)
			 63  *  * (*) *  * (*)  *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [000000, 100000, 010000, 001000, 101000, 011000, 000100, 100100, 010100, 000010, 010010, 001010, 011010, 000110, 010110, 000001, 100001, 010001, 000101, 100101, 010101]
			initial nc:  [110000, 111000, 110100, 001100, 101100, 011100, 111100, 110010, 111010, 110110, 001110, 101110, 011110, 111110, 110001, 111001, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial cost: 58 terms with a total of 348 literals

			                |                  sel                  |                            |                  sel                  |
			      out       |                   |        l1         |                  out       |                   |        l1         |
			       l0       |                   |                   |                   l0       |                   |                   |
			                |        in2        |        in2                                     |        in2        |        in2
			                |         |   l1    |   l1    |         |                            |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | 0  | *  | F  | 0  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 0  | *  | F  | 0  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1            in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | 0  | *  | F  | 0  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------    sel ------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | *  | *  | *  | *  | 0  | *  | *  | 0  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1     l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | F  | F  | *  | F  | *  | *  | *  | *  |                            | *  | *  | *  | *  | 0  | *  | *  | 0  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | 0  | *  | *  | 0  |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |                            |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |                            |        in2        |        in2        |

			pos: []
			neg: [****00, **0**1, 0****0]
			cost: 3 terms with a total of 6 literals

			(                                        -sel.l0 & -sel.l1) -> -out.l0
			(                    -in2.l0 &                      sel.l1) -> -out.l0
			(-in1.l0 &                                         -sel.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [000000, 100000, 010000, 001000, 101000, 011000, 000100, 100100, 010100, 000010, 100010, 001010, 101010, 000110, 100110, 000001, 100001, 010001, 001001, 101001, 011001]
			initial nc:  [110000, 111000, 110100, 001100, 101100, 011100, 111100, 110010, 111010, 110110, 001110, 101110, 011110, 111110, 110001, 111001, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial cost: 58 terms with a total of 348 literals

			                |                  sel                  |                            |                  sel                  |
			      out       |                   |        l1         |                  out       |                   |        l1         |
			       l1       |                   |                   |                   l1       |                   |                   |
			                |        in2        |        in2                                     |        in2        |        in2
			                |         |   l1    |   l1    |         |                            |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | *  | 0  | F  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | *  | *  | 0  | 0  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | F  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | *  | *  | 0  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1            in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | *  | *  | 0  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | F  | *  | 0  | 0  |                            | 0  | 0  | 0  | 0  | *  | *  | 0  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------    sel ------------+----+----+----+----+----+----+----+----+--------
			                | F  | F  | *  | F  | *  | *  | *  | *  |                            | *  | *  | *  | *  | *  | *  | *  | *  |
			             l1 +----+----+----+----+----+----+----+----+----                     l1 +----+----+----+----+----+----+----+----+----
			                | *  | *  | *  | *  | *  | *  | *  | *  |                            | *  | *  | *  | *  | *  | *  | *  | *  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1     l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | *  | *  | *  | *  |
			                +----+----+----+----+----+----+----+----+----                        +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | *  | 0  | *  | *  | *  | *  |                            | 0  | 0  | 0  | 0  | *  | *  | *  | *  |
			----------------+----+----+----+----+----+----+----+----+--------    ----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |                            |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |                            |        in2        |        in2        |

			pos: []
			neg: [****00, ***00*, *0***0]
			cost: 3 terms with a total of 6 literals

			(                                        -sel.l0 & -sel.l1) -> -out.l1
			(                              -in2.l1 & -sel.l0          ) -> -out.l1
			(          -in1.l1 &                               -sel.l1) -> -out.l1
		*/
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l0);
		_solver->CommitClause(                   in2.l0,                   -sel.l1, -out.l0);
		_solver->CommitClause( in1.l0,                                      sel.l1, -out.l0);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l1);
		_solver->CommitClause(                            in2.l1,  sel.l0,          -out.l1);
		_solver->CommitClause(          in1.l1,                             sel.l1, -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: MUX
			Input Encodings: U01F, U01F, U01F
			Output Encodings: U01F

			     sel      in2       in1       | out
			     l1 l0    l1 l0     l1 l0     | l1 l0
			----------------------------------+-----------
			  0  0  0 (U) 0  0 (U)  0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  0 (U)  F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 0  0 (U)  1  F (1)  | 0  0 (U)
			  3  0  0 (U) 0  0 (U)  1  1 (X)  | 0  0 (U)
			                                  |
			  4  0  0 (U) F  1 (0)  0  0 (U)  | 0  0 (U)
			  5  0  0 (U) F  1 (0)  F  1 (0)  | 0  0 (U)
			  6  0  0 (U) F  1 (0)  1  F (1)  | 0  0 (U)
			  7  0  0 (U) F  1 (0)  1  1 (X)  | 0  0 (U)
			                                  |
			  8  0  0 (U) 1  F (1)  0  0 (U)  | 0  0 (U)
			  9  0  0 (U) 1  F (1)  F  1 (0)  | 0  0 (U)
			 10  0  0 (U) 1  F (1)  1  F (1)  | 0  0 (U)
			 11  0  0 (U) 1  F (1)  1  1 (X)  | 0  0 (U)
			                                  |
			 12  0  0 (U) 1  1 (X)  0  0 (U)  | 0  0 (U)
			 13  0  0 (U) 1  1 (X)  F  1 (0)  | 0  0 (U)
			 14  0  0 (U) 1  1 (X)  1  F (1)  | 0  0 (U)
			 15  0  0 (U) 1  1 (X)  1  1 (X)  | 0  0 (U)
			                                  |
			 16  F  1 (0) 0  0 (U)  0  0 (U)  | 0  0 (U)
			 17  F  1 (0) 0  0 (U)  F  1 (0)  | F  1 (0)
			 18  F  1 (0) 0  0 (U)  1  F (1)  | 1  F (1)
			 19  F  1 (0) 0  0 (U)  1  1 (X)  | 1  1 (X)
			                                  |
			 20  F  1 (0) F  1 (0)  0  0 (U)  | 0  0 (U)
			 21  F  1 (0) F  1 (0)  F  1 (0)  | F  1 (0)
			 22  F  1 (0) F  1 (0)  1  F (1)  | 1  F (1)
			 23  F  1 (0) F  1 (0)  1  1 (X)  | 1  1 (X)
			                                  |
			 24  F  1 (0) 1  F (1)  0  0 (U)  | 0  0 (U)
			 25  F  1 (0) 1  F (1)  F  1 (0)  | F  1 (0)
			 26  F  1 (0) 1  F (1)  1  F (1)  | 1  F (1)
			 27  F  1 (0) 1  F (1)  1  1 (X)  | 1  1 (X)
			                                  |
			 28  F  1 (0) 1  1 (X)  0  0 (U)  | 0  0 (U)
			 29  F  1 (0) 1  1 (X)  F  1 (0)  | F  1 (0)
			 30  F  1 (0) 1  1 (X)  1  F (1)  | 1  F (1)
			 31  F  1 (0) 1  1 (X)  1  1 (X)  | 1  1 (X)
			                                  |
			 32  1  F (1) 0  0 (U)  0  0 (U)  | 0  0 (U)
			 33  1  F (1) 0  0 (U)  F  1 (0)  | 0  0 (U)
			 34  1  F (1) 0  0 (U)  1  F (1)  | 0  0 (U)
			 35  1  F (1) 0  0 (U)  1  1 (X)  | 0  0 (U)
			                                  |
			 36  1  F (1) F  1 (0)  0  0 (U)  | F  1 (0)
			 37  1  F (1) F  1 (0)  F  1 (0)  | F  1 (0)
			 38  1  F (1) F  1 (0)  1  F (1)  | F  1 (0)
			 39  1  F (1) F  1 (0)  1  1 (X)  | F  1 (0)
			                                  |
			 40  1  F (1) 1  F (1)  0  0 (U)  | 1  F (1)
			 41  1  F (1) 1  F (1)  F  1 (0)  | 1  F (1)
			 42  1  F (1) 1  F (1)  1  F (1)  | 1  F (1)
			 43  1  F (1) 1  F (1)  1  1 (X)  | 1  F (1)
			                                  |
			 44  1  F (1) 1  1 (X)  0  0 (U)  | 1  1 (X)
			 45  1  F (1) 1  1 (X)  F  1 (0)  | 1  1 (X)
			 46  1  F (1) 1  1 (X)  1  F (1)  | 1  1 (X)
			 47  1  F (1) 1  1 (X)  1  1 (X)  | 1  1 (X)
			                                  |
			 48  1  1 (X) 0  0 (U)  0  0 (U)  | 1  1 (X)
			 49  1  1 (X) 0  0 (U)  F  1 (0)  | 1  1 (X)
			 50  1  1 (X) 0  0 (U)  1  F (1)  | 1  1 (X)
			 51  1  1 (X) 0  0 (U)  1  1 (X)  | 1  1 (X)
			                                  |
			 52  1  1 (X) F  1 (0)  0  0 (U)  | 1  1 (X)
			 53  1  1 (X) F  1 (0)  F  1 (0)  | 1  1 (X)
			 54  1  1 (X) F  1 (0)  1  F (1)  | 1  1 (X)
			 55  1  1 (X) F  1 (0)  1  1 (X)  | 1  1 (X)
			                                  |
			 56  1  1 (X) 1  F (1)  0  0 (U)  | 1  1 (X)
			 57  1  1 (X) 1  F (1)  F  1 (0)  | 1  1 (X)
			 58  1  1 (X) 1  F (1)  1  F (1)  | 1  1 (X)
			 59  1  1 (X) 1  F (1)  1  1 (X)  | 1  1 (X)
			                                  |
			 60  1  1 (X) 1  1 (X)  0  0 (U)  | 1  1 (X)
			 61  1  1 (X) 1  1 (X)  F  1 (0)  | 1  1 (X)
			 62  1  1 (X) 1  1 (X)  1  F (1)  | 1  1 (X)
			 63  1  1 (X) 1  1 (X)  1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [100010, 110010, 101010, 111010, 100110, 110110, 101110, 111110, 001001, 101001, 011001, 111001, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial neg: [000000, 100000, 010000, 110000, 001000, 101000, 011000, 111000, 000100, 100100, 010100, 110100, 001100, 101100, 011100, 111100, 000010, 001010, 000110, 001110, 000001, 100001, 010001, 110001]
			initial nc:  []
			initial cost: 56 terms with a total of 336 literals

			                |                  sel                  |
			      out       |                   |        l1         |
			       l0       |                   |                   |
			                |        in2        |        in2
			                |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | 0  | 0  | F  | 1  | 1  | 0  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | F  | 1  | 1  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | 0  | 0  | F  | 1  | 1  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | F  | 1  | 1  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------
			                | F  | F  | F  | F  | 1  | 1  | 1  | 1  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |

			pos: [****11, **1**1, 1***1*]
			neg: [****00, **000*, 00***0]
			cost: 6 terms with a total of 14 literals

			(                                         sel.l0 &  sel.l1) ->  out.l0
			(                     in2.l0 &                      sel.l1) ->  out.l0
			( in1.l0 &                                sel.l0          ) ->  out.l0
			(                                        -sel.l0 & -sel.l1) -> -out.l0
			(                    -in2.l0 & -in2.l1 & -sel.l0          ) -> -out.l0
			(-in1.l0 & -in1.l1 &                               -sel.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [010010, 110010, 011010, 111010, 010110, 110110, 011110, 111110, 000101, 100101, 010101, 110101, 001101, 101101, 011101, 111101, 000011, 100011, 010011, 110011, 001011, 101011, 011011, 111011, 000111, 100111, 010111, 110111, 001111, 101111, 011111, 111111]
			initial neg: [000000, 100000, 010000, 110000, 001000, 101000, 011000, 111000, 000100, 100100, 010100, 110100, 001100, 101100, 011100, 111100, 000010, 001010, 000110, 001110, 000001, 100001, 010001, 110001]
			initial nc:  []
			initial cost: 56 terms with a total of 336 literals

			                |                  sel                  |
			      out       |                   |        l1         |
			       l1       |                   |                   |
			                |        in2        |        in2
			                |         |   l1    |   l1    |         |
			----------------+----+----+----+----+----+----+----+----+--------
			                | 0  | 0  | 0  | 0  | 1  | 1  | F  | 0  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | F  | 0  |
			        in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | 0  | 0  | 0  | 0  | 1  | 1  | F  | 0  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | F  | 0  |
			sel ------------+----+----+----+----+----+----+----+----+--------
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			             l1 +----+----+----+----+----+----+----+----+----
			                | 1  | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
			 l0     in1 ----+----+----+----+----+----+----+----+----+ l0  in1
			                | F  | F  | F  | F  | 1  | 1  | 1  | 1  |
			                +----+----+----+----+----+----+----+----+----
			                | 0  | 0  | 0  | 0  | 1  | 1  | 1  | 1  |
			----------------+----+----+----+----+----+----+----+----+--------
			                |    |   l0    |    |    |   l0    |    |
			                |        in2        |        in2        |

			pos: [****11, ***1*1, *1**1*]
			neg: [****00, **000*, 00***0]
			cost: 6 terms with a total of 14 literals

			(                                         sel.l0 &  sel.l1) ->  out.l1
			(                               in2.l1 &            sel.l1) ->  out.l1
			(           in1.l1 &                      sel.l0          ) ->  out.l1
			(                                        -sel.l0 & -sel.l1) -> -out.l1
			(                    -in2.l0 & -in2.l1 & -sel.l0          ) -> -out.l1
			(-in1.l0 & -in1.l1 &                               -sel.l1) -> -out.l1
		*/
		_solver->CommitClause(                                    -sel.l0, -sel.l1,  out.l0);
		_solver->CommitClause(                  -in2.l0,                   -sel.l1,  out.l0);
		_solver->CommitClause(-in1.l0,                            -sel.l0,           out.l0);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l0);
		_solver->CommitClause(                   in2.l0,  in2.l1,  sel.l0,          -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,                             sel.l1, -out.l0);
		_solver->CommitClause(                                    -sel.l0, -sel.l1,  out.l1);
		_solver->CommitClause(                           -in2.l1,          -sel.l1,  out.l1);
		_solver->CommitClause(         -in1.l1,                   -sel.l0,           out.l1);
		_solver->CommitClause(                                     sel.l0,  sel.l1, -out.l1);
		_solver->CommitClause(                   in2.l0,  in2.l1,  sel.l0,          -out.l1);
		_solver->CommitClause( in1.l0,  in1.l1,                             sel.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeSetControl(const LogicContainer& in, const LogicContainer& set, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(set);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: SET
			Input Encodings: 01, 01
			Output Encodings: 01

			    set   in     | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 01, 11]
			initial neg: [00]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   set   |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 1  |
			 in ----+----+----+
			     l0 | 1  | 1  |
			--------+----+----+

			pos: [*1, 1*]
			neg: [00]
			cost: 3 terms with a total of 4 literals

			(          set.l0) ->  out.l0
			( in.l0          ) ->  out.l0
			(-in.l0 & -set.l0) -> -out.l0
		*/
		_solver->CommitClause(        -set.l0,  out.l0);
		_solver->CommitClause(-in.l0,           out.l0);
		_solver->CommitClause( in.l0,  set.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: SET
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     set      in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 1  0 (1)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010]
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        set        |          out   |        set        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [1*1*]
			neg: [**0*, 0***]
			cost: 3 terms with a total of 4 literals

			( in.l0 &           set.l0          ) ->  out.l0
			(                  -set.l0          ) -> -out.l0
			(-in.l0                             ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 0001, 1001, 0101]
			initial neg: [0000, 1000, 0100, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        set        |          out   |        set        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 1  |                | 0  | 1  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [***1, *11*]
			neg: [**00, *0*0]
			cost: 4 terms with a total of 7 literals

			(                             set.l1) ->  out.l1
			(          in.l1 &  set.l0          ) ->  out.l1
			(                  -set.l0 & -set.l1) -> -out.l1
			(         -in.l1 &           -set.l1) -> -out.l1
		*/
		_solver->CommitClause(-in.l0,         -set.l0,           out.l0);
		_solver->CommitClause(                 set.l0,          -out.l0);
		_solver->CommitClause( in.l0,                           -out.l0);
		_solver->CommitClause(                         -set.l1,  out.l1);
		_solver->CommitClause(        -in.l1, -set.l0,           out.l1);
		_solver->CommitClause(                 set.l0,  set.l1, -out.l1);
		_solver->CommitClause(         in.l1,           set.l1, -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: SET
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     set      in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 1  0 (1)
			  9  1  0 (1) 0  1 (0)  | 1  0 (1)
			 10  1  0 (1) 1  0 (1)  | 1  0 (1)
			 11  1  0 (1) 1  1 (X)  | 1  0 (1)
			                        |
			 12  1  1 (X) 0  0 (U)  | 1  1 (X)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110, 0001, 1001, 0101, 1101]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        set        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**11, 1*1*]
			neg: [**0*, 0**0]
			cost: 4 terms with a total of 7 literals

			(                   set.l0 &  set.l1) ->  out.l0
			( in.l0 &           set.l0          ) ->  out.l0
			(                  -set.l0          ) -> -out.l0
			(-in.l0 &                    -set.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        set        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [***1, *11*]
			neg: [**00, *0*0]
			cost: 4 terms with a total of 7 literals

			(                             set.l1) ->  out.l1
			(          in.l1 &  set.l0          ) ->  out.l1
			(                  -set.l0 & -set.l1) -> -out.l1
			(         -in.l1 &           -set.l1) -> -out.l1
		*/
		_solver->CommitClause(                -set.l0, -set.l1,  out.l0);
		_solver->CommitClause(-in.l0,         -set.l0,           out.l0);
		_solver->CommitClause(                 set.l0,          -out.l0);
		_solver->CommitClause( in.l0,                   set.l1, -out.l0);
		_solver->CommitClause(                         -set.l1,  out.l1);
		_solver->CommitClause(        -in.l1, -set.l0,           out.l1);
		_solver->CommitClause(                 set.l0,  set.l1, -out.l1);
		_solver->CommitClause(         in.l1,           set.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: SET
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     set      in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  F (0)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | F  0 (1)
			  9  F  0 (1) 0  F (0)  | F  0 (1)
			 10  F  0 (1) F  0 (1)  | F  0 (1)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |        set        |          out   |        set        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, 0***]
			cost: 2 terms with a total of 2 literals

			(                  -set.l0          ) -> -out.l0
			(-in.l0                             ) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 12 terms with a total of 48 literals

			  out   |        set        |          out   |        set        |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | F  |                | 0  | 0  | 0  | *  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | 0  | *  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | F  |                | 0  | *  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**00, *01*]
			cost: 2 terms with a total of 4 literals

			(                  -set.l0 & -set.l1) -> -out.l1
			(         -in.l1 &  set.l0          ) -> -out.l1
		*/
		_solver->CommitClause(                 set.l0,          -out.l0);
		_solver->CommitClause( in.l0,                           -out.l0);
		_solver->CommitClause(                 set.l0,  set.l1, -out.l1);
		_solver->CommitClause(         in.l1, -set.l0,          -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: SET
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     set      in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | F  1 (0)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | 1  F (1)
			  9  1  F (1) F  1 (0)  | 1  F (1)
			 10  1  F (1) 1  F (1)  | 1  F (1)
			 11  1  F (1) 1  1 (X)  | 1  F (1)
			                        |
			 12  1  1 (X) 0  0 (U)  | 1  1 (X)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010]
			initial nc:  []
			initial cost: 11 terms with a total of 44 literals

			  out   |        set        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | F  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | F  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**11, 1*1*]
			neg: [**00, 00*0]
			cost: 4 terms with a total of 9 literals

			(                   set.l0 &  set.l1) ->  out.l0
			( in.l0 &           set.l0          ) ->  out.l0
			(                  -set.l0 & -set.l1) -> -out.l0
			(-in.l0 & -in.l1 &           -set.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |        set        |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [***1, *11*]
			neg: [**00, 00*0]
			cost: 4 terms with a total of 8 literals

			(                             set.l1) ->  out.l1
			(          in.l1 &  set.l0          ) ->  out.l1
			(                  -set.l0 & -set.l1) -> -out.l1
			(-in.l0 & -in.l1 &           -set.l1) -> -out.l1
		*/
		_solver->CommitClause(                -set.l0, -set.l1,  out.l0);
		_solver->CommitClause(-in.l0,         -set.l0,           out.l0);
		_solver->CommitClause(                 set.l0,  set.l1, -out.l0);
		_solver->CommitClause( in.l0,  in.l1,           set.l1, -out.l0);
		_solver->CommitClause(                         -set.l1,  out.l1);
		_solver->CommitClause(        -in.l1, -set.l0,           out.l1);
		_solver->CommitClause(                 set.l0,  set.l1, -out.l1);
		_solver->CommitClause( in.l0,  in.l1,           set.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeResetControl(const LogicContainer& in, const LogicContainer& reset, const LogicContainer& out)
{
	ASSERT_SET(in);
	ASSERT_SET(reset);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: RESET
			Input Encodings: 01, 01
			Output Encodings: 01

			    reset in     | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [10]
			initial neg: [00, 01, 11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |  reset  |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 0  |
			 in ----+----+----+
			     l0 | 1  | 0  |
			--------+----+----+

			pos: [10]
			neg: [*1, 0*]
			cost: 3 terms with a total of 4 literals

			( in.l0 & -reset.l0) ->  out.l0
			(          reset.l0) -> -out.l0
			(-in.l0            ) -> -out.l0
		*/
		_solver->CommitClause(-in.l0,  reset.l0,  out.l0);
		_solver->CommitClause(        -reset.l0, -out.l0);
		_solver->CommitClause( in.l0,            -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: RESET
			Input Encodings: 01X, 01X
			Output Encodings: 01X

			     reset    in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  1 (0)  | 0  0 (X)
			  2  0  0 (X) 1  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0  0 (X)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) *  * (*)  | *  * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0  1 (0)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  1 (0)  | *  * (*)
			 14  *  * (*) 1  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 0001, 1001, 0101]
			initial neg: [0000, 1000, 0100, 0010, 0110]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |       reset       |          out   |       reset       |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 1  |                | 0  | 1  | 1  | 1  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [***1, 1*1*]
			neg: [**00, 0**0]
			cost: 4 terms with a total of 7 literals

			(                               reset.l1) ->  out.l0
			( in.l0 &           reset.l0            ) ->  out.l0
			(                  -reset.l0 & -reset.l1) -> -out.l0
			(-in.l0 &                      -reset.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |       reset       |          out   |       reset       |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*]
			neg: [**0*, *0**]
			cost: 3 terms with a total of 4 literals

			(          in.l1 &  reset.l0            ) ->  out.l1
			(                  -reset.l0            ) -> -out.l1
			(         -in.l1                        ) -> -out.l1
		*/
		_solver->CommitClause(                           -reset.l1,  out.l0);
		_solver->CommitClause(-in.l0,         -reset.l0,             out.l0);
		_solver->CommitClause(                 reset.l0,  reset.l1, -out.l0);
		_solver->CommitClause( in.l0,                     reset.l1, -out.l0);
		_solver->CommitClause(        -in.l1, -reset.l0,             out.l1);
		_solver->CommitClause(                 reset.l0,            -out.l1);
		_solver->CommitClause(         in.l1,                       -out.l1);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: RESET
			Input Encodings: U01X, U01X
			Output Encodings: U01X

			     reset    in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) 0  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  0 (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0  0 (U)
			  5  0  1 (0) 0  1 (0)  | 0  1 (0)
			  6  0  1 (0) 1  0 (1)  | 1  0 (1)
			  7  0  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0  1 (0)
			  9  1  0 (1) 0  1 (0)  | 0  1 (0)
			 10  1  0 (1) 1  0 (1)  | 0  1 (0)
			 11  1  0 (1) 1  1 (X)  | 0  1 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 1  1 (X)
			 13  1  1 (X) 0  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  0 (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 0110]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |       reset       |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 1  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [***1, 1*1*]
			neg: [**00, 0**0]
			cost: 4 terms with a total of 7 literals

			(                               reset.l1) ->  out.l0
			( in.l0 &           reset.l0            ) ->  out.l0
			(                  -reset.l0 & -reset.l1) -> -out.l0
			(-in.l0 &                      -reset.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 0001, 1001, 0101, 1101]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |       reset       |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 1  | 0  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**11, *11*]
			neg: [**0*, *0*0]
			cost: 4 terms with a total of 7 literals

			(                   reset.l0 &  reset.l1) ->  out.l1
			(          in.l1 &  reset.l0            ) ->  out.l1
			(                  -reset.l0            ) -> -out.l1
			(         -in.l1 &             -reset.l1) -> -out.l1
		*/
		_solver->CommitClause(                           -reset.l1,  out.l0);
		_solver->CommitClause(-in.l0,         -reset.l0,             out.l0);
		_solver->CommitClause(                 reset.l0,  reset.l1, -out.l0);
		_solver->CommitClause( in.l0,                     reset.l1, -out.l0);
		_solver->CommitClause(                -reset.l0, -reset.l1,  out.l1);
		_solver->CommitClause(        -in.l1, -reset.l0,             out.l1);
		_solver->CommitClause(                 reset.l0,            -out.l1);
		_solver->CommitClause(         in.l1,             reset.l1, -out.l1);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: RESET
			Input Encodings: 01F, 01F
			Output Encodings: 01F

			     reset    in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (X) 0  0 (X)  | 0  0 (X)
			  1  0  0 (X) 0  F (0)  | 0  0 (X)
			  2  0  0 (X) F  0 (1)  | 0  0 (X)
			  3  0  0 (X) *  * (*)  | *  * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0  0 (X)
			  5  0  F (0) 0  F (0)  | 0  F (0)
			  6  0  F (0) F  0 (1)  | F  0 (1)
			  7  0  F (0) *  * (*)  | *  * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0  F (0)
			  9  F  0 (1) 0  F (0)  | 0  F (0)
			 10  F  0 (1) F  0 (1)  | 0  F (0)
			 11  F  0 (1) *  * (*)  | *  * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | *  * (*)
			 13  *  * (*) 0  F (0)  | *  * (*)
			 14  *  * (*) F  0 (1)  | *  * (*)
			 15  *  * (*) *  * (*)  | *  * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 0110]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 12 terms with a total of 48 literals

			  out   |       reset       |          out   |       reset       |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | F  | *  | F  |                | 0  | *  | *  | *  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | *  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | F  |                | 0  | 0  | *  | *  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**00, 0**0]
			cost: 2 terms with a total of 4 literals

			(                  -reset.l0 & -reset.l1) -> -out.l0
			(-in.l0 &                      -reset.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: []
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 1001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 15 terms with a total of 60 literals

			  out   |       reset       |          out   |       reset       |
			  l1    |         |   l1    |          l1    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 0  |                | 0  | 0  | 0  | 0  |
			 in----+----+----+----+----+ l0       in----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | 0  | *  | *  | 0  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | F  | *  | 0  |                | 0  | *  | *  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: []
			neg: [**0*, *0**]
			cost: 2 terms with a total of 2 literals

			(                  -reset.l0            ) -> -out.l1
			(         -in.l1                        ) -> -out.l1
		*/
		_solver->CommitClause(                 reset.l0,  reset.l1, -out.l0);
		_solver->CommitClause( in.l0,                     reset.l1, -out.l0);
		_solver->CommitClause(                 reset.l0,            -out.l1);
		_solver->CommitClause(         in.l1,                       -out.l1);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: RESET
			Input Encodings: U01F, U01F
			Output Encodings: U01F

			     reset    in        | out
			     l1 l0    l1 l0     | l1 l0
			------------------------+-----------
			  0  0  0 (U) 0  0 (U)  | 0  0 (U)
			  1  0  0 (U) F  1 (0)  | 0  0 (U)
			  2  0  0 (U) 1  F (1)  | 0  0 (U)
			  3  0  0 (U) 1  1 (X)  | 0  0 (U)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0  0 (U)
			  5  F  1 (0) F  1 (0)  | F  1 (0)
			  6  F  1 (0) 1  F (1)  | 1  F (1)
			  7  F  1 (0) 1  1 (X)  | 1  1 (X)
			                        |
			  8  1  F (1) 0  0 (U)  | F  1 (0)
			  9  1  F (1) F  1 (0)  | F  1 (0)
			 10  1  F (1) 1  F (1)  | F  1 (0)
			 11  1  F (1) 1  1 (X)  | F  1 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 1  1 (X)
			 13  1  1 (X) F  1 (0)  | 1  1 (X)
			 14  1  1 (X) 1  F (1)  | 1  1 (X)
			 15  1  1 (X) 1  1 (X)  | 1  1 (X)


			Output out, Literal l0
			-------------------------------

			initial pos: [1010, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010]
			initial nc:  []
			initial cost: 15 terms with a total of 60 literals

			  out   |       reset       |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | 1  |
			        +----+----+----+----+----
			        | 0  | 1  | 1  | 1  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----
			        | 0  | F  | 1  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [***1, 1*1*]
			neg: [**00, 00*0]
			cost: 4 terms with a total of 8 literals

			(                               reset.l1) ->  out.l0
			( in.l0 &           reset.l0            ) ->  out.l0
			(                  -reset.l0 & -reset.l1) -> -out.l0
			(-in.l0 & -in.l1 &             -reset.l1) -> -out.l0

			Output out, Literal l1
			-------------------------------

			initial pos: [0110, 1110, 0011, 1011, 0111, 1111]
			initial neg: [0000, 1000, 0100, 1100, 0010]
			initial nc:  []
			initial cost: 11 terms with a total of 44 literals

			  out   |       reset       |
			  l1    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 1  | F  |
			        +----+----+----+----+----
			        | 0  | F  | 1  | F  |
			 in----+----+----+----+----+ l0
			        | 0  | 1  | 1  | F  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 1  | F  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [**11, *11*]
			neg: [**00, 00*0]
			cost: 4 terms with a total of 9 literals

			(                   reset.l0 &  reset.l1) ->  out.l1
			(          in.l1 &  reset.l0            ) ->  out.l1
			(                  -reset.l0 & -reset.l1) -> -out.l1
			(-in.l0 & -in.l1 &             -reset.l1) -> -out.l1
		*/
		_solver->CommitClause(                           -reset.l1,  out.l0);
		_solver->CommitClause(-in.l0,         -reset.l0,             out.l0);
		_solver->CommitClause(                 reset.l0,  reset.l1, -out.l0);
		_solver->CommitClause( in.l0,  in.l1,             reset.l1, -out.l0);
		_solver->CommitClause(                -reset.l0, -reset.l1,  out.l1);
		_solver->CommitClause(        -in.l1, -reset.l0,             out.l1);
		_solver->CommitClause(                 reset.l0,  reset.l1, -out.l1);
		_solver->CommitClause( in.l0,  in.l1,             reset.l1, -out.l1);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicEquivalenceDetector(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer01& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: EQ
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 1 (1)
			 1  0 (0) 1 (1)  | 0 (0)
			 2  1 (1) 0 (0)  | 0 (0)
			 3  1 (1) 1 (1)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [00, 11]
			initial neg: [10, 01]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 1  | 0  |
			in1 ----+----+----+
			     l0 | 0  | 1  |
			--------+----+----+

			pos: [00, 11]
			neg: [01, 10]
			cost: 4 terms with a total of 8 literals

			(-in1.l0 & -in2.l0) ->  out.l0
			( in1.l0 &  in2.l0) ->  out.l0
			(-in1.l0 &  in2.l0) -> -out.l0
			( in1.l0 & -in2.l0) -> -out.l0
		*/
		_solver->CommitClause( in1.l0,  in2.l0,  out.l0);
		_solver->CommitClause(-in1.l0, -in2.l0,  out.l0);
		_solver->CommitClause( in1.l0, -in2.l0, -out.l0);
		_solver->CommitClause(-in1.l0,  in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: EQ
			Input Encodings: 01X, 01X
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (X) 0  0 (X)  | 1 (1)
			  1  0  0 (X) 0  1 (0)  | 0 (0)
			  2  0  0 (X) 1  0 (1)  | 0 (0)
			  3  0  0 (X) *  * (*)  | * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0 (0)
			  5  0  1 (0) 0  1 (0)  | 1 (1)
			  6  0  1 (0) 1  0 (1)  | 0 (0)
			  7  0  1 (0) *  * (*)  | * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0 (0)
			  9  1  0 (1) 0  1 (0)  | 0 (0)
			 10  1  0 (1) 1  0 (1)  | 1 (1)
			 11  1  0 (1) *  * (*)  | * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | * (*)
			 13  *  * (*) 0  1 (0)  | * (*)
			 14  *  * (*) 1  0 (1)  | * (*)
			 15  *  * (*) *  * (*)  | * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0000, 1010, 0101]
			initial neg: [1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 1  | 0  | *  | 0  |                | 1  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1, 0000, 1*1*]
			neg: [00*1, 001*, 01*0, 100*]
			cost: 7 terms with a total of 20 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			(-in1.l0 & -in1.l1 & -in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 &            in2.l0          ) ->  out.l0
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l0
			(-in1.l0 &  in1.l1 &           -in2.l1) -> -out.l0
			( in1.l0 & -in1.l1 & -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,  in2.l0,          -out.l0);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: EQ
			Input Encodings: U01X, U01X
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (U) 0  0 (U)  | 1 (1)
			  1  0  0 (U) 0  1 (0)  | 0 (0)
			  2  0  0 (U) 1  0 (1)  | 0 (0)
			  3  0  0 (U) 1  1 (X)  | 0 (0)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0 (0)
			  5  0  1 (0) 0  1 (0)  | 1 (1)
			  6  0  1 (0) 1  0 (1)  | 0 (0)
			  7  0  1 (0) 1  1 (X)  | 0 (0)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0 (0)
			  9  1  0 (1) 0  1 (0)  | 0 (0)
			 10  1  0 (1) 1  0 (1)  | 1 (1)
			 11  1  0 (1) 1  1 (X)  | 0 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0 (0)
			 13  1  1 (X) 0  1 (0)  | 0 (0)
			 14  1  1 (X) 1  0 (1)  | 0 (0)
			 15  1  1 (X) 1  1 (X)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [0000, 1010, 0101, 1111]
			initial neg: [1000, 0100, 1100, 0010, 0110, 1110, 0001, 1001, 1101, 0011, 1011, 0111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 1  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 0  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [0000, 0101, 1010, 1111]
			neg: [*0*1, *1*0, 0*1*, 1*0*]
			cost: 8 terms with a total of 24 literals

			(-in1.l0 & -in1.l1 & -in2.l0 & -in2.l1) ->  out.l0
			(-in1.l0 &  in1.l1 & -in2.l0 &  in2.l1) ->  out.l0
			( in1.l0 & -in1.l1 &  in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 &  in1.l1 &  in2.l0 &  in2.l1) ->  out.l0
			(          -in1.l1 &            in2.l1) -> -out.l0
			(           in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &            in2.l0          ) -> -out.l0
			( in1.l0 &           -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,  in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1, -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0, -in1.l1, -in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause(         -in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,          -in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0,           in2.l0,          -out.l0);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: EQ
			Input Encodings: 01F, 01F
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (X) 0  0 (X)  | 1 (1)
			  1  0  0 (X) 0  F (0)  | 0 (0)
			  2  0  0 (X) F  0 (1)  | 0 (0)
			  3  0  0 (X) *  * (*)  | * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0 (0)
			  5  0  F (0) 0  F (0)  | 1 (1)
			  6  0  F (0) F  0 (1)  | 0 (0)
			  7  0  F (0) *  * (*)  | * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0 (0)
			  9  F  0 (1) 0  F (0)  | 0 (0)
			 10  F  0 (1) F  0 (1)  | 1 (1)
			 11  F  0 (1) *  * (*)  | * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | * (*)
			 13  *  * (*) 0  F (0)  | * (*)
			 14  *  * (*) F  0 (1)  | * (*)
			 15  *  * (*) *  * (*)  | * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0000, 1010, 0101]
			initial neg: [1000, 0100, 0010, 0110, 0001, 1001]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 1  | 0  | *  | 0  |                | 1  | 0  | 0  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*1*1, 0000, 1*1*]
			neg: [00*1, 001*, 01*0, 100*]
			cost: 7 terms with a total of 20 literals

			(           in1.l1 &            in2.l1) ->  out.l0
			(-in1.l0 & -in1.l1 & -in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 &            in2.l0          ) ->  out.l0
			(-in1.l0 & -in1.l1 &            in2.l1) -> -out.l0
			(-in1.l0 & -in1.l1 &  in2.l0          ) -> -out.l0
			(-in1.l0 &  in1.l1 &           -in2.l1) -> -out.l0
			( in1.l0 & -in1.l1 & -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause(         -in1.l1,          -in2.l1,  out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,          -in2.l0,           out.l0);
		_solver->CommitClause( in1.l0,  in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,  in1.l1, -in2.l0,          -out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,  in2.l0,          -out.l0);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: EQ
			Input Encodings: U01F, U01F
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (U) 0  0 (U)  | 1 (1)
			  1  0  0 (U) F  1 (0)  | 0 (0)
			  2  0  0 (U) 1  F (1)  | 0 (0)
			  3  0  0 (U) 1  1 (X)  | 0 (0)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0 (0)
			  5  F  1 (0) F  1 (0)  | 1 (1)
			  6  F  1 (0) 1  F (1)  | 0 (0)
			  7  F  1 (0) 1  1 (X)  | 0 (0)
			                        |
			  8  1  F (1) 0  0 (U)  | 0 (0)
			  9  1  F (1) F  1 (0)  | 0 (0)
			 10  1  F (1) 1  F (1)  | 1 (1)
			 11  1  F (1) 1  1 (X)  | 0 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0 (0)
			 13  1  1 (X) F  1 (0)  | 0 (0)
			 14  1  1 (X) 1  F (1)  | 0 (0)
			 15  1  1 (X) 1  1 (X)  | 1 (1)


			Output out, Literal l0
			-------------------------------

			initial pos: [0000, 1010, 0101, 1111]
			initial neg: [1000, 0100, 1100, 0010, 0110, 1110, 0001, 1001, 1101, 0011, 1011, 0111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 1  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 1  | 0  | 0  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 1  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 0  | 0  | 1  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [0000, 0101, 1010, 1111]
			neg: [*0*1, *1*0, 0*1*, 1*0*]
			cost: 8 terms with a total of 24 literals

			(-in1.l0 & -in1.l1 & -in2.l0 & -in2.l1) ->  out.l0
			(-in1.l0 &  in1.l1 & -in2.l0 &  in2.l1) ->  out.l0
			( in1.l0 & -in1.l1 &  in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 &  in1.l1 &  in2.l0 &  in2.l1) ->  out.l0
			(          -in1.l1 &            in2.l1) -> -out.l0
			(           in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &            in2.l0          ) -> -out.l0
			( in1.l0 &           -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause( in1.l0,  in1.l1,  in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause( in1.l0, -in1.l1,  in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1, -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0, -in1.l1, -in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,          -in2.l1, -out.l0);
		_solver->CommitClause(         -in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,          -in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0,           in2.l0,          -out.l0);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogic01DifferenceDetector(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer01& out)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Implementation: 01DIFF
			Input Encodings: 01, 01
			Output Encodings: 01

			    in2   in1    | out
			    l0    l0     | l0
			-----------------+--------
			 0  0 (0) 0 (0)  | 0 (0)
			 1  0 (0) 1 (1)  | 1 (1)
			 2  1 (1) 0 (0)  | 1 (1)
			 3  1 (1) 1 (1)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [10, 01]
			initial neg: [00, 11]
			initial nc:  []
			initial cost: 4 terms with a total of 8 literals

			  out   |   in2   |
			  l0    |    | l0 |
			--------+----+----+
			        | 0  | 1  |
			in1 ----+----+----+
			     l0 | 1  | 0  |
			--------+----+----+

			pos: [01, 10]
			neg: [00, 11]
			cost: 4 terms with a total of 8 literals

			(-in1.l0 &  in2.l0) ->  out.l0
			( in1.l0 & -in2.l0) ->  out.l0
			(-in1.l0 & -in2.l0) -> -out.l0
			( in1.l0 &  in2.l0) -> -out.l0
		*/
		_solver->CommitClause( in1.l0, -in2.l0,  out.l0);
		_solver->CommitClause(-in1.l0,  in2.l0,  out.l0);
		_solver->CommitClause( in1.l0,  in2.l0, -out.l0);
		_solver->CommitClause(-in1.l0, -in2.l0, -out.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: 01DIFF
			Input Encodings: 01X, 01X
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (X) 0  0 (X)  | 0 (0)
			  1  0  0 (X) 0  1 (0)  | 0 (0)
			  2  0  0 (X) 1  0 (1)  | 0 (0)
			  3  0  0 (X) *  * (*)  | * (*)
			                        |
			  4  0  1 (0) 0  0 (X)  | 0 (0)
			  5  0  1 (0) 0  1 (0)  | 0 (0)
			  6  0  1 (0) 1  0 (1)  | 1 (1)
			  7  0  1 (0) *  * (*)  | * (*)
			                        |
			  8  1  0 (1) 0  0 (X)  | 0 (0)
			  9  1  0 (1) 0  1 (0)  | 1 (1)
			 10  1  0 (1) 1  0 (1)  | 0 (0)
			 11  1  0 (1) *  * (*)  | * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | * (*)
			 13  *  * (*) 0  1 (0)  | * (*)
			 14  *  * (*) 1  0 (1)  | * (*)
			 15  *  * (*) *  * (*)  | * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*, 1**1]
			neg: [*0*0, 0*0*]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer>)
	{
		/*
			Implementation: 01DIFF
			Input Encodings: U01X, U01X
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (U) 0  0 (U)  | 0 (0)
			  1  0  0 (U) 0  1 (0)  | 0 (0)
			  2  0  0 (U) 1  0 (1)  | 0 (0)
			  3  0  0 (U) 1  1 (X)  | 0 (0)
			                        |
			  4  0  1 (0) 0  0 (U)  | 0 (0)
			  5  0  1 (0) 0  1 (0)  | 0 (0)
			  6  0  1 (0) 1  0 (1)  | 1 (1)
			  7  0  1 (0) 1  1 (X)  | 0 (0)
			                        |
			  8  1  0 (1) 0  0 (U)  | 0 (0)
			  9  1  0 (1) 0  1 (0)  | 1 (1)
			 10  1  0 (1) 1  0 (1)  | 0 (0)
			 11  1  0 (1) 1  1 (X)  | 0 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0 (0)
			 13  1  1 (X) 0  1 (0)  | 0 (0)
			 14  1  1 (X) 1  0 (1)  | 0 (0)
			 15  1  1 (X) 1  1 (X)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 1110, 0001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [0110, 1001]
			neg: [**11, *0*0, 0*0*, 11**]
			cost: 6 terms with a total of 16 literals

			(-in1.l0 &  in1.l1 &  in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 & -in1.l1 & -in2.l0 &  in2.l1) ->  out.l0
			(                     in2.l0 &  in2.l1) -> -out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
			( in1.l0 &  in1.l1                    ) -> -out.l0
		*/
		_solver->CommitClause( in1.l0, -in1.l1, -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,  in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(                  -in2.l0, -in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0, -in1.l1,                   -out.l0);
	}
	else if constexpr (is_01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: 01DIFF
			Input Encodings: 01F, 01F
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (X) 0  0 (X)  | 0 (0)
			  1  0  0 (X) 0  F (0)  | 0 (0)
			  2  0  0 (X) F  0 (1)  | 0 (0)
			  3  0  0 (X) *  * (*)  | * (*)
			                        |
			  4  0  F (0) 0  0 (X)  | 0 (0)
			  5  0  F (0) 0  F (0)  | 0 (0)
			  6  0  F (0) F  0 (1)  | 1 (1)
			  7  0  F (0) *  * (*)  | * (*)
			                        |
			  8  F  0 (1) 0  0 (X)  | 0 (0)
			  9  F  0 (1) 0  F (0)  | 1 (1)
			 10  F  0 (1) F  0 (1)  | 0 (0)
			 11  F  0 (1) *  * (*)  | * (*)
			                        |
			 12  *  * (*) 0  0 (X)  | * (*)
			 13  *  * (*) 0  F (0)  | * (*)
			 14  *  * (*) F  0 (1)  | * (*)
			 15  *  * (*) *  * (*)  | * (*)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 0010, 1010, 0001, 0101]
			initial nc:  [1100, 1110, 1101, 0011, 1011, 0111, 1111]
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |          out   |        in2        |
			  l0    |         |   l1    |          l0    |         |   l1    |
			--------+----+----+----+----+        --------+----+----+----+----+
			        | 0  | 0  | *  | 0  |                | 0  | 0  | *  | 0  |
			        +----+----+----+----+----            +----+----+----+----+----
			        | 0  | 0  | *  | 1  |                | 0  | 0  | 1  | 1  |
			in1----+----+----+----+----+ l0      in1----+----+----+----+----+ l0
			        | *  | *  | *  | *  |                | *  | 1  | 1  | 1  |
			     l1 +----+----+----+----+----         l1 +----+----+----+----+----
			        | 0  | 1  | *  | 0  |                | 0  | 1  | 1  | 0  |
			--------+----+----+----+----+        --------+----+----+----+----+
			             |   l0    |                          |   l0    |

			pos: [*11*, 1**1]
			neg: [*0*0, 0*0*]
			cost: 4 terms with a total of 8 literals

			(           in1.l1 &  in2.l0          ) ->  out.l0
			( in1.l0 &                      in2.l1) ->  out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
		*/
		_solver->CommitClause(         -in1.l1, -in2.l0,           out.l0);
		_solver->CommitClause(-in1.l0,                   -in2.l1,  out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
	}
	else if constexpr (is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Implementation: 01DIFF
			Input Encodings: U01F, U01F
			Output Encodings: 01

			     in2      in1       | out
			     l1 l0    l1 l0     | l0
			------------------------+--------
			  0  0  0 (U) 0  0 (U)  | 0 (0)
			  1  0  0 (U) F  1 (0)  | 0 (0)
			  2  0  0 (U) 1  F (1)  | 0 (0)
			  3  0  0 (U) 1  1 (X)  | 0 (0)
			                        |
			  4  F  1 (0) 0  0 (U)  | 0 (0)
			  5  F  1 (0) F  1 (0)  | 0 (0)
			  6  F  1 (0) 1  F (1)  | 1 (1)
			  7  F  1 (0) 1  1 (X)  | 0 (0)
			                        |
			  8  1  F (1) 0  0 (U)  | 0 (0)
			  9  1  F (1) F  1 (0)  | 1 (1)
			 10  1  F (1) 1  F (1)  | 0 (0)
			 11  1  F (1) 1  1 (X)  | 0 (0)
			                        |
			 12  1  1 (X) 0  0 (U)  | 0 (0)
			 13  1  1 (X) F  1 (0)  | 0 (0)
			 14  1  1 (X) 1  F (1)  | 0 (0)
			 15  1  1 (X) 1  1 (X)  | 0 (0)


			Output out, Literal l0
			-------------------------------

			initial pos: [0110, 1001]
			initial neg: [0000, 1000, 0100, 1100, 0010, 1010, 1110, 0001, 0101, 1101, 0011, 1011, 0111, 1111]
			initial nc:  []
			initial cost: 16 terms with a total of 64 literals

			  out   |        in2        |
			  l0    |         |   l1    |
			--------+----+----+----+----+
			        | 0  | 0  | 0  | 0  |
			        +----+----+----+----+----
			        | 0  | 0  | 0  | 1  |
			in1----+----+----+----+----+ l0
			        | 0  | 0  | 0  | 0  |
			     l1 +----+----+----+----+----
			        | 0  | 1  | 0  | 0  |
			--------+----+----+----+----+
			             |   l0    |

			pos: [0110, 1001]
			neg: [**11, *0*0, 0*0*, 11**]
			cost: 6 terms with a total of 16 literals

			(-in1.l0 &  in1.l1 &  in2.l0 & -in2.l1) ->  out.l0
			( in1.l0 & -in1.l1 & -in2.l0 &  in2.l1) ->  out.l0
			(                     in2.l0 &  in2.l1) -> -out.l0
			(          -in1.l1 &           -in2.l1) -> -out.l0
			(-in1.l0 &           -in2.l0          ) -> -out.l0
			( in1.l0 &  in1.l1                    ) -> -out.l0
		*/
		_solver->CommitClause( in1.l0, -in1.l1, -in2.l0,  in2.l1,  out.l0);
		_solver->CommitClause(-in1.l0,  in1.l1,  in2.l0, -in2.l1,  out.l0);
		_solver->CommitClause(                  -in2.l0, -in2.l1, -out.l0);
		_solver->CommitClause(          in1.l1,           in2.l1, -out.l0);
		_solver->CommitClause( in1.l0,           in2.l0,          -out.l0);
		_solver->CommitClause(-in1.l0, -in1.l1,                   -out.l0);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

}
}

#endif // BASIC_LOGIC_ENCODER_GEN
