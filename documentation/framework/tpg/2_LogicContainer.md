[Previous Topic](1_SolverProxy.md) - [Index](../../../README.md) - [Next Topic](3_LogicEncoding.md)

# Logic Container

To encode circuit logic that is not strictly just two-valued binary logic, logic containers store one or more literals (`BaseLiteral`).
Depending on the circuit encoding different logic containers are used.

- Two-Valued Logic: `LogicContainer01`
  - Has one literal in field `l0`
  - Has only logic values 0 (LOGIC_ZERO), and 1 (LOGIC_ONE)
  - Use the template is_01_encoding_v&lt;LogicContainer&gt; to check for this type
- Three-Valued Logic: `LogicContainer01X`
  - Has two literals in fields `l0` and `l1`
  - Has the logic values 0 (LOGIC_ZERO), 1 (LOGIC_ONE), and X (LOGIC_DONT_CARE)
  - Use the template is_01X_encoding_v&lt;LogicContainer&gt; to check for this type
- Four-Valued Logic: `LogicContainerU01X`
  - Has two literals in fields `l0` and `l1`
  - Has the logic values 0 (LOGIC_ZERO), 1 (LOGIC_ONE), X (LOGIC_DONT_CARE), and U (LOGIC_UNKNOWN)
  - Use the template is_U01X_encoding_v&lt;LogicContainer&gt; to check for this type

## Logic Container Encoding

The following tables contain the literal encoding of the container classes.

```text
LogicContainer01:
  Logic 0: l0 = 0
  Logic 1: l0 = 1

  BUF          INV
in |out      in |out
---+---      ---+---
 0 | 0        0 | 1
 1 | 1        1 | 0

   XOR          XNOR
   | 0 1        | 0 1
---+-----    ---+-----
 0 | 0 1      0 | 1 0
 1 | 1 0      1 | 0 1
 
   AND          NAND
   | 0 1        | 0 1
---+-----    ---+-----
 0 | 0 0      0 | 1 1
 1 | 0 1      1 | 1 0
 
   OR           NOR
   | 0 1        | 0 1
---+-----    ---+-----
 0 | 0 1      0 | 1 0
 1 | 1 1      1 | 0 0
```

-------------------------------------------------------------------------------

```text
LogicContainer01X:
  Logic X: l1 = 0, l0 = 0
  Logic 0: l1 = 0, l0 = 1
  Logic 1: l1 = 1, l0 = 0

  BUF              INV
in |out          in |out
---+---          ---+---
 X | X            X | X
 0 | 0            0 | 1
 1 | 1            1 | 0

   XOR            XNOR
   | X 0 1        | X 0 1
---+-------    ---+-------
 X | X X X      X | X X X
 0 | X 0 1      0 | X 1 0
 1 | X 1 0      1 | X 0 1
 
   AND            NAND
   | X 0 1        | X 0 1
---+-------    ---+-------
 X | X 0 X      X | X 1 X
 0 | 0 0 0      0 | 1 1 1
 1 | X 0 1      1 | X 1 0
 
    OR            NOR
   | X 0 1        | X 0 1
---+-------    ---+-------
 X | X X 1      X | X X 0
 0 | X 0 1      0 | X 1 0
 1 | 1 1 1      1 | 0 0 0
```

-------------------------------------------------------------------------------

```text
LogicContainerU01X:
  Logic U: l1 = 0, l0 = 0
  Logic 0: l1 = 0, l0 = 1
  Logic 1: l1 = 1, l0 = 0
  Logic X: l1 = 1, l0 = 1

  BUF              INV
in |out          in |out
---+---          ---+---
 U | U            U | U
 0 | 0            0 | 1
 1 | 1            1 | 0
 X | X            X | X

      XOR              XNOR
   | U 0 1 X        | U 0 1 X
---+---------    ---+---------
 U | U U U U      U | U U U U
 0 | U 0 1 X      0 | U 1 0 X
 1 | U 1 0 X      1 | U 0 1 X
 X | U X X X      X | U X X X
 
      AND              NAND
   | U 0 1 X        | U 0 1 X
---+---------    ---+---------
 U | U 0 U U      U | U 1 U U
 0 | 0 0 0 0      0 | 1 1 1 1
 1 | U 0 1 X      1 | U 1 0 X
 X | U 0 X X      X | U 1 X X
 
       OR              NOR
   | U 0 1 X        | U 0 1 X
---+---------    ---+---------
 U | U U 1 U      U | U U 0 U
 0 | U 0 1 X      0 | U 1 0 X
 1 | 1 1 1 1      1 | 0 0 0 0
 X | U X 1 X      X | U X 0 X
```

[Previous Topic](1_SolverProxy.md) - [Index](../../../README.md) - [Next Topic](3_LogicEncoding.md)
