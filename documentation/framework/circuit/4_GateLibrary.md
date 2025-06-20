[Previous Topic](3_CircuitBuilder.md) - [Index](../../../README.md)

# Gate Library

Below is a list with all gate types implemented.
Each gate has a set of inputs and outputs.
A port (input / output) can be either matched by name or by position.
The positions of the pins start at 0 and increases from left to right (top to bottom in the figures).
Below is an exmple, where an AND-gate myAnd1 is matched by position and an AND-gate myAnd2 is using port names.
Matching by port name should be preferred when writing a cell-library to prevent mistakes.

```verilog
`celldefine

module MY_CELL(input A, input B, input C, output O1, output O2);
       and myAnd1(OUT1, A, B);
       and myAnd2(.in1(A), .in2(C), .out(O2));
endmodule

`endcelldefine
```

## Primary Input / Output gates

- input (Input port of the top-level module)
- output (Output port of the top-level module)
- inout (Inout port of the top-level module)

```text
        module input(output out);

        +-----------+
        |   input   +--- out
        +-----------+

        module output(input in);

        +-----------+
  in ---+   output  |
        +-----------+

        module inout(output out, input in);

        +-----------+
  in ---+   inout   +--- out
        +-----------+
```

# Simple combinatorial gates

- buf (Buffer gate)
- inv (Inverter gate)
- and (Logical And gate)
- nand (Logical Nand gate)
- or (Logical Or gate)
- nor (Logical Nor gate)
- xor (Logical Xor gate)
- xnor (Logical Xnor gate)
- mux (Multiplexer gate)

```text
        module buf(output out, input in1);

        +-----------+
 in1 ---+    buf    +--- out
        +-----------+

        module inv(output out, input in1);

        +-----------+
 in1 ---+    inv    +--- out
        +-----------+

        module and(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |    and    +--- out
 in2 ---+           |
        +-----------+

        module nand(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |   nand    +--- out
 in2 ---+           |
        +-----------+

        module or(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |    or     +--- out
 in2 ---+           |
        +-----------+

        module nor(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |    nor    +--- out
 in2 ---+           |
        +-----------+

        module xor(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |    xor    +--- out
 in2 ---+           |
        +-----------+

        module xnor(output out, input in1, input in2);

        +-----------+
 in1 ---+           |
        |   xnor    +--- out
 in2 ---+           |
        +-----------+

        module mux(output out, input in1, input in2, input select);

        +-----------+
 in1 ---+           |
        |           |
 in2 ---+   mux     +--- out
        |           |
select -+           |
        +-----------+
```

# Constant gates

These gates are also used when specifying a constant bit-vector
for an input of a gate.

- tie0 (Tie to Low gate)
- tie1 (Tie to High gate)
- tiex (Tie to Don't Care gate)
- tieu (Tie to Undefined gate)

```
        module tie0(output out);

        +-----------+
        |   tie0    +--- out
        +-----------+

        module tie1(output out);

        +-----------+
        |   tie1    +--- out
        +-----------+

        module tiex(output out);

        +-----------+
        |   tiex    +--- out
        +-----------+

        module tieu(output out);

        +-----------+
        |   tieu    +--- out
        +-----------+
```

# Flip-flops / latches

The inverting output of flip-flops and latches is implement by using a second output
port of the flip-flop.

- dflipflop (D-flip-flop with clocked input signals D, SET, RESET)
- dlatch (D-latch with unclocked asynchronous signals D, SET, RESET)
- dsequential (generic sequential element with input signals D, SET, RESET)

```
        module dflipflop(output q, output qn, input d, input ck, input set, input reset);

        +------------+
   d ---+            +--- q
  ck ---+>           |
        | dflipflop  |
 set ---+            |
reset --+            +--- qn
        +------------+

        module dlatch(output q, output qn, input d, input en, input set, input reset);

        +------------+
   d ---+            +--- q
  en ---+            |
        |   dlatch   |
 set ---+            |
reset --+            +--- qn
        +------------+

        module dsequential(output q, output qn, input d, input ck, input set, input reset);

        +------------+
   d ---+            +--- q
  ck ---+>           |
        | dsequential|
 set ---+            |
reset --+            +--- qn
        +------------+
```

# Tristate gates

- bufif1 (Tristate-Buffer, mapped to AND gate)
- notif1 (Tristate-Inverter, mapped to NAND gate)
- bufif0 (Tristate-Buffer, mapped to AND gate with inverted enable)
- notif0 (Tristate-Inverter, mapped to NAND gate with inverted enable)

```
        module bufif1(output out, input in, input enable);

        +-----------+
  in ---+           |
        |  bufif1   +--- out
enable -+           |
        +-----------+

        module notif1(output out, input in, input enable);

        +-----------+
  in ---+           |
        |  notif1   +--- out
enable -+           |
        +-----------+

        module bufif0(output out, input in, input enable);

        +-----------+
  in ---+           |
        |  bufif0   +--- out
enable -+           |
        +-----------+

        module notif0(output out, input in, input enable);

        +-----------+
  in ---+           |
        |  notif0   +--- out
enable -+           |
        +-----------+
```

[Previous Topic](3_CircuitBuilder.md) - [Index](../../../README.md)
