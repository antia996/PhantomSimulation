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
/// \file B4/B4a/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B4::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh" //incluida por mi
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>


namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  auto particleDefinition
    = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -40 * cm));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(2.5*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

    
  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }


  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }


  //Generacion de numero random entre 0.997199 y 1:
 // Definir los límites del rango del coseno del ángulo
  G4double cosMin = std::cos(8.0 * deg); // cos(4 grados)
  G4double cosMax = 1.0;                 // cos(0 grados)

  // Generar un número aleatorio entre cosMin y cosMax
  G4double randomNumber = cosMin + (cosMax - cosMin) * G4UniformRand();

  // Calcular el ángulo correspondiente usando arccos (en radianes)
  G4double theta = std::acos(randomNumber);

  // Generar un ángulo phi aleatorio entre 0 y 360 grados
  G4double phi = G4UniformRand() * 360.0 * deg;

  // Convertir coordenadas esféricas a cartesianas
  G4double ux = std::sin(theta) * std::cos(phi);
  G4double uy = std::sin(theta) * std::sin(phi);
  G4double uz = std::cos(theta);

  // Establecer la dirección del momento de la partícula
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

  // Generate the primary vertex
  fParticleGun->GeneratePrimaryVertex(anEvent);

  /*
  // Ángulo del cono
  G4double theta = std::atan((12 * cm) / (80 * cm)); // Ángulo para cubrir 12 cm a 80 cm de distancia

  // Generar dirección dentro del cono
  G4double phi = 2. * 3.14159265358979323846 * G4UniformRand(); // Ángulo azimutal aleatorio
  G4double costheta = std::cos(theta * G4UniformRand()); // Ángulo polar aleatorio dentro del cono
  G4double sintheta = std::sqrt(1. - costheta * costheta);

  G4double ux = sintheta * std::cos(phi);
  G4double uy = sintheta * std::sin(phi);
  G4double uz = costheta;

  G4ThreeVector direction(ux, uy, uz);
  fParticleGun->SetParticleMomentumDirection(direction);

  // Generar el evento
  fParticleGun->GeneratePrimaryVertex(anEvent);
  */


  // Set gun position
  //fParticleGun
    //->SetParticlePosition(G4ThreeVector(0., 0., -0.4*cm));
  
  //fParticleGun->GeneratePrimaryVertex(anEvent);


  //
  //AHORA
  //
  //if (!trackerTube) {
    //  G4ExceptionDescription msg;
      //msg << "Tracker volume of G4Tubs shape not found." << G4endl;
      //msg << "The gun will be placed in the center.";
      //G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
        //  "MyCode0003", JustWarning, msg);
      //fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
  //}
  //else {
      // Get the half length of the cylinder along the Z axis
    //  G4double hz = trackerTube->GetZHalfLength();
      // Set gun position 1 cm in front of the mouth of the cylinder
      //G4double zPosition = hz - 10.0 * cm;
      //fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -zPosition));
  //}

  //fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
