# MATHUSLA Mu-Simulation

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/71dc15058b39409ea724775ec2b9816d)](https://app.codacy.com/app/MATHUSLA/Mu-Simulation?utm_source=github.com&utm_medium=referral&utm_content=MATHUSLA/Mu-Simulation&utm_campaign=badger)

_simulation of muons through earth material_

## Build & Run

The simulation comes with a simple build script called `install` which allows for simple build customization and execution of the muon simulation.

Here is a list of useful commands:

| Action | Options for `./install` |
|:-:|:-:|
| Build Only | `(none)`  |
| Build and Auto Run | `--run`  |
| Clean CMake Build | `--cmake --clean` |
| More Options | `--help` |

The simulation executable also comes with several configuration parameters:

| Action | Short Options | Long Options |
|:-:|:-:|:-:|
| Event Count | `-e <count>` | `--events=<count>`  |
| Particle Generator | `-g <generator>` | `--gen=<generator>` |
| Number of Threads | `-j <count>`  | `--threads=<count>`  |
| Visualization  | `-v` | `--vis` |
| Quiet Mode  | `-q` | `--quiet` |
| Custom Script  | `-s <file>`  | `--script=<file>`  |
| Help | `-h` | `--help` |

### Generators

There are two general purpose generators built in, `basic` and `range`. The `basic` generator produces a particle with constant `pT`, `eta`, and `phi` while the `range` generator produces particle within a specified range of values for each of the three variables. Any variable can also be fixed to a constant value. 

The generator defaults are specified in `src/action/GeneratorAction.cc` but they can be overwritten by a custom generation script.

### Custom Scripts

A custom _Geant4_ script can be specified at run time. The script can contain generator specific commands and settings as well as _Pythia8_ settings in the form of `readString`.

### Custom Detector

_coming soon..._