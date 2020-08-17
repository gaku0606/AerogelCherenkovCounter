 //
 // include/B4aStackinAction.hhのソースコード
 //
 #ifndef StackingAction_H
 #define StackingAction_H 1
 
 #include "globals.hh"
 #include "G4UserStackingAction.hh"
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 class StackingAction : public G4UserStackingAction
 {
   public:
     StackingAction();
    ~StackingAction();
 
   public:
     G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
     void NewStage();
     void PrepareNewEvent();
 
   private:
     G4int gammaCounter;
 };
 
 //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
 #endif