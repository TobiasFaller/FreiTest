[Previous Topic](1_CircuitRepresentation.md) - [Index](../../../README.md) -  [Next Topic](3_CircuitBuilder.md)

# Verilog Instantiator

The task of the Verilog instantiator is to convert the Verilog description of a module into a circuit representation.
This is done in two steps:

1. First, the module is instantiated and converted into an intermediate representation which consists of gates and connections.
   The hierarchy has been collapsed by the Verilog Instantiator and all connections are formed.
   However, only basic connectivity and sanity checks are performed.
2. Then, the intermediate representation is converted into a `CircuitBuilder` instance which can be used to create the different circuit representations.

```text
+-------------------------------------------------------+
|                       Verilog Module                  |
+-------------------------------------------------------+
                              |
                              V
+-------------------------------------------------------+
|                 Intermediate Representation           |
+-------------------------------------------------------+
                              |
                              V
+-------------------------------------------------------+
|                      Circuit Builder                  |
+-------------------------------------------------------+
          |                   |                 |
          V                   V                 V
+-------------------------------------------------------+
| Circuit Environment                                   |
| +----------------+ +------------------+ +-----------+ |
| | Mapped Circuit | | Unmapped Circuit | | Meta Data | |
| +----------------+ +------------------+ +-----------+ |
+-------------------------------------------------------+
```

## Transformation from Verilog Module to Intermediate Representation

The transformation from the Verilog Module to an intermediate representation format is done by using so called primitives.
The name primitives originates from the fact, that these represent the minimal building blocks for a circuit.

```text
   +--------------+
   |   Primitive  |
   |              |
---+ IN_A   OUT_A +---
   |              |
---+ IN_B   OUT_B +---
   |              |
---+ IN_C   OUT_C +---
   |              |
---+ IN_D   OUT_D +---
   |              |
   +--------------+
```

Each primitive has a defined number of input and output ports which have to be named.
The method `ConvertToCircuit` has to be implemented for an primitive to be valid.
It converts the primitive element into the mapped and unmapped circuit representation by using a CircuitBuilder instance.
This is documented below in more detailed form.

Example:

```cpp
BasicPrimitive("xor", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_XOR, CellType::XOR);
BasicPrimitive("and", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_AND, CellType::AND);
```
```text
   +----------+             +----------+
   |          |             |          |
---+ in1  out +---       ---+ in1  out +---
   |    xor   |             |    and   |
---+ in2      |          ---+ in2      |
   |          |             |          |
   +----------+             +----------+
```

The order and naming of the ports is relevant for when the primitive is instantiated in a Verilog module.
Only Verilog modules which are library modules (surrounded by `'celldefine` and `'endcelldefine`) are allowed to instantiate primitives.

```verilog
`celldefine

// A Verilog module, which is a library module and instantiates two primitives
module half_adder(input A, input B, output S, output C);
    xor output_xor(.in1(A), .in2(B), .out(S));
    and carry_and(.in1(A), .in2(B), .out(C));
endmodule

`endcelldefine
```

## Intermediate Representation

The intermediate representation of the circuit consists of three types - gates, connections and groups.
The constructs are combined together into a circuit with the `InstantiationContext`.

```cpp
struct Gate {
    std::vector<size_t> inputs;
    std::vector<size_t> outputs;
};

struct Connection {
    std::vector<size_t> drivingGates;
    std::vector<size_t> drivenGates;
};

struct Group { };

struct InstantiationContext {
    std::vector<Gate> gates;
    std::vector<Connection> connections;
    std::Vector<Group> groups;
};
```

## Intermediate Representation to Unmapped and Mapped Circuit

The method `ConvertToCircuit` of the Primitive class is used to convert each instantiated primitive into a unmapped and mapped circuit representation.
The method is called with an PrimitiveContext which is the "hull" of the primitive used to represent all the declared input / output pins.
The PrimitiveContext is later used to connect all primitives which were created by themselves one after eachother.

The implementation of the primitive creates gates inside this primitive "hull".
All internal connections inside the "hull" are created by the primitive implementation.

After creation of the internal components, the pins of the PrimitiveContext should be assigned with the corresponding pins of the internal gates.
All input and output pins of the PrimitiveContext have to be connected to a gate.
If an input pin is unused it can be connected to a buffer terminating the input.
If an output pin is unused it can be connected to a tieX gate.

```text
+-------------------------------------+
| Circuit                             |
|      +----------------------+       |
|      | Primitive Context    |       |
|      |                      |       |
|      |           +---+      |       |
| IN_A +-----------+   +------+ OUT_A |
|      |  +---+  +-+   |      |       |
| IN_B +--+   +--+ +---+      + OUT_B |
|      |  +---+    +---+      |       |
| IN_C +-----------+   +------+ OUT_C |
|      |  +---+    +---+      |       |
| IN_D +--+   +---------------+ OUT_D |
|      |  +---+               |       |
|      +----------------------+       |
+-------------------------------------+
```

```cpp
// Example implementation of a two-port XOR gate
bool ConvertToCircuit(CircuitBuilder& builder, const BuildConfiguration& configuration, PrimitiveContext& context) override
{
    // Convert the XOR gate to a gate in the unmapped circuit
    UnmappedNodeId unmappedNode = builder.AddUnmappedNode(context.gate.name, "xor" 2, 1);
    UnmappedNode& unmappedNodeRef = builder.GetUnmappedNode(unmappedNode);

    // Convert the XOR gate to a gate in the mapped circuit
    MappedNodeId mappedNode = builder.AddMappedNode(context.gate.name, CellCategory::MAIN_XOR, CellType::XOR, 2);
    MappedNode& mappedNodeRef = builder.GetMappedNode(mappedNode);
    builder.LinkMappedToUnmappedNode(mappedNode, unmappedNode);

    // Name the ports
    unmappedNodeRef.SetInputPortName(0u, "in1");
    unmappedNodeRef.SetInputPortName(1u, "in2");
    unmappedNodeRef.SetOutputPortName(0u, "out");
    mappedNodeRef.SetInputPortName(0u, "in1");
    mappedNodeRef.SetInputPortName(1u, "in2");
    mappedNodeRef.SetOutputPortName("out");

    // Assign the node ids and pins to the context
    context.unmappedInputs[0u] = std::make_pair(unmappedNode, 0u);
    context.unmappedInputs[1u] = std::make_pair(unmappedNode, 1u);
    context.unmappedOutputs[0u] = std::make_pair(unmappedNode, 0u);
    context.mappedInputs[0u] = std::make_pair(mappedNode, 0u);
    context.mappedInputs[1u] = std::make_pair(mappedNode, 1u);
    context.mappedOutputs[0u] = std::make_pair(mappedNode, 0u);

    return true;
}
```

```cpp
// Example implementation of a two-port XNOR gate which uses an inverter
// instead of a native XNOR gate in the mapped circuit
bool ConvertToCircuit(CircuitBuilder& builder, const BuildConfiguration& configuration, PrimitiveContext& context) override
{
    // Convert the XOR gate to an unmapped circuit
    UnmappedNodeId unmappedXnorNode = builder.AddUnmappedNode(context.gate.name, "xnor" 2, 1);
    UnmappedNode& unmappedXnorNodeRef = builder.GetUnmappedNode(unmappedXorNode);

    // Convert the XOR gate to a mapped circuit
    MappedNodeId mappedXorNode = builder.AddMappedNode(context.gate.name, CellCategory::MAIN_XOR, CellType::XOR, 2);
    MappedNodeId mappedInvNode = builder.AddMappedNode(context.gate.name, CellCategory::MAIN_INV, CellType::INV, 1);
    MappedNode& mappedXorNodeRef = builder.GetMappedNode(mappedXorNode);
    MappedNode& mappedInvNodeRef = builder.GetMappedNode(mappedInvNode);
    builder.LinkMappedToUnmappedNode(mappedXorNode, unmappedXnorNode);
    builder.LinkMappedToUnmappedNode(mappedInvNode, unmappedXnorNode);

    // Connect the mapped gates
    mappedXorNodeRef.AddSuccessorNode(mappedInvNode);
    mappedInvNodeRef.SetInputNode(0u, mappedXorNode);

    // Name the ports
    unmappedXnorNodeRef.SetInputPortName(0u, "in1");
    unmappedXnorNodeRef.SetInputPortName(1u, "in2");
    unmappedXnorNodeRef.SetOutputPortName(0u, "out");
    mappedXorNodeRef.SetInputPortName(0u, "in1");
    mappedXorNodeRef.SetInputPortName(1u, "in2");
    mappedXorNodeRef.SetOutputPortName("out");
    mappedInvNodeRef.SetInputPortName(0u, "in");
    mappedInvNodeRef.SetOutputPortName("out");

    // Assign the node ids and pins to the context
    context.unmappedInputs[0u] = std::make_pair(unmappedXnorNode, 0u);
    context.unmappedInputs[1u] = std::make_pair(unmappedXnorNode, 1u);
    context.unmappedOutputs[0u] = std::make_pair(unmappedXnorNode, 0u);
    context.mappedInputs[0u] = std::make_pair(mappedXorNode, 0u);
    context.mappedInputs[1u] = std::make_pair(mappedXorNode, 1u);
    context.mappedOutputs[0u] = std::make_pair(mappedInvNode, 0u);

    return true;
}
```

## Flip-Flop and Latch conversion

Flip-flops and latches are replaced replaced with a secondary input and secondary output gate in the mapped circuit.
The secondary ports represent the internal state of the flip-flop between timeframes (clock cycles).
Below is an example of how a flip-flop is split:

```text
Step 1: Cut in the middle of the flip-flop

        +-----+-----+
   d ---+ dflipflop +--- q
  ck ---+>    |     |
        |sout | sin |
 set ---+     |     |
reset --+     |     +--- qn
        +-----+-----+

Step 2: Separate the two halves (imagine the flip-flop
        to still be internally connected)

        +----+     +----+
   d ---+    |     |    +--- q
  ck ---+>   |     |    |
        |sout|- - -| sin|
 set ---+    |     |    |
reset --+    |     |    +--- qn
        +----+     +----+

Step 3: The ports are now moved to the input and output
        side of the circuit, and on unrolling, the secondary
        input is connected to the secondary output of
        the previous timeframe.

+------------------------------------+----------------------------...
|                                    |
|  +----+                    +----+  |  +----+      
|  |    +--- q          d ---+    |  |  |    +--- q 
|  |    |              ck ---+>   |  |  |    |      
- -| sin|                    |sout|- - -| sin|      
|  |    |             set ---+    |  |  |    |      
|  |    +--- qn      reset --+    |  |  |    +--- qn
|  +----+                    +----+  |  +----+      
|                                    |
+------------------------------------+----------------------------...
|             timeframe 1            |             timeframe 2
+------------------------------------+----------------------------...
```

The actual implementation differs slightly from the example given above as gates in the mapped circuit always have exaclty one output pin.
The secondary input only has a single output and instead drives a buffer and an inverter that represent the Q and QN outputs.
Depending on the type of flip-flop or latch different secondary output cells are used:

- **dsequential**: S_OUT
- **dflipflop**: S_OUT_CLK
- **dlatch**: S_OUT_EN

```text
Before mapping (UnmappedCircuit)
--------------------------------

Gate names before mapping:

  (1): {ORIGINAL_GATE_NAME}

           (1)
        +--------+
   d ---+        +--- q
  ck ---+>       |
        |  dff   |
 set ---+        |
reset --+        +--- qn
        +--------+

After mapping (Circuit)
-----------------------

Gate names after mapping:
  (1): {ORIGINAL_GATE_NAME}_sout
  (2): {ORIGINAL_GATE_NAME}_sin
  (3): {ORIGINAL_GATE_NAME}_buf
  (4): {ORIGINAL_GATE_NAME}_inv

             (1)
        +-----------+
        |  S_OUT    |
d ------+ 0 (d)     |
        |           |
ck -----+ 1 (ck)    |
        |           |
set ----+ 2 (set)   |
        |           |
reset --+ 3 (reset) |
        +-----------+

     (2)                   (3)
+-----------+         +-----------+
|   sin     |    +----+  buf      +--- q
|           |    |    +-----------+
|     (q) 0 +----+         (4)
|           |    |    +-----------+
|           |    +----+  inv      +--- qn
+-----------+         +-----------+
```

[Previous Topic](1_CircuitRepresentation.md) - [Index](../../../README.md) -  [Next Topic](3_CircuitBuilder.md)
