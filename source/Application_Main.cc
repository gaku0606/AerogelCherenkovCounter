//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geant4 Application: Tutorial course for Hep/Medicine Users
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "UserActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4UserRunAction.hh"

#include "G4TrajectoryDrawByParticleID.hh"
//-------------------------------------------------------------------------------
  int main( int argc, char** argv )
//-------------------------------------------------------------------------------
{
// Construct the default run manager
   auto runManager = new G4RunManager;

// Set up mandatory user initialization: Geometry
   runManager->SetUserInitialization( new Geometry );

// Set up mandatory user initialization: Physics-List
   G4VModularPhysicsList* physicsList = new FTFP_BERT;
   
   ///
   G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();

   //scintillation
   opticalPhysics->SetScintillationYieldFactor(1.0);
   opticalPhysics->SetScintillationExcitationRatio(0.0);

   opticalPhysics->SetMaxNumPhotonsPerStep(300);
   opticalPhysics->SetMaxBetaChangePerStep(10.0);
   //general
   opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
   opticalPhysics->SetTrackSecondariesFirst(kScintillation, false);

   //cerenkov
   opticalPhysics->Configure(kCerenkov, true);
   opticalPhysics->SetCerenkovStackPhotons(true);
   opticalPhysics->SetMaxNumPhotonsPerStep(300);

   physicsList->RegisterPhysics(opticalPhysics);
   ///
   runManager->SetUserInitialization(physicsList);
   //runManager->SetUserInitialization(new PhysicsList("QGSP_BERT"));

// Set up user initialization: User Actions
   runManager->SetUserInitialization( new UserActionInitialization );
   
// Initialize G4 kernel
   runManager->Initialize();

// Create visualization environment
	auto visManager = new G4VisExecutive;
	visManager->Initialize();
	G4TrajectoryDrawByParticleID* modelp = new G4TrajectoryDrawByParticleID;
   modelp->Set("gamma", "white");
   modelp->Set("opticalphoton", "green");

   modelp->Set("e-", "red");
	modelp->Set("e+", "blue");
	modelp->Set("mu+", "magenta");
	modelp->Set("mu-", "magenta");
	modelp->Set("nu_e", "yellow");
	modelp->Set("nu_mu", "cyan");
	modelp->Set("anti_nu_e", "yellow");
	modelp->Set("anti_nu_mu","cyan");
	modelp->Set("neutron", "gray");
	visManager->RegisterModel(modelp);

// Get UI (User Interface) manager
   G4UImanager* uiManager = G4UImanager::GetUIpointer();

// Switch batch or interactive mode
   if ( argc == 1 ) {  // Interactive mode - no command argument
     auto uiExec = new G4UIExecutive( argc, argv );
     uiExec->SessionStart();
     delete uiExec;
   } else {            // Batch mode - 1st command argument is a macro-filename
     G4String macroName = argv[1];
     uiManager->ApplyCommand( "/control/execute " + macroName );
   }

// Job termination
   delete visManager;
   delete runManager;

   return 0;
}
