/* src/simulation.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Geant4/G4MTRunManager.hh"
#include "Geant4/FTFP_BERT.hh"
#include "Geant4/G4StepLimiterPhysics.hh"
#include "Geant4/G4UIExecutive.hh"
#include "Geant4/G4VisExecutive.hh"

#include "action.hh"
#include "geometry/Construction.hh"
#include "physics/Units.hh"
#include "ui.hh"

#include "util/command_line_parser.hh"
#include "util/error.hh"

//__Main Function: Simulation___________________________________________________________________
int main(int argc, char* argv[]) {
  using namespace MATHUSLA;
  using namespace MATHUSLA::MU;

  using util::cli::option;

  option help_opt   ('h', "help",   "MATHUSLA Muon Simulation", option::no_arguments);
  option gen_opt    ('g', "gen",    "Generator",                option::required_arguments);
  option det_opt    ('d', "det",    "Detector",                 option::required_arguments);
  option data_opt   ('o' ,"out",    "Data Output Directory",    option::required_arguments);
  option script_opt ('s', "script", "Custom Script",            option::required_arguments);
  option events_opt ('e', "events", "Event Count",              option::required_arguments);
  option vis_opt    ('v', "vis",    "Visualization",            option::no_arguments);
  option quiet_opt  ('q', "quiet",  "Quiet Mode",               option::no_arguments);
  option thread_opt ('j', "threads",
    "Multi-Threading Mode: Specify Optional number of threads (default: 2)",
    option::optional_arguments);

  //TODO: pass quiet argument to builder and action initiaization to improve quietness

  const auto script_argc = -1 + util::cli::parse(argv,
    {&help_opt, &gen_opt, &det_opt, &data_opt, &script_opt, &events_opt, &vis_opt, &quiet_opt, &thread_opt});

  util::error::exit_when(script_argc && !script_opt.argument,
    "[FATAL ERROR] Illegal Forwarding Arguments:\n"
    "              Passing arguments to simulation without script is disallowed.\n");

  G4UIExecutive* ui = nullptr;
  if (argc == 1 || vis_opt.count) {
    ui = new G4UIExecutive(argc, argv);
    vis_opt.count = 1;
  }

  util::error::exit_when(script_opt.argument && events_opt.argument,
    "[FATAL ERROR] Incompatible Arguments:\n",
    "              A script OR an event count can be provided, but not both.\n");

  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(time(nullptr));

  #ifdef G4MULTITHREADED
    if (thread_opt.argument) {
      auto opt = std::string(thread_opt.argument);
      if (opt == "on") {
        thread_opt.count = 2;
      } else if (opt == "off" || opt == "0") {
        thread_opt.count = 1;
      } else {
        try {
          thread_opt.count = std::stoi(opt);
        } catch (...) {
          thread_opt.count = 2;
        }
      }
    } else if (!thread_opt.count) {
      thread_opt.count = 2;
    }
    auto run = new G4MTRunManager;
    run->SetNumberOfThreads(thread_opt.count);
    std::cout << "Running " << thread_opt.count
              << (thread_opt.count > 1 ? " Threads" : " Thread") << "\n";
  #else
    auto run = new G4RunManager;
    std::cout << "Running in Single Threaded Mode.\n";
  #endif

  run->SetPrintProgress(1000);
  run->SetRandomNumberStore(false);

  Units::Define();

  auto physics = new FTFP_BERT;
  physics->RegisterPhysics(new G4StepLimiterPhysics);
  run->SetUserInitialization(physics);

  const auto detector = det_opt.argument ? det_opt.argument : "Prototype";
  run->SetUserInitialization(new Construction::Builder(detector));

  const auto generator = gen_opt.argument ? gen_opt.argument : "basic";
  const auto data_dir = data_opt.argument ? data_opt.argument : "data";
  run->SetUserInitialization(new ActionInitialization(generator, data_dir));

  auto vis = new G4VisExecutive("Quiet");
  vis->Initialize();

  Command::Execute("/run/initialize",
                   "/control/saveHistory scripts/G4History",
                   "/control/stopSavingHistory");

  Command::Execute(quiet_opt.count ? "/control/execute scripts/settings/quiet"
                                   : "/control/execute scripts/settings/verbose");

  if (vis_opt.count) {
    Command::Execute("/control/execute scripts/settings/init_vis");
    if (ui->IsGUI())
      Command::Execute("/control/execute scripts/settings/init_gui");
  }

  if (script_opt.argument) {
    util::error::exit_when(script_argc % 2,
      "[FATAL ERROR] Illegal Number of Script Forwarding Arguments:\n",
      "              Inputed ", script_argc, " arguments but forward arguments must be key-value pairs.\n");

    const auto script_path = std::string(script_opt.argument);
    if (script_argc) {
      for (size_t i = 0; i < script_argc; i+=2) {
        Command::Execute("/control/alias " + std::string(argv[i+1]) + " " + argv[i+2]);
      }
      Command::Execute("/control/execute " + script_path);
    } else {
      Command::Execute("/control/execute " + script_path);
    }
  } else if (events_opt.argument) {
    Command::Execute("/run/beamOn " + std::string(events_opt.argument));
  }

  if (ui) {
    ui->SessionStart();
    delete ui;
  }

  delete vis;
  delete run;
  return 0;
}
//----------------------------------------------------------------------------------------------
