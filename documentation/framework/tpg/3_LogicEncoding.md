[Previous Topic](2_LogicContainer.md) - [Index](../../../README.md) - [Next Topic](4_LogicGenerator.md)

# LogicEncoder

This is the base class for the new extendable logic generator.
It can encode most basic logic gates with two-valued 01, three-valued 01X and four-valued U01X logic values.
The "LogicContainer" template parameter for the encoding methods specifies the logic representation which is used.

Legend:

- `->` := Implies
- `-` := Negation of literal
- `|` := Or conjunction of literals
- `&` := And conjunction of literals

Each clause accepted by the SAT-Solver is an OR conjunction of (inverted) literals. \
Example clause: `out = -a | -b | c`

Most used simple logic value conclusions are implications (P -> Q) (aka. "when P is true, then Q is true"),
where P is an AND conjunction of literals and Q is the resulting logic value on the output.
All implications (P -> Q) can be rewritten as (-P | Q) according to Wikipedia.
See [Material implication (Wikipedia)](https://en.wikipedia.org/wiki/Material_implication_(rule_of_inference)) for more information.

When negating an AND conjunction it can be rewritten as an OR conjunction where all literals are inverted.
See [De Morgan's laws (Wikipedia)](https://en.wikipedia.org/wiki/De_Morgan%27s_laws) for more information. \
Example: `-(a & b & -c) = -a | -b | c`

> Example for OR gate with inputs in1, in2 and output out
> and 01 logic values:
> ```text
>        |  in1  in2 | out | Logic Value
>   Case |  l0   l0  | l0  | Conclusion
>   -----+-----------+-----+------------------------------------------
>   0    |  0    0   | 0   | when not in1 and not in2, then not out
>   1    |  0    1   | 0   | when not in1 and in2, then not out
>   2    |  1    0   | 0   | when in1 and not in2, then not out
>   3    |  1    1   | 1   | when in1 and in2, then out
>
>   This can be written as:
>   Case 0: ((-in1.l0 & -in2.l0) -> (-out.l0)) = ( in1.l0 |  in2.l0 | -out.l0)
>   Case 1: ((-in1.l0 &  in2.l0) -> (-out.l0)) = ( in1.l0 | -in2.l0 | -out.l0)
>   Case 2: (( in1.l0 & -in2.l0) -> (-out.l0)) = (-in1.l0 |  in2.l0 | -out.l0)
>   Case 3: (( in1.l0 &  in2.l0) -> ( out.l0)) = (-in1.l0 | -in2.l0 |  out.l0)
>
> This results in the following method calls to implement the or gate.
>   CommitClause( in1.l0,  in2.l0, -out.l0)
>   CommitClause( in1.l0, -in2.l0, -out.l0)
>   CommitClause(-in1.l0,  in2.l0, -out.l0)
>   CommitClause(-in1.l0, -in2.l0,  out.l0)
>
> The logic table can be simplified by declaring values as "don't care".
> Therefore the OR gate implementation can be reduced to the following clauses.
> 
>        |  in1  in2 | out | Logic Value
>   Case |  l0   l0  | l0  | Conclusion
>   -----+-----------+-----+------------------------------------------
>   0    |  0    X   | 0   | when not in1, then not out
>   1    |  X    0   | 0   | when not in2, then not out
>   2    |  1    1   | 1   | when in1 and in2, then out
>
>   This can be written as:
>   Case 0: ((-in1.l0) -> (-out.l0)) = ( in1.l0 | -out.l0)
>   Case 1: ((-in2.l0) -> (-out.l0)) = ( in2.l0 | -out.l0)
>   Case 3: (( in1.l0 &  in2.l0) -> ( out.l0)) = (-in1.l0 | -in2.l0 |  out.l0)
>
> This results in the following method calls to implement the or gate.
>   CommitClause( in1.l0, -out.l0)
>   CommitClause( in2.l0, -out.l0)
>   CommitClause(-in1.l0, -in2.l0,  out.l0)
> ```

[Previous Topic](2_LogicContainer.md) - [Index](../../../README.md) - [Next Topic](4_LogicGenerator.md)
