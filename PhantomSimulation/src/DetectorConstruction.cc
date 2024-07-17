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
/// \file B4/B4a/src/DetectorConstruction.cc
/// \brief Implementation of the B4::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh" //Este lo he puesto yo
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh" //Este lo he añadido yo
#include "G4Transform3D.hh" //Este lo he añadido yo
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SubtractionSolid.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_B"); 
  nistManager->FindOrBuildMaterial("G4_AIR"); 
  nistManager->FindOrBuildMaterial("G4_Ge"); 
  nistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE"); 
  nistManager->FindOrBuildMaterial("G4_CONCRETE"); 
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildElement("G4_C");
  nistManager->FindOrBuildElement("G4_O");
  nistManager->FindOrBuildElement("G4_H");
  nistManager->FindOrBuildMaterial("G4_F"); 

  auto H = G4Element::GetElement("H");
  auto C = G4Element::GetElement("C");
  auto O = G4Element::GetElement("O");
  auto F = G4Element::GetElement("F");


 

  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;
  //new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
  

  //Definir PLA 
  density = 1.24 * g / cm3;
  G4Material* PLA = new G4Material("PLA", density, 3); // 3 es el número de elementos en el compuesto 

  PLA->AddElement(C, 3); // Proporción de átomos de carbono en el PLA 
  PLA->AddElement(H, 4); // Proporción de átomos de hidrógeno en el PLA 
  PLA->AddElement(O, 2); // Proporción de átomos de oxígeno en el PLA


  //Definir TEFLÓN
  density = 2.2 * g / cm3;
  G4Material* teflon = new G4Material("teflon", density, 2);
  teflon->AddElement(C, 2);
  teflon->AddElement(F, 4);


  //Definir POLIETILENO
  density = 0.95 * g / cm3;
  G4Material* polietileno = new G4Material("polietileno", density, 2);
  polietileno->AddElement(C, 2);
  polietileno->AddElement(H, 4);


  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_Ge");
  auto detectorMaterial = G4Material::GetMaterial("G4_SODIUM_IODIDE");
  auto air = G4Material::GetMaterial("G4_AIR");
  auto hormigon = G4Material::GetMaterial("G4_CONCRETE");
  auto iron = G4Material::GetMaterial("G4_Fe");
  auto plomo = G4Material::GetMaterial("G4_Pb");
  auto PLA = G4Material::GetMaterial("PLA");
  auto teflon = G4Material::GetMaterial("teflon");
  auto polietileno = G4Material::GetMaterial("polietileno");


  if ( ! defaultMaterial || ! absorberMaterial || ! detectorMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
    

  //
  // World
  //
  G4double world_hx = 0.5*m; 
  G4double world_hy = 0.50*m;
  G4double world_hz = 0.50 * m;

  auto worldBox
      = new G4Box("World",           // its name
          world_hx, world_hy, world_hz); // its size

  auto worldLog
      = new G4LogicalVolume(
          worldBox,           // its solid
          defaultMaterial,  // its material
          "World");         // its name


  auto worldPhys = new G4PVPlacement(nullptr,  // no rotation
      G4ThreeVector(),                         // at (0,0,0)
      worldLog,                                 // its logical volume
      "World",                                 // its name
      nullptr,                                 // its mother  volume
      false,                                   // no boolean operation
      0,                                       // copy number
      fCheckOverlaps);                         // checking overlaps



  /*
  
  //Defino bloque de polietileno de 5 cm de espesor
  G4double pol_hx = 6. * cm;
  G4double pol_hy = 6. * cm;
  G4double pol_hz = 2.5 * cm;

  auto polietilenoBox
      = new G4Box("PolietilenoBox",
          pol_hx, pol_hy, pol_hz);

  auto polietilenoLV
      = new G4LogicalVolume(
          polietilenoBox,
          polietileno,
          "PolietilenoLV");


  G4double polPos_x = 0.0 * m;
  G4double polPos_y = 0.0 * m;
  G4double polPos_z = -30 * cm;

  
  auto polietilenoPV
      = new G4PVPlacement(0,                       // no rotation
          G4ThreeVector(polPos_x, polPos_y, polPos_z),
          // translation position
          polietilenoLV,              // its logical volume
          "PolietilenoPV",               // its name
          worldLog,                // its mother (logical) volume
          false,                   // no boolean operations
          0);                      // its copy number




  //Creo el volumen del colimador 

  G4double radioInicial = 2.5 * cm;
  G4double RadioFinal = 6.0 * cm;
  G4double altura = 1.75 * cm;
  G4double anguloInicial = 0. * deg;
  G4double AnguloFinal = 360. * deg;


  //Solido:
  auto colimador
      = new G4Tubs("colimador",
          radioInicial,
          RadioFinal,
          altura,
          anguloInicial,
          AnguloFinal);

  //Ahora creamos el volumen logico:
  // 
  auto colimadorLV
      = new G4LogicalVolume(colimador, polietileno, "ColimadorLV");


  // Creamos el volumen fisico
  // 
  G4double xinit = 0.0 * m;
  G4double yinit = 0.0 * m;
  G4double zinit = -25.5 * cm;
    

  auto colimadorPV = new G4PVPlacement(0,  // no rotation 
      G4ThreeVector(xinit, yinit, zinit),          // at (0,0,0)
      colimadorLV,                  // its logical volume
      "ColimadorPV",            // its name
      worldLog,                  // its mother  volume
      false,                    // no boolean operation
      0,                        // copy number
      fCheckOverlaps);          // checking overlaps





  //Hay una caja de 20 x 20 cm de polietileno que recubre los volúmenes del colimador y de la placa moderadora

  G4double pebox_hx = 2.5 * cm;
  G4double pebox_hy = 6.0 * cm;
  G4double pebox_hz = 4.5 * cm;

  auto polietilenoBigBox
      = new G4Box("PolietilenoBigBox",
          pebox_hx, pebox_hy, pebox_hz);

  auto polietilenoBigLV
      = new G4LogicalVolume(
          polietilenoBigBox,
          polietileno,
          "PolietilenoBigLV");

  G4double pePos_x = 8.5 * cm;
  G4double pePos_y = 0.0 * m;
  G4double pePos_z = -28 * cm;


  auto polietilenoBigPV
      = new G4PVPlacement(0,                       // no rotation
          G4ThreeVector(pePos_x, pePos_y, pePos_z),
          // translation position
          polietilenoBigLV,              // its logical volume
          "PolietilenoBigPV",               // its name
          worldLog,                // its mother (logical) volume
          false,                   // no boolean operations
          0);                      // its copy number

  
  //Creo una caja para el otro lateral
  G4double innerBox_hx = 2.5 * cm;
  G4double innerBox_hy = 6.0 * cm;
  G4double innerBox_hz = 4.5 * cm;

  auto solidInnerBox = new G4Box("solidInnerBox",
      innerBox_hx, innerBox_hy, innerBox_hz);

  auto solidInnerBoxLV
      = new G4LogicalVolume(
          solidInnerBox,
          polietileno,
          "solidInnerBoxLV");

  auto solidInnerBoxPV
      = new G4PVPlacement(0,
          G4ThreeVector(-85., 0.0, -280),
          solidInnerBoxLV,
          "solidInnerBoxPV",
          worldLog,
          false,
          0);


  //Creo una caja para el "techo"
  G4double upperBox_hx = 11.0 * cm;
  G4double upperBox_hy = 2.5 * cm;
  G4double upperBox_hz = 4.5 * cm;

  auto solidUpperBox = new G4Box("solidUpperBox",
      upperBox_hx, upperBox_hy, upperBox_hz);

  auto solidUpperBoxLV
      = new G4LogicalVolume(
          solidUpperBox,
          polietileno, 
          "solidUpperBoxLV");

  auto solidUpperBoxPV
      = new G4PVPlacement(0,
          G4ThreeVector(0.0, 85.0, -280.0),
          solidUpperBoxLV,
          "solidUpperBoxPV",
          worldLog,
          false,
          0);
  




  */





  //Creo la caje de plomo de 20.0 cm x 20.  cm x 2.5 cm:
  G4double plomo_hx = 6. * cm;
  G4double plomo_hy = 6. * cm;
  G4double plomo_hz = 1.25 * cm;

  auto plomoBox
      = new G4Box("Plomo",           // its name
          plomo_hx, plomo_hy, plomo_hz); // its size



  //Voy a crear una caja para recortar el bloque de plomo
  G4double airBox_hx = 3.0 * cm;
  G4double airBox_hy = 2.00 * cm;
  G4double airBox_hz = plomo_hz + 1*cm;

  auto airBox
      = new G4Box("AirBox",
          airBox_hx, airBox_hy, airBox_hz);

  
  //Ahora hacemos el recorte del volumen de plomo con esta caja de aire:

  G4SubtractionSolid* solidPlomoWithHole = new G4SubtractionSolid("PlomoWithHole",
      plomoBox,
      airBox,
      0,
      G4ThreeVector(plomo_hx-airBox_hx, 0.0, 0.0));


  G4double plomoPosX = 0.0 * cm;//+5.0 * cm;
  G4double plomoPosY = 0.0 * cm;
  G4double plomoPosZ = 0.0;  // Ajustar según las dimensiones del plomo y el phantom 


  auto plomoLV = new G4LogicalVolume(solidPlomoWithHole, plomo, "plomoLV");

  G4PVPlacement* physPlomo = new G4PVPlacement(0,                              // Sin rotación 
      G4ThreeVector(plomoPosX, plomoPosY, plomoPosZ), // Posición detrás del plomo 
      plomoLV,                      // Volumen lógico del phantom 
      "plomoPV",                    // Nombre del volumen físico del phantom 
      worldLog,                       // Volumen madre (world) 
      false,                          // No usar operación booleana 
      0,                              // Número de copia
      fCheckOverlaps);                // Chequear superposiciones




  
  //Creo el PHANTOM de PLA:

    //Creo la caje de plomo de 10.0 cm x 5.0  cm x 1.0 cm:
  G4double phantom_hx = 1.5 * cm;
  G4double phantom_hy = 3.0 * cm;
  G4double phantom_hz = 0.75 * cm;

  auto phantomBox
      = new G4Box("Phantom",           // its name
          phantom_hx, phantom_hy, phantom_hz); // its size



  //Ahora CIRCUNFERENCIA DE AIRE DENTRO DEL PHANTOM
  G4double initRadius = 0.0 * m;
  G4double finRadius = 0.50 * cm;
  G4double high = 0.5 * cm;
  G4double initAngle = 0. * deg;
  G4double finAngle = 360. * deg;


  //Solido:
  auto AirTube
      = new G4Tubs("AirTube",
          initRadius,
          finRadius,
          high,
          initAngle,
          finAngle);


  //Creo OTRA CIRCUNFERENCIA DE AIRE MÁS PEQUEÑA
  G4double smallTubeInnerRadius = 0.0 * cm; // Radio interno
  G4double smallTubeOuterRadius = 0.25 * cm; // Radio externo
  G4double smallTubeHeight = 1.0 * cm;      // Altura
  G4double smallTubeStartAngle = 0.0 * deg; // Ángulo inicial
  G4double smallTubeSpanningAngle = 360.0 * deg; // Ángulo total (360 grados para un tubo completo)

  auto smallTube = new G4Tubs("SmallTube",
      smallTubeInnerRadius,
      smallTubeOuterRadius,
      smallTubeHeight / 2.0, // Dividir por 2 porque G4Tubs usa la mitad de la altura
      smallTubeStartAngle,
      smallTubeSpanningAngle);



  //Creo OTRA CIRCUNFERENCIA MÁS PEQUEÑA
  G4double smallerTubeInnerRadius = 0.0 * cm; // Radio interno
  G4double smallerTubeOuterRadius = 0.125 * cm; // Radio externo
  G4double smallerTubeHeight = 1.0 * cm;      // Altura
  G4double smallerTubeStartAngle = 0.0 * deg; // Ángulo inicial
  G4double smallerTubeSpanningAngle = 360.0 * deg; // Ángulo total (360 grados para un tubo completo)

  auto smallerTube = new G4Tubs("SmallerTube",
      smallerTubeInnerRadius,
      smallerTubeOuterRadius,
      smallerTubeHeight / 2.0, // Dividir por 2 porque G4Tubs usa la mitad de la altura 
      smallerTubeStartAngle, 
      smallerTubeSpanningAngle);



  // Posición del phantom detrás del plomo
  G4double phantomPosX = +5.0 * cm; 
  G4double phantomPosY = 0.0 * cm; 
  G4double phantomPosZ = (plomo_hz + phantom_hz);  // Ajustar según las dimensiones del plomo y el phantom 

  // Crear la operación booleana de sustracción: PLA - AirTube
  G4SubtractionSolid* solidPhantomWithHole = new G4SubtractionSolid("PhantomWithHole", // Nombre del nuevo sólido
      phantomBox,        // Sólido inicial del phantom (PLA)
      AirTube,      // Sólido a sustraer (cilindro de aire)
      0,                 // Rotación relativa
      G4ThreeVector(0, 17.00, 0)); // Traslación relativa

  G4SubtractionSolid* solidPhantomWith2Holes = new G4SubtractionSolid("PhantomWith2Holes",
      solidPhantomWithHole,
      smallTube,
      0,
      G4ThreeVector(0, 0.0, 0.0));




  G4SubtractionSolid* solidPhantomWith3Holes = new G4SubtractionSolid("PhantomWith3Holes",
      solidPhantomWith2Holes,
      smallerTube,
      0,
      G4ThreeVector(0, -17.0, 0.0));



  G4LogicalVolume* phantomLV = new G4LogicalVolume(solidPhantomWith3Holes, PLA, "phantomLV");


  G4PVPlacement* physPhantom = new G4PVPlacement(0,                             // Sin rotación 
      G4ThreeVector(0, phantomPosY, phantomPosZ), // Posición detrás del plomo 
      phantomLV,                      // Volumen lógico del phantom 
      "PhantomPV",                    // Nombre del volumen físico del phantom 
      worldLog,                       // Volumen madre (world) 
      false,                          // No usar operación booleana 
      0,                              // Número de copia
      fCheckOverlaps);                // Chequear superposiciones






  //Creo OTRO PHANTOM de TEFLÓN:

  G4double phantom2_hx = 1.5 * cm;
  G4double phantom2_hy = 3.0 * cm;
  G4double phantom2_hz = 0.75 * cm;

  auto phantom2Box
      = new G4Box("Phantom2",           // its name
          phantom2_hx, phantom2_hy, phantom2_hz); // its size


  //Ahora recortamos unos rectángulos de aire:

  G4double air_hx = 0.5 * cm;
  G4double air_hy = 0.25 * cm;
  G4double air_hz = phantom2_hz;

  auto airRectangle
      = new G4Box("Air",
          air_hx, air_hy, air_hz);



  //Ahora hacemos el recorte del volumen de plomo con esta caja de aire:

  G4SubtractionSolid* solidPhantom2WithHole = new G4SubtractionSolid("Phantom2WithHole",
      phantom2Box,
      airRectangle,
      0,
      G4ThreeVector(0.0, 15.50, 0.0));




  //Ahora recortamos OTRO rectángulo de aire:

  G4double air2_hx = 0.25 * cm;
  G4double air2_hy = 0.125 * cm;
  G4double air2_hz = phantom2_hz;

  auto airRectangle2
      = new G4Box("AirRectangle",
          air2_hx, air2_hy, air2_hz);



  //Ahora hacemos el recorte del volumen de plomo con esta caja de aire:

  G4SubtractionSolid* solidPhantom2With2Holes = new G4SubtractionSolid("Phantom2With2Holes",
      solidPhantom2WithHole,
      airRectangle2,
      0,
      G4ThreeVector(0.0, 0.0, 0.0));



  //Ahora recortamos OTRO rectángulo de aire:

  G4double air3_hx = 0.125 * cm;
  G4double air3_hy = 0.05 * cm;
  G4double air3_hz = phantom2_hz;

  auto airRectangle3
      = new G4Box("AirRectangle3",
          air3_hx, air3_hy, air3_hz);




  //Ahora hacemos el recorte del volumen de plomo con esta caja de aire:

  G4SubtractionSolid* solidPhantom2With3Holes = new G4SubtractionSolid("Phantom2With3Holes",
      solidPhantom2With2Holes,
      airRectangle3,
      0,
      G4ThreeVector(0.0, -15.50, 0.0));


  G4double phantom4PosX = -3.5 * cm;//+5.0 * cm;
  G4double phantom4PosY = 0.0 * cm;
  G4double phantom4PosZ = (plomo_hz + phantom2_hz);  // Ajustar según las dimensiones del plomo y el phantom 

  auto phantom2LV = new G4LogicalVolume(solidPhantom2With3Holes, teflon, "phantom2LV");

  G4PVPlacement* physPhantom4 = new G4PVPlacement(0,                              // Sin rotación 
      G4ThreeVector(phantom4PosX, phantom4PosY, phantom4PosZ), // Posición detrás del plomo 
      phantom2LV,                      // Volumen lógico del phantom 
      "phantomPV",                    // Nombre del volumen físico del phantom 
      worldLog,                       // Volumen madre (world) 
      false,                          // No usar operación booleana 
      0,                              // Número de copia
      fCheckOverlaps);                // Chequear superposiciones














  // Creo el detector:
  // 
  G4double detector_hx = 6.0 * cm;
  G4double detector_hy = 6.0 * cm;
  G4double detector_hz = 0.1 * m;

  G4Box* detectorBox = new G4Box("Detector", detector_hx, detector_hy, detector_hz);
  G4LogicalVolume* detectorLog
      = new G4LogicalVolume(detectorBox, detectorMaterial, "Detector");


  //Coloco el detector justo detrás de el cilindro
  G4double detectorPos_x = 0.0 * cm;
  G4double detectorPos_y = 0.0 * cm;
  G4double detectorPos_z = phantom4PosZ+11.0 * cm;

  //Volumen fisico del detector:
  //
  fDetectorPhys
      = new G4PVPlacement(0,                       // no rotation
          G4ThreeVector(detectorPos_x, detectorPos_y, detectorPos_z),
          // translation position
          detectorLog,              // its logical volume
          "Detector",               // its name
          worldLog,                // its mother (logical) volume
          false,                   // no boolean operations
          0);                      // its copy number











  worldLog->SetVisAttributes(G4VisAttributes::GetInvisible()); 
  //detectorLog->SetVisAttributes(G4VisAttributes::GetInvisible());
  //phantom2LV->SetVisAttributes(G4VisAttributes::GetInvisible());
  //phantomLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  //plomoLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  /*
    // Visualization attributes
    G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); // Red
    worldVisAtt->SetVisibility(true);
    worldLog->SetVisAttributes(worldVisAtt);
    */
    
    G4VisAttributes* PbVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Green
    PbVisAtt->SetVisibility(true);
    plomoLV->SetVisAttributes(PbVisAtt);

    
    G4VisAttributes* PhantomVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); // Blue
    PhantomVisAtt->SetVisibility(true); 
    phantomLV->SetVisAttributes(PhantomVisAtt);

    
    G4VisAttributes* Phantom2VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); //Red
    Phantom2VisAtt->SetVisibility(true);
    phantom2LV->SetVisAttributes(Phantom2VisAtt);
    
    G4VisAttributes* DetectorVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0)); // Azul claritoB
    DetectorVisAtt->SetVisibility(true);
    detectorLog->SetVisAttributes(DetectorVisAtt);

    /*
    //"Blindajes" de PE
    G4VisAttributes* polietilenoBigVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0)); // Azul claritoB
    polietilenoBigVisAtt->SetVisibility(true);
    polietilenoBigLV->SetVisAttributes(polietilenoBigVisAtt);

    G4VisAttributes* solidInnerBoxVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0)); // Azul claritoB
    solidInnerBoxVisAtt->SetVisibility(true);
    solidInnerBoxLV->SetVisAttributes(solidInnerBoxVisAtt);

    G4VisAttributes* solidUpperBoxVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0)); // Azul claritoB
    solidUpperBoxVisAtt->SetVisibility(true);
    solidUpperBoxLV->SetVisAttributes(solidUpperBoxVisAtt);

    G4VisAttributes* polietilenoVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)); // Azul claritoB
    polietilenoVisAtt->SetVisibility(true);
    polietilenoLV->SetVisAttributes(polietilenoVisAtt);

    G4VisAttributes* colimadorVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)); // Azul claritoB
    colimadorVisAtt->SetVisibility(true);
    colimadorLV->SetVisAttributes(colimadorVisAtt);
    */

  //
  // Always return the physical World
  //
   return worldPhys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}




