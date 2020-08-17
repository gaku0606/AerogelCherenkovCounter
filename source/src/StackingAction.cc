 //
 // include/B4aStackinAction.ccのソースコード
 //
 #include "StackingAction.hh"
 
 #include "G4ParticleDefinition.hh"
 #include "G4ParticleTypes.hh"
 #include "G4Track.hh"
 #include "G4ios.hh"
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 StackingAction::StackingAction()
 : gammaCounter(0)
 {}
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 StackingAction::~StackingAction()
 {}
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 G4ClassificationOfNewTrack
 StackingAction::ClassifyNewTrack(const G4Track * aTrack)
 {
   if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
   { // particle is optical photon
     if(aTrack->GetParentID()>0)
     { // particle is secondary
       gammaCounter++;
     }
   }
   return fUrgent;
 }
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 void StackingAction::NewStage()
 {
   G4cout << "Number of optical photons produced in this event : "
          << gammaCounter << G4endl;
 }
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 void StackingAction::PrepareNewEvent()
 { gammaCounter = 0; }
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......