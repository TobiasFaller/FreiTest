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

## Where is the code?

We are in the process of cleaning up the code, and removing and replacing proprietary data in our internal repository.
We will upload the cleaned parts as soon as they are ready.
Find the current progress below:

- [X] Data / Cell-Libraries: Provide a generic cell-library verilog implementation to eliminate dependency on proprietary cell-libraries
- [X] Data / Cell-Libraries: Provide a generic cell-library liberty file for synthesis
- [ ] Data / Benchmarks: Provide generated benchmarks for generic cell-library
- [ ] Data / Fault Models: Provide cell-aware fault model for generic cell-library
- [ ] Script: Generator for generic benchmark circuits and VCMs for generic cell-library
- [ ] Script: Synthesis for ITC99 benchmarks for generic cell-library
- [ ] Build: Clean build configuration from proprietary parts
- [ ] Build: Port Ubuntu 20.04 LTS build to Ubuntu 24.04 LTS
- [ ] Build: Port CentOS 7 build to CentOS 8 (for RHEL 8)
- [ ] Build: Provide GitHub CI configuration
- [ ] Code: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Code: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Code: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Code: Provide open-source untestability ATPG workflows (PoliTO)
- [ ] Code: Provide utility workflows
- [ ] Documentation: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Documentation: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Documentation: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Documentation: Provide open-source untestability ATPG workflows (PoliTO)
- [ ] Documentation: Provide utility workflows
- [ ] Configuration: Provide open-source basic ATPG workflows (Scale4Edge)
- [ ] Configuration: Provide open-source SBST ATPG workflows (Scale4Edge)
- [ ] Configuration: Provide open-source switching activity ATPG workflows (PoliTO)
- [ ] Configuration: Provide open-source untestability ATPG workflows (PoliTO)
- [ ] Configuration: Provide utility workflows

---

This work is supported in part by the German Federal Ministry of Education and Research (BMBF) within the project Scale4Edge under contract no. 16ME0132.
