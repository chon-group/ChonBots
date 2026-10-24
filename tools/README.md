# CHON Basic Prototypes HUB

**A community-driven repository of cognitive-agent prototypes for distributed and embedded multi-agent systems.**

The CHON Prototype HUB brings together reproducible examples of **BDI (Belief–Desire–Intention) agents** developed with **Jason, JaCaMo, JasonEmbedded, and related technologies**, interacting with physical devices or simulated environments.

Prototypes may run on **real hardware**, in **simulation** (such as SimulIDE or Webots), or in both. **A physical implementation is not required.** The goal is to make prototypes easy to explore, reproduce, adapt, and improve through community contributions.

## Architecture

Projects follow the five-layer **ACIRMa** architecture described in *[An Operating-System Infrastructure for Embedded BDI-based Multi-Agent Systems](https://www.researchgate.net/publication/405508586)*:

| Layer | Directory | Purpose |
| --- | --- | --- |
| **L5 — Managing** | `managing/` | Operating-system services, network and device configuration, deployment, monitoring, and scripts. |
| **L4 — Reasoning** | `reasoning/` | BDI agents, beliefs, goals, plans, and MAS configurations. |
| **L3 — Interfacing** | `interfacing/` | Middleware, communication protocols, and interfaces between agents and devices. |
| **L2 — Controlling** | `controlling/` | Microcontroller programs and firmware for device-level control and sensor-data processing. |
| **L1 — Actuating** | `actuating/` | Sensors, actuators, circuit designs, physical assemblies, and their simulated representations. |

The additional `assets/` directory holds documentation media; **it is not an ACIRMa layer**.

A prototype only needs the directories relevant to its implementation. Empty layers are not mandatory.


## Prototype catalog

The following prototypes form the **initial catalog**. Each link will become available once the corresponding project has been added to `prototypes/`.

| Prototype | Description | Project |
| --- | --- | --- |
| **Arduino Blink** | Basic BDI control of an LED, including actuation and device feedback. | [arduinoBlink](prototypes/arduinoBlink/) |
| **Arduino LDR Sensor** | A cognitive agent perceives ambient light through an LDR sensor. | [arduinoLDRSensor](prototypes/arduinoLDRSensor/) |
| **Arduino LCD Keypad** | Agent interaction with an LCD/keypad-based prototype and environmental readings. | [arduinoLCDKeypad](prototypes/arduinoLCDKeypad/) |
| **Arduino Smoke and Temperature Detection** | Monitoring of smoke, temperature, and humidity with agent-controlled alerts. | [arduinoSmokeAndTemperatureDetection](prototypes/arduinoSmokeAndTemperatureDetection/) |

Additional projects are welcome, including mobile robots, collaborative multi-agent devices, distributed AIoT applications, and simulated cyber-physical systems.

## Explore a prototype

1. Open a project under [`prototypes/`](prototypes/).
2. Read its `README.md` for requirements, dependencies, and execution instructions.
3. Locate the relevant implementation files under the ACIRMa layers.
4. Follow the instructions for its supported physical or simulated environment.

A project may support one or several execution frameworks and environments. **Not every project requires chonOS, a microcontroller, or physical hardware.**

## How to contribute with prototypes

We welcome new prototypes, improvements, documentation, bug fixes, and additional simulation or hardware implementations.

### 1. Fork and clone

Fork this repository on GitHub and clone your fork:

```bash
git clone https://github.com/<YOUR-USERNAME>/<REPOSITORY-NAME>.git
cd <REPOSITORY-NAME>
```

Replace the placeholders with your fork's actual URL and directory name.

### 2. Copy the template

From the repository root:

```bash
cd prototypes
cp -r ../template .
mv template myNewPrototype
cd myNewPrototype
```

Or, equivalently:

```bash
cd prototypes
cp -r ../template myNewPrototype
```

Choose a short, descriptive, unique directory name.

### 3. Implement and document

- Replace the Blink example with your prototype's source code and configuration.
- Keep files under the appropriate ACIRMa layers; remove unused directories if appropriate.
- Add physical designs under `actuating/physical/` and/or simulations under `actuating/simulated/<simulator>/`.
- Update your project's `README.md` with its purpose, prerequisites, execution steps, supported environments, BDI agents, perceptions, and actions.
- Include relevant schematics, screenshots, diagrams, or demonstration links under `assets/`.
- Prefer browsable source files over ZIP-only distributions whenever possible.
- Verify that another contributor can reproduce the example from your instructions.

### 4. Open a Pull Request

From the repository root:

```bash
git switch -c add-myNewPrototype
git add prototypes/myNewPrototype
git commit -m "Add myNewPrototype prototype"
git push -u origin add-myNewPrototype
```

Then open a Pull Request from your fork to the main repository. Describe what the prototype does, how you tested it, and which execution environments it supports.

Please also read [CONTRIBUTING.md](CONTRIBUTING.md) for the contribution guidelines.

## Minimum submission checklist

- [ ] The prototype uses BDI agents (Jason, JaCaMo, JasonEmbedded, or a related implementation).
- [ ] The agents interact with a physical device **or a simulation of a physical/exogenous environment**.
- [ ] A project-level `README.md` explains setup and execution.
- [ ] Source code and required configurations are included or clearly linked.
- [ ] The relevant ACIRMa layers are documented.
- [ ] Perceptions and actions are identified.
- [ ] The submission has been tested in at least one documented environment.

**Simulation-only submissions are welcome.**

