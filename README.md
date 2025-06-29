# FreiTest Framework (Freiburg Test Suite)

![FreiTest Logo](logo.png)

This is a **research framework** for Automated Test Pattern Generation (ATPG).
It contains components for fault list generation, circuit iteration, SAT gate encoding and fault simulation.
This project does not have the aim to provide a full ATPG flow and lacks most features that are expected for production use (scan chain ordering, support for industrial file formats, integrated test bench validation, etc.).

> Beware, that this framework is **constantly being updated and extended** for research.
> Some updates **will break** existing logic and no guarantee of compatibility is given.
> However, most updates require only minor changes.

On top of the FreiTest framework so-called _workflows_ provide executable utilities for research, each designed for a specific purpose.
All of the workflows are integrated into a single executable named _freitest_ that handles command line parsing, configuration file processing and circuit loading.

## Documentation

- **First Steps**
  - [**Git Setup**](documentation/framework/first_steps/1_GitSetup.md)
  - [**IDE Setup**](documentation/framework/first_steps/2_IdeSetup.md)
  - [**Build Setup**](documentation/framework/first_steps/3_BuildSetup.md)
  - [**Running FreiTest**](documentation/framework/first_steps/4_RunningFreiTest.md)
  - [**Developing Applications**](documentation/framework/first_steps/5_DevelopingApplication.md)
- Verilog Circuit Import
  - [Cricuit Representation](documentation/framework/circuit/1_CircuitRepresentation.md)
  - [Verilog Instantiator](documentation/framework/circuit/2_VerilogInstantiator.md)
  - [Circuit Builder](documentation/framework/circuit/3_CircuitBuilder.md)
  - [Gate Library](documentation/framework/circuit/4_GateLibrary.md)
- Automatic Test Pattern Generation (ATPG)
  - [Solver Proxy](documentation/framework/tpg/1_SolverProxy.md)
  - [Logic Container](documentation/framework/tpg/2_LogicContainer.md)
  - [Logic Encoding](documentation/framework/tpg/3_LogicEncoding.md)
  - [Logic Generator](documentation/framework/tpg/4_LogicGenerator.md)
  - [Circuit Simulator](documentation/framework/tpg/5_CircuitSimulator.md)
- Applications
  - [Utility Applications](documentation/applications/1_Utility.md)
  - [Circuit Applications](documentation/applications/2_Circuit.md)
  - [Scale4Edge Applications](documentation/applications/3_Scale4Edge.md)


## Where is the code?

We are in the process of cleaning up the code, and removing and replacing proprietary data in our internal repository.
We will upload the cleaned parts as soon as they are ready and the code will be available soon.
Find the current progress below:

- [X] Data / Cell-Libraries: Provide a generic cell-library verilog implementation to eliminate dependency on proprietary cell-libraries
- [X] Data / Cell-Libraries: Provide a generic cell-library liberty file for synthesis
- [X] Data / Benchmarks: Provide generated benchmarks for generic cell-library (through automatic generation)
- [X] Data / Benchmarks: Provide ISCAS85, ISCAS89, ITC99, IWS05 benchmarks for generic cell-library (through automatic download and synthesis)
- [X] Data / Benchmarks: Provide DarkRISCV and PicoRV32 processors for generic cell-library (through automatic download and synthesis)
- [ ] Data / Fault Models: Provide cell-aware fault model for generic cell-library
- [X] Script: Generator for generic benchmark circuits and VCMs for generic cell-library
- [X] Script: Synthesis for ISCAS85, ISCAS89, ITC99, IWS05 benchmarks for generic cell-library
- [X] Script: Synthesis for DarkRISCV and PicoRV32 processors for generic cell-library
- [X] Build: Clean build configuration from proprietary parts
- [X] Build: Port Ubuntu 20.04 LTS Docker build to Ubuntu 24.04 LTS
- [X] Build: Port CentOS 7 Docker build to CentOS 8 (for RHEL 8)
- [X] Build: Finish Alpine linux Docker build
- [ ] Build: Provide GitHub CI configuration
- [X] Code: Provide core FreiTest framework
- [X] Code: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Code: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Code: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Code: Provide open-source untestability ATPG workflows (PoliTO)
- [X] Code: Provide circuit export workflows
- [X] Documentation: Provide core FreiTest framework
- [X] Documentation: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Documentation: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Documentation: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Documentation: Provide open-source untestability ATPG workflows (PoliTO)
- [X] Documentation: Provide circuit export workflows
- [X] Configuration: Provide core FreiTest framework
- [X] Configuration: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Configuration: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Configuration: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Configuration: Provide open-source untestability ATPG workflows (PoliTO)
- [X] Configuration: Provide circuit export workflows

---

This work is supported in part by the German Federal Ministry of Education and Research (BMBF) within the project Scale4Edge under contract no. 16ME0132.
