//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B4/B4a/include/DetectorConstruction.hh
/// \brief Definition of the B4::DetectorConstruction class

#ifndef B4DetectorConstruction_h
#define B4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

namespace B4
{

/// Detector construction class to define materials and geometry.
/// In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // get methods
    //
    const G4VPhysicalVolume* GetDetectorPhys() const; 
    const G4VPhysicalVolume* GetphysPlomo() const; 
    const G4VPhysicalVolume* GetphysPhantom() const;
    const G4VPhysicalVolume* GetphysPhantom4() const;
   


  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
                                      // magnetic field messenger

    G4VPhysicalVolume* fDetectorPhys = nullptr; 
    G4VPhysicalVolume* fphysPlomo = nullptr;
    G4VPhysicalVolume* fphysPhantom = nullptr;
    G4VPhysicalVolume* fphysPhantom4 = nullptr;


    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};

// inline functions
inline const G4VPhysicalVolume* DetectorConstruction::GetDetectorPhys() const {  
    return fDetectorPhys;  
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysPlomo() const {
    return fphysPlomo;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysPhantom() const {
    return fphysPhantom;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetphysPhantom4() const {
    return fphysPhantom4;
}


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

