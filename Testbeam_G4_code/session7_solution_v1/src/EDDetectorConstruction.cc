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
// $Id$
//
/// \file EDDetectorConstruction.cc
/// \brief Implementation of the EDDetectorConstruction class

#include "EDDetectorConstruction.hh"

EDDetectorConstruction::EDDetectorConstruction()
  : G4VUserDetectorConstruction()
{ }

EDDetectorConstruction::~EDDetectorConstruction()
{ }

G4VPhysicalVolume* EDDetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4bool fromIsotopes = false;  
  // nistManager->FindOrBuildMaterial( "G4_Fe", fromIsotopes );
  // nistManager->FindOrBuildMaterial( "G4_POLYSTYRENE", fromIsotopes );
  // nistManager->FindOrBuildMaterial( "G4_He", fromIsotopes );
  // nistManager->FindOrBuildMaterial( "G4_AIR", fromIsotopes );

  // Build materials
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR",false);
  // There is no need to test if materials were built/found
  // as G4NistManager would issue an error otherwise
  // Try the code with "XYZ".      
  
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double hx = 0.2*m;
  G4double hy = 0.1*m;
  G4double hz = 0.4*m;
  
  // world volume
  G4Box* worldS = new G4Box("World", hx, hy, hz); 
      
  G4LogicalVolume* worldLog                         
    = new G4LogicalVolume(worldS, air, "worldLog");
                                   
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                     //no rotation
			G4ThreeVector(),       //at (0,0,0)
			worldLog,               //its logical volume
			"worldLog",               //its name
			0,                     //its mother  volume
			false,                 //no boolean operation
			0,                     //copy number
			checkOverlaps);        //overlaps checking

  G4int nel,natoms;
  std::cout << "===========================" << std::endl;
  G4Material* silicon =  nistManager->FindOrBuildMaterial("G4_Si",false);
  G4Material* Tungsten = nistManager->FindOrBuildMaterial("G4_W");

  //vacuum 
  G4double atomicNumber = 1.;
  G4double massOfMole = 1.008*g/mole;
  G4double density = 1.e-25*g/cm3;
  G4double temperature = 2.73*kelvin;
  G4double pressure = 3.e-18*pascal;
  G4Material* Vacuum = new G4Material("interGalactic", atomicNumber,massOfMole, density, kStateGas, temperature, pressure);
  
  //element
  G4double fractionmass;
  G4NistManager *man = G4NistManager::Instance();
  G4Element *elH = man->FindOrBuildElement("H");
  G4Element *elC = man->FindOrBuildElement("C");
  G4Element *elN = man->FindOrBuildElement("N");
  G4Element *elO = man->FindOrBuildElement("O");
  G4Element* elementH  = new G4Element("Hydrogen",  "H",  1.,  1.0079*g/mole);
  G4Element* elementC  = new G4Element("Carbon",    "C",  6.,  12.011*g/mole);

  //sci_BC-408
  G4double d_PolyVinylToluene = 0.57*g/cm3;
  G4double d_Anthracene = 1.28*g/cm3;
  G4double d_BBHodo_Scinti = 1.023*g/cm3;
  G4Material* PolyVinylToluene = new G4Material( "PolyVinylToluene", d_PolyVinylToluene, 2 );
  PolyVinylToluene->AddElement(elC, fractionmass = 0.91471);
  PolyVinylToluene->AddElement(elH, fractionmass = 0.08529);
  
  G4Material* Anthracene = new G4Material( "Anthracene", d_Anthracene, 2 );
  Anthracene->AddElement(elC, fractionmass = 0.943447);
  Anthracene->AddElement(elH, fractionmass = 0.056553);
  
  G4Material *BBHodo_Scinti = new G4Material( "BBHodo_Scinti", d_BBHodo_Scinti,  2 );
  BBHodo_Scinti->AddMaterial(PolyVinylToluene, fractionmass = 0.36);
  BBHodo_Scinti->AddMaterial(Anthracene, fractionmass = 0.64);

  //kapton
  density = 1.42*g/cm3;
  G4Material* Kapton = new G4Material("Kapton",density, nel=4);
  Kapton->AddElement(elH, fractionmass = 0.0264);
  Kapton->AddElement(elC, fractionmass = 0.6911);
  Kapton->AddElement(elN, fractionmass = 0.0733);
  Kapton->AddElement(elO, fractionmass = 0.2092);

  //Copper
  density = 8.96 * g / cm3;
  G4double a = 63.54 * g / mole;
  G4Material* Copper = new G4Material("Copper", 29.0, a, density);

  // Glue for the carbon fiber
  // actually, it's the same as one used in the sPHENIX Geant simulation
  G4Material *Epoxy = new G4Material("Epoxy", 1.2 * g / cm3, 2);
  Epoxy->AddElement(G4Element::GetElement("H"), natoms = 2);
  Epoxy->AddElement(G4Element::GetElement("C"), natoms = 2);
  
  //carbon fiber
  density = 0.145*g/cm3;
  G4Material* CFRP = new G4Material("CFRP",density, nel=1);
  CFRP->AddElement(elC,1);

  // Foam: Polystyrene-based
  G4Material* foam = new G4Material("foam",0.05*g/cm3, 2);
  foam->AddElement(elementC, 0.90);
  foam->AddElement(elementH, 0.10);

  //water
  density = 1.000*g/cm3;
  G4Material* water = new G4Material("water",density,2);
  water->AddElement(elH, natoms=2);
  water->AddElement(elO, natoms=1);


  //======================G4solid===============================================  
  const int kLadder_num = 4;
  const bool kIs_silicon_off = false; // flag for debugging, turning all silicon strips off to save time
  
  //G4LogicalVolume* INTT_siLV[kLadder_num];
  vector < G4String > INTT_siLV_name, INTT_HDI_copperLV_name, INTT_HDI_KaptonLV_name,
    INTT_CFRPLVf_name, INTT_foam_LVup_name, INTT_foam_LVdown_name,
    INTT_CFRP_tubeLV_name, INTT_waterLV_name, INTT_CFRPLVr_name,
    INTT_CFRP_glueLV_name;
  for( int i=0; i<kLadder_num; i++ )
    {
      INTT_siLV_name.push_back		( G4String("INTT_siLV")			+ to_string(i+1) );
      INTT_HDI_copperLV_name.push_back	( G4String("INTT_HDI_copperLV")		+ to_string(i+1) );
      INTT_HDI_KaptonLV_name.push_back	( G4String("INTT_HDI_KaptonLV")		+ to_string(i+1) );
      INTT_CFRPLVf_name.push_back	( G4String("INTT_carbonfiberLVf")	+ to_string(i+1) );
      INTT_foam_LVup_name.push_back	( G4String("INTT_foam_LVup")		+ to_string(i+1) );
      INTT_foam_LVdown_name.push_back	( G4String("INTT_foam_LVdown")		+ to_string(i+1) );
      INTT_CFRP_glueLV_name.push_back	( G4String("INTT_carbonfiber_glueLV")	+ to_string(i+1) );
      INTT_CFRP_tubeLV_name.push_back	( G4String("INTT_carbonfiber_tubeLV")	+ to_string(i+1) );
      INTT_waterLV_name.push_back	( G4String("INTT_waterLV")		+ to_string(i+1) );
      INTT_CFRPLVr_name.push_back	( G4String("INTT_carbonfiberLVr")	+ to_string(i+1) );
    }
  
  vector < G4LogicalVolume* > INTT_siLV_outer( kLadder_num );
  vector < G4LogicalVolume* > INTT_HDI_copperLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_HDI_KaptonLV( kLadder_num );

  vector < G4LogicalVolume* > INTT_stave_areaLV( kLadder_num );  
  vector < G4LogicalVolume* > INTT_CFRPLVf( kLadder_num );
  vector < G4LogicalVolume* > INTT_foam_LV( kLadder_num );
  vector < G4LogicalVolume* > INTT_CFRP_tube_areaLV( kLadder_num );  
  vector < G4LogicalVolume* > INTT_CFRP_glueLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_CFRP_tubeLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_waterLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_CFRPLVr( kLadder_num ); // to be deleted
  vector < G4LogicalVolume* > INTT_formed_CFRPLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_formed_CFRP_straightLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_formed_CFRP_outer_partLV( kLadder_num );
  vector < G4LogicalVolume* > INTT_formed_CFRP_slant_partLV( kLadder_num );

  const G4double kLadder_horizontal_length = 232.2 * mm;
  const G4double kLadder_vertical_length =  38 * mm;
  //  1.47684 cm

  G4double si_thickness = 78 * um;
  G4Box* INTT_si_box		= new G4Box("INTT_si_box", 0.3 * m, 0.3 * m , 0.3 * m);
  G4Box* INTT_si		= new G4Box("INTT_si", 116.1 * mm, 11.25 * mm , 0.16 * mm);
  G4Box* INTT_si_test		= new G4Box("INTT_si_test", 116.1 * mm, 5.625 * mm , 0.16 * mm);
  
  G4Box* INTT_si_typeA		= new G4Box("INTT_si_typeA", 8. * mm, 0.039 * mm , 0.16 * mm);
  G4Box* INTT_si_typeB		= new G4Box("INTT_si_typeB", 10. * mm, 0.039 * mm , 0.16 * mm);
  
  G4Box* INTT_sci		= new G4Box("INTT_sci", 116.1 * mm, 11.25 * mm , 2.5 * mm);
  
  G4Box* INTT_Chip		= new G4Box("INTT_Chip", 4.5 * mm, 1.5 * mm, 0.16 * mm); 
  G4Box* INTT_Chip_glue		= new G4Box("INTT_Chip_glue", 4.5 * mm, 1.5 * mm, 50 * um); 
  G4Box* INTT_Chip_area		= new G4Box("INTT_Chip_area", 4.5 * mm, 1.5 * mm, 0.16 * mm + 50 * um); 
    
  //G4VSolid *siinactive_box = new G4SubtractionSolid((boost::format("siinactive_box_%d_%d") % inttlayer % itype).str(),
  G4double INTT_CFRP_thickness = 300 * um;

  //sifull_box, siactive_box, 0, G4ThreeVector(0, 0, 0));
  G4double INTT_CFRP_tube_inner_radius = 1.5 * mm;
  G4double INTT_CFRP_tube_outer_radius =  INTT_CFRP_tube_inner_radius + INTT_CFRP_thickness;
  G4double INTT_CFRP_tube_area_thickness = INTT_CFRP_tube_outer_radius * 2;

  G4Box* INTT_CFRP	= new G4Box("INTT_carbonfiber",
					    kLadder_horizontal_length / 2,
					    //17.95 * mm,
					    kLadder_vertical_length / 2,
					    INTT_CFRP_thickness / 2);
  // 0.33227 cm
  G4double INTT_formed_CFRP_outer_length = 3.3227 * mm;
  G4double INTT_formed_CFRP_straight_length = 14.7684 * mm;
  G4double INTT_formed_CFRP_thickness = INTT_CFRP_thickness;
  G4double INTT_formed_CFRP_block_thickness = INTT_CFRP_tube_outer_radius * 2  + INTT_formed_CFRP_thickness;
  G4Box* INTT_formed_CFRP_straight = new G4Box("INTT_formed_carbonfiber_straight",
					       kLadder_horizontal_length / 2,
					       INTT_formed_CFRP_straight_length / 2,
					       INTT_formed_CFRP_thickness / 2 );
  
  G4Box* INTT_formed_CFRP_outer_part = new G4Box("INTT_formed_carbonfiber_outer_part",
					       kLadder_horizontal_length / 2,
					       INTT_formed_CFRP_outer_length / 2,
					       INTT_formed_CFRP_thickness / 2 );
  
  G4double trap_y_length = kLadder_vertical_length - 2 * INTT_formed_CFRP_outer_length ;
  vector < G4TwoVector > form_trap_vectors;
  for( int i=0; i<2; i++ ) // for 8 vertices
    {
      form_trap_vectors.push_back( G4TwoVector(0. * cm, -trap_y_length / 2) );
      form_trap_vectors.push_back( G4TwoVector(0. * cm,  trap_y_length / 2) );
      form_trap_vectors.push_back( G4TwoVector( INTT_CFRP_tube_outer_radius * 2, INTT_formed_CFRP_straight_length / 2 ) );
      form_trap_vectors.push_back( G4TwoVector( INTT_CFRP_tube_outer_radius * 2, -INTT_formed_CFRP_straight_length / 2 ) );

    }
  
  // 	G4GenericTrap (const G4String &name, G4double halfZ, const std::vector< G4TwoVector > &vertices)  
  auto INTT_foam = new G4GenericTrap( "INTT_form", kLadder_horizontal_length / 2, form_trap_vectors );
  G4RotationMatrix* foam_rotation = new G4RotationMatrix();
  foam_rotation->rotateY( 90.0 * deg );
  
  vector < G4TwoVector > formed_CFRP_slant_vectors;
  for( int i=0; i<2; i++ ) // for 8 vertices
    {
      formed_CFRP_slant_vectors.push_back( G4TwoVector(0.0, trap_y_length / 2 - INTT_formed_CFRP_straight_length / 2) );
      formed_CFRP_slant_vectors.push_back( G4TwoVector( INTT_formed_CFRP_thickness, trap_y_length / 2  - INTT_formed_CFRP_straight_length / 2) );

      formed_CFRP_slant_vectors.push_back( G4TwoVector( INTT_CFRP_tube_outer_radius * 2 + INTT_formed_CFRP_thickness, 0.0  ) );
      formed_CFRP_slant_vectors.push_back( G4TwoVector( INTT_CFRP_tube_outer_radius * 2, 0.0  ) );

    }
  
  auto INTT_formed_CFRP_slant = new G4GenericTrap( "INTT_formed_CFRP_slant", kLadder_horizontal_length / 2, formed_CFRP_slant_vectors );
    
  G4Box* INTT_formed_CFRP = new G4Box("INTT_formed_carbonfiber",
					     kLadder_horizontal_length / 2,
					     INTT_formed_CFRP_straight_length / 2,
					     INTT_formed_CFRP_thickness / 2);

  // for mother volue of the carbon fiber tune and the cooling water
  //G4double INTT_CFRP_tube_area_thickness = 4.32 * mm;
  G4Box* INTT_CFRP_tube_area = new G4Box( "INTT_carbonfiber_glue_solid",
					  kLadder_horizontal_length / 2,
					  INTT_CFRP_tube_area_thickness / 2,
					  INTT_CFRP_tube_area_thickness / 2
					  );
  
  G4Tubs* INTT_CFRP_tube	= new G4Tubs("INTT_carbonfiber_tube",
					     INTT_CFRP_tube_inner_radius,
					     INTT_CFRP_tube_outer_radius,
					     kLadder_horizontal_length/2, 0, 360 * degree);
  
  G4Tubs* INTT_water		= new G4Tubs("INTT_water",
					     0*mm, INTT_CFRP_tube_inner_radius,
					     kLadder_horizontal_length/2, 0, 360*degree);

  G4double INTT_form_vertical_length = ( kLadder_vertical_length - 2 * INTT_CFRP_tube_outer_radius ) / 2;
  G4double INTT_form_thickness = INTT_CFRP_tube_outer_radius * 2; // radius -> diameter

  G4double INTT_stave_area_thickness = INTT_CFRP_thickness
    + INTT_formed_CFRP_thickness
    + INTT_CFRP_tube_area_thickness;
  
  G4Box* INTT_stave_area        = new G4Box( "INTT_stave_area", 
					     kLadder_horizontal_length / 2,
					     kLadder_vertical_length / 2,
					     INTT_stave_area_thickness / 2 );

  G4double HDI_copper_thickness = 37.6 * um;
  //  G4Box* INTT_HDI_copper	= new G4Box("INTT_HDI_copper", 116.1*mm, 19.*mm, 37.6 * um / 2);
  G4Box* INTT_HDI_copper	= new G4Box("INTT_HDI_copper",
					    kLadder_horizontal_length / 2,
					    kLadder_vertical_length / 2,
					    HDI_copper_thickness / 2);
  G4double HDI_kapton_thickness = 380 * um;
  G4Box* INTT_HDI_Kapton	= new G4Box("INTT_HDI_Kapton", 116.1*mm, 19.*mm, HDI_kapton_thickness / 2);

  G4Box* INTT_testbeam_BOX	= new G4Box("INTT_testbeam_BOX", 116.1*mm, 19.*mm, 170.1*mm);
  
  G4int counting_number = 0;

  //===============================================================================================================
  G4RotationMatrix* tubeRotation = new G4RotationMatrix();
  tubeRotation->rotateY( 90.0 * deg );

  G4RotationMatrix* testbeam_BOX_rotation_X = new G4RotationMatrix(); //theta angle
  testbeam_BOX_rotation_X->rotateX( 16.0 * deg );

  G4RotationMatrix* testbeam_BOX_rotation_Y = new G4RotationMatrix(); //phi angle
  testbeam_BOX_rotation_Y->rotateY( 90.0 * deg );

  //===============================================================================================================
  // G4LogicalVolume * INTT_si_boxLV = new G4LogicalVolume(INTT_si_box, air, "INTT_si_boxPV");
  // new G4PVPlacement(0, 
  //                   G4ThreeVector(0, 0, 0), 
  //                   INTT_si_boxLV,            //its logical volume
  //                   "INTT_si_boxPV",       //its name
  //                   worldLog,                //its mother  volume
  //                   false,                   //no boolean operation
  //                   0,                       //copy number
  //                   checkOverlaps);    

  G4LogicalVolume * INTT_testbeam_BOXLV = new G4LogicalVolume(INTT_testbeam_BOX, air, "INTT_testbeam_BOXLV");
  G4double zpos_testbeam_box = 108.85 * mm;
  //  G4double zpos_testbeam_box = 0;
  G4double zpos_testbeam_box_1 = 108.85 * mm;  
  //  G4double zpos_testbeam_box_1 = zpos_testbeam_box;
  //fmovement = 35;
  //G4double public_variable::theoffset = 35;
  //G4cout<<" theoffset test in EDD : "<<theoffset<<G4endl;

  G4VPhysicalVolume * INTT_testbeam_BOXPV =
    new G4PVPlacement(0, 
		      G4ThreeVector(0, 0, zpos_testbeam_box_1), 
		      INTT_testbeam_BOXLV,            //its logical volume
		      "INTT_testbeam_BOXLV",       //its name
		      worldLog,                //its mother  volume
		      false,                   //no boolean operation
		      0,                       //copy number
		      checkOverlaps);

  G4LogicalVolume* INTT_siLV_typeA
    = new G4LogicalVolume(INTT_si_typeA, silicon, "INTT_siLV_all_typeA");
  G4LogicalVolume* INTT_siLV_typeB
    = new G4LogicalVolume(INTT_si_typeB, silicon, "INTT_siLV_all_typeB");

  // G4LogicalVolume*INTT_siLV_outer
  //   = new G4LogicalVolume(INTT_si, silicon, "INTT_siLV_all_outer");

  // loop over all INTT ladders
  G4double initialposition = 100.0 * mm;
  for (G4int l=0; l<kLadder_num; l++ )
    {

      // if the debuggind flag is ture, skip silicon strips
      if( kIs_silicon_off == true )
	continue;
      
      G4double xpos, ypos;
      INTT_siLV_outer[l] = new G4LogicalVolume(INTT_si, silicon, INTT_siLV_name[l]);

      G4double gap = 35.0 * mm;

      //for silicon    
      G4double zpos = initialposition + l * gap;   
      G4VPhysicalVolume* INTT_siLV_outer_allPV =
	new G4PVPlacement(0, 
			  G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
			  INTT_siLV_outer[l],            //its logical volume
			  INTT_siLV_name[l],       //its name
			  INTT_testbeam_BOXLV,                //its mother  volume
			  false,                   //no boolean operation
			  0,                       //copy number
			  checkOverlaps);

      G4ThreeVector the_position;
      for (G4int l1=0; l1<256; l1++)
	{
       
	  if (l1<128)
	    {
	      ypos = ( -9.961 + (l1 * 0.078) ) * mm;
	      
	      for (G4int l2=0; l2<13; l2++)
		{
		  
		  if (l2<8)
		    {

		      xpos = ( -107.1 + (l2*16) ) * mm;
		      G4VPhysicalVolume * chip_channelsPV =
			new G4PVPlacement(0, 
					  G4ThreeVector(xpos, ypos, 0), 
					  INTT_siLV_typeA,            //its logical volume
					  "INTT_siLV_all_typeA",       //its name
					  INTT_siLV_outer[l],                //its mother  volume
					  false,                   //no boolean operation
					  counting_number,                       //copy number
					  false);
		      
		      the_position = INTT_testbeam_BOXPV->GetTranslation()
			+ INTT_siLV_outer_allPV->GetTranslation()
			+ chip_channelsPV->GetTranslation();
		      
		      G4cout << "copy test : " << counting_number
			     << " ID : " << l << " " << l1 << " " << l2
			     << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
			     << " up : " << 0
			     << " type : " << 0 << G4endl;
		      counting_number+=1;
                    
		    }
		  else 
		    {
		      xpos = ( 25.1 + ( (l2-8) * 20) ) * mm;
		      G4VPhysicalVolume * chip_channelsPV =
			new G4PVPlacement(0, 
					  G4ThreeVector(xpos, ypos, 0), 
					  INTT_siLV_typeB,            //its logical volume
					  "INTT_siLV_all_typeB",       //its name
					  INTT_siLV_outer[l],                //its mother  volume
					  false,                   //no boolean operation
					  counting_number,                       //copy number
					  false);
		      the_position = INTT_testbeam_BOXPV->GetTranslation()
			+ INTT_siLV_outer_allPV->GetTranslation()
			+ chip_channelsPV->GetTranslation();
		      G4cout << "copy test : " << counting_number
			     << " ID : " << l << " " << l1 << " " << l2
			     << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
			     << " up : " << 0
			     << " type : " << 1 << G4endl;
		      counting_number+=1;
		      
		    }  
		}
	    }
	  else
	    {
	      
	      ypos = ( 0.055 + ( (l1-128) * 0.078) ) * mm;
	      for (G4int l2=0; l2<13; l2++)
		{ 
		  if (l2<8)
		    {
		      
		      xpos = ( -107.1 + (l2 * 16) ) * mm;
		      G4VPhysicalVolume * chip_channelsPV =
			new G4PVPlacement(0, 
					  G4ThreeVector(xpos, ypos, 0), 
					  INTT_siLV_typeA,            //its logical volume
					  "INTT_siLV_all_typeA",       //its name
					  INTT_siLV_outer[l],                //its mother  volume
					  false,                   //no boolean operation
					  counting_number,                       //copy number
					  false);
		      
		      the_position = INTT_testbeam_BOXPV->GetTranslation()
			+ INTT_siLV_outer_allPV->GetTranslation()
			+ chip_channelsPV->GetTranslation();
		      G4cout << "copy test : " << counting_number
			     << " ID : " << l << " " << l1 << " " << l2
			     << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
			     << " up : " << 1
			     << " type : " << 0 << G4endl;
		      counting_number+=1;

		    }
		  else 
		    {

		      xpos = ( 25.1 + ( (l2-8) * 20) ) * mm;
		      G4VPhysicalVolume * chip_channelsPV =
			new G4PVPlacement(0, 
					  G4ThreeVector(xpos, ypos, 0), 
					  INTT_siLV_typeB,            //its logical volume
					  "INTT_siLV_all_typeB",       //its name
					  INTT_siLV_outer[l],                //its mother  volume
					  false,                   //no boolean operation
					  counting_number,                       //copy number
					  false);
		      
		      the_position = INTT_testbeam_BOXPV->GetTranslation()
			+ INTT_siLV_outer_allPV->GetTranslation()
			+ chip_channelsPV->GetTranslation();
		      G4cout << "copy test : " << counting_number
			     << " ID : " << l << " " << l1 << " " << l2
			     << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
			     << " up : " << 1
			     << " type : " << 1 << G4endl;
		      counting_number+=1;

		    }  
		}
	    }  
	}  
    }  // end block for for loop over all INTT ladders

  //  G4LogicalVolume*INTT_siLV
  //    = new G4LogicalVolume(INTT_si, silicon, "INTT_siLV_all");
  // for (G4int l=0;l<4;l++)
  // { 
  //  G4double gap = 35.*mm;
  //  G4double initialposition = 100.*mm;
  //  //for silicon
  //  G4double zpos = initialposition+l*gap;   
  //  new G4PVPlacement(0, 
  //                    G4ThreeVector(0, 0, zpos), 
  //                    INTT_siLV,            //its logical volume
  //                    "INTT_siLV_all",       //its name
  //                    INTT_si_boxLV,                //its mother  volume
  //                    false,                   //no boolean operation
  //                    l,                       //copy number
  //                    checkOverlaps);
  // }  // end block for for loop
  // G4LogicalVolume*INTT_siLV_inside
  //    = new G4LogicalVolume(INTT_si, silicon, "INTT_siLV_all_inside");
  //  new G4PVPlacement(0, 
  //                    G4ThreeVector(0, 0, 0), 
  //                    INTT_siLV_inside,            //its logical volume
  //                    "INTT_siLV_all_inside ",       //its name
  //                    INTT_siLV,                //its mother  volume
  //                    false,                   //no boolean operation
  //                    0,                       //copy number
  //                    checkOverlaps);
  //   G4LogicalVolume*INTT_siLV_typeA
  //     = new G4LogicalVolume(INTT_si_typeA, silicon, "INTT_siLV_all_typeA");
  //   G4LogicalVolume*INTT_siLV_typeB
  //     = new G4LogicalVolume(INTT_si_typeB, silicon, "INTT_siLV_all_typeB");    
  //  for (G4int l=0;l<4;l++)
  // { 
  //   G4double gap = 35.*mm;
  //   G4double initialposition = 100.*mm;
  //   G4double ypos;
  //   G4double xpos;
  //   //for silicon  
  //   G4double zpos = initialposition+l*gap;   
  //   for (G4int l1=0; l1<256; l1++)
  //     {
  //       if (l1<128)
  //         {
  //           ypos=(-9.961+(l1*0.078))*mm;
  //           for (G4int l2=0; l2<13; l2++)
  //             { 
  //               if (l2<8)
  //                 {
  //                     xpos=(-107.1+(l2*16))*mm;
  //                     new G4PVPlacement(0, 
  //                                       G4ThreeVector(xpos, ypos, zpos), 
  //                                       INTT_siLV_typeA,            //its logical volume
  //                                       "INTT_siLV_all_typeA",       //its name
  //                                       worldLog,                //its mother  volume
  //                                       false,                   //no boolean operation
  //                                       counting_number,                       //copy number
  //                                       false);
  //                     counting_number+=1;
  //                     G4cout<<"copy test : "<<counting_number<<G4endl;
  //                 }
  //               else 
  //                 {
  //                     xpos=(25.1+((l2-8)*20))*mm;
  //                     new G4PVPlacement(0, 
  //                                       G4ThreeVector(xpos, ypos, zpos), 
  //                                       INTT_siLV_typeB,            //its logical volume
  //                                       "INTT_siLV_all_typeB",       //its name
  //                                       worldLog,                //its mother  volume
  //                                       false,                   //no boolean operation
  //                                       counting_number,                       //copy number
  //                                       false);
  //                     counting_number+=1;
  //                     G4cout<<"copy test : "<<counting_number<<G4endl;
  //                 }  
  //             }
  //         }
  //       else
  //         {
  //           ypos=(0.055+((l1-128)*0.078))*mm;
  //           for (G4int l2=0; l2<13; l2++)
  //             { 
  //               if (l2<8)
  //                 {
  //                     xpos=(-107.1+(l2*16))*mm;
  //                     new G4PVPlacement(0, 
  //                                       G4ThreeVector(xpos, ypos, zpos), 
  //                                       INTT_siLV_typeA,            //its logical volume
  //                                       "INTT_siLV_all_typeA",       //its name
  //                                       worldLog,                //its mother  volume
  //                                       false,                   //no boolean operation
  //                                       counting_number,                       //copy number
  //                                       false);
  //                     counting_number+=1;
  //                     G4cout<<"copy test : "<<counting_number<<G4endl;
  //                 }
  //               else 
  //                 {
  //                     xpos=(25.1+((l2-8)*20))*mm;
  //                     new G4PVPlacement(0, 
  //                                       G4ThreeVector(xpos, ypos, zpos), 
  //                                       INTT_siLV_typeB,            //its logical volume
  //                                       "INTT_siLV_all_typeB",       //its name
  //                                       worldLog,                //its mother  volume
  //                                       false,                   //no boolean operation
  //                                       counting_number,                       //copy number
  //                                       false);
  //                     counting_number+=1;
  //                     G4cout<<"copy test : "<<counting_number<<G4endl;
  //                 }  
  //             }
  //         }  
  //     }
  //  }  // end block for for loop

  G4LogicalVolume * INTT_sciLV = new G4LogicalVolume(INTT_sci, BBHodo_Scinti, "INTT_sciLV1");
  G4double zpos = -58.75 * mm;   
  new G4PVPlacement(0, 
                    G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
                    INTT_sciLV,            //its logical volume
                    "INTT_sciLV1",       //its name
                    INTT_testbeam_BOXLV,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);

  G4double zpos1 = 276.45 * mm;   
  new G4PVPlacement(0, 
                    G4ThreeVector(0, 0, zpos1- zpos_testbeam_box), 
                    INTT_sciLV,            //its logical volume
                    "INTT_sciLV1",       //its name
                    INTT_testbeam_BOXLV,                //its mother  volume
                    false,                   //no boolean operation
                    1,                       //copy number
                    checkOverlaps);

  
  //for chip
  G4LogicalVolume* INTT_ChipLV = new G4LogicalVolume(INTT_Chip, silicon, "INTT_ChipLV_name");
  G4LogicalVolume* INTT_Chip_glueLV = new G4LogicalVolume(INTT_Chip_glue, silicon, "INTT_Chip_glueLV_name");
  G4LogicalVolume* INTT_Chip_areaLV = new G4LogicalVolume(INTT_Chip_area, silicon, "INTT_Chip_areaLV_name");
  G4int counting_number_chip = 0;

  for (G4int l=0; l<kLadder_num; l++)
    { 
      G4double gap = 35.0 * mm;
      initialposition = 100.0 * mm;
  
      G4double zpos = initialposition + l * gap;

      for (G4int l1=0; l1< 2; l1++)
	{
	  for(G4int l2=0; l2<13; l2++)
	    {
	      if ( l2 < 8 ) // not 12 but l2
		{

		  auto chip_position
		    = G4ThreeVector( -1 * ( 116.1 - 5 - 4.5 - l2 * 16. ) * mm,
				     ( 19 - 3.775 - 1.5 - l1 * 27.45 ) * mm,
				     zpos- zpos_testbeam_box);
		  new G4PVPlacement(0, 
				    chip_position,
				    INTT_ChipLV,            //its logical volume
				    "INTT_ChipLV_name",       //its name
				    INTT_testbeam_BOXLV,                //its mother  volume
				    false,                   //no boolean operation
				    counting_number_chip,                       //copy number
				    checkOverlaps);	 	  	  	
		}
	      else 
		{

		  auto chip_position
		    = G4ThreeVector( -1 * ( 116.1 - 141.7 - (l2 - 8) * 20) * mm,
				     ( 19 - 3.775 - 1.5 - l1 * 27.45 ) * mm,
				     zpos - zpos_testbeam_box);
		  new G4PVPlacement(0, 
				    chip_position,
				    INTT_ChipLV,            //its logical volume
				    "INTT_ChipLV_name",       //its name
				    INTT_testbeam_BOXLV,                //its mother  volume
				    false,                   //no boolean operation
				    counting_number_chip,                       //copy number
				    checkOverlaps);	 	  	  	
		}

	      counting_number_chip+=1;
	    }
	}

      // INTT_ChipLV[l]
      //   = new G4LogicalVolume(INTT_Chip, silicon, INTT_ChipLV_name[0]);  
    }  // end block for for loop


  // loop over all INTT ladders to create HDI cables
  for (G4int l=0;l<kLadder_num;l++)
    { 
      G4double gap = 35.*mm;
      initialposition = 100.186*mm;
      //initialposition += + HDI_copper_thickness;

      //for HDI copper
      INTT_HDI_copperLV[l]
	= new G4LogicalVolume(INTT_HDI_copper, Copper, INTT_HDI_copperLV_name[l]);
    
      G4double zpos = initialposition + l * gap;   
      new G4PVPlacement(0, 
			G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
			INTT_HDI_copperLV[l],            //its logical volume
			INTT_HDI_copperLV_name[l],       //its name
			INTT_testbeam_BOXLV,                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      //initialposition = 100.402 * mm;
      initialposition += (HDI_copper_thickness + HDI_kapton_thickness ) / 2 ;
      //for HDI Kapton
      INTT_HDI_KaptonLV[l]
	= new G4LogicalVolume(INTT_HDI_Kapton,Kapton, INTT_HDI_KaptonLV_name[l]);
    
      zpos = initialposition + l * gap;   
      new G4PVPlacement(0, 
			G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
			INTT_HDI_KaptonLV[l],            //its logical volume
			INTT_HDI_KaptonLV_name[l],       //its name
			INTT_testbeam_BOXLV,                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

    }  // end block for for loop
  
  // loop over all INTT ladders to create the carbon staves
  for (G4int l=0; l<kLadder_num; l++)
    { 
      G4double gap = 35.0 * mm;
      //initialposition = 103.232 * mm;
      initialposition += HDI_kapton_thickness / 2 + INTT_stave_area_thickness / 2;
      
      // mother volume of the carbon stave:
      //              flat CFRP plate, rohacell forms, CFRP tube, cooling water, and formed CFRP plate
      G4String stave_areaLV_name = "INTT_stave_area" + to_string( l + 1 );
      INTT_stave_areaLV[l]
	= new G4LogicalVolume(INTT_stave_area, air, stave_areaLV_name );
    
      G4double zpos = initialposition + l * gap;   
      new G4PVPlacement(0, 
			//G4ThreeVector(0, 9.725*mm, zpos- zpos_testbeam_box), 
			G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
			INTT_stave_areaLV[l],            //its logical volume
			stave_areaLV_name,
			INTT_testbeam_BOXLV,                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      //for flat carbon fiber layer
      INTT_CFRPLVf[l]
	= new G4LogicalVolume(INTT_CFRP, CFRP, INTT_CFRPLVf_name[l] );

      G4double xpos_local = 0.0;
      G4double ypos_local = 0.0;
      G4double zpos_local = -INTT_stave_area_thickness / 2 + INTT_CFRP_thickness / 2;
      new G4PVPlacement(0, 
			///			G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
			G4ThreeVector(xpos_local, ypos_local, zpos_local ), 
			INTT_CFRPLVf[l],            //its logical volume
			INTT_CFRPLVf_name[l],       //its name
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);
      
      //for formed carbon fiber layer
      INTT_CFRPLVr[l]
	= new G4LogicalVolume(INTT_CFRP,CFRP, INTT_CFRPLVr_name[l]);

      // the formed carbon, short outer part on the top
      G4String formed_CFRP_outer_partLV_name = "INTT_formed_carbonfiber_outer_top" + to_string(l+1);
      INTT_formed_CFRP_outer_partLV[l]
	= new G4LogicalVolume(INTT_formed_CFRP_outer_part, CFRP, formed_CFRP_outer_partLV_name );

      ypos_local = kLadder_vertical_length / 2 - INTT_formed_CFRP_outer_length / 2;
      zpos_local = -INTT_stave_area_thickness / 2 + INTT_CFRP_thickness * 3 / 2;
      new G4PVPlacement(0, 
			G4ThreeVector(xpos_local, ypos_local, zpos_local ), 
			INTT_formed_CFRP_outer_partLV[l],            //its logical volume
			formed_CFRP_outer_partLV_name,
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // the formed carbon, short outer part on the bottom
      formed_CFRP_outer_partLV_name = "INTT_formed_carbonfiber_outer_bottom" + to_string(l+1);
      ypos_local = -ypos_local;
      new G4PVPlacement(0, 
			G4ThreeVector(xpos_local, ypos_local, zpos_local ), 
			INTT_formed_CFRP_outer_partLV[l],            //its logical volume
			formed_CFRP_outer_partLV_name,
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // the formed carbon, slantpart on the upper side
      G4String slant_upper_name = "INTT_formed_carbonfiber_slant_upper" + to_string(l+1);
      INTT_formed_CFRP_slant_partLV[l] = new G4LogicalVolume(INTT_formed_CFRP_slant,
							     CFRP, slant_upper_name );
			      
      ypos_local = INTT_formed_CFRP_straight_length / 2;
      zpos_local = -INTT_stave_area_thickness / 2 + INTT_formed_CFRP_thickness ;
      new G4PVPlacement(tubeRotation,
			G4ThreeVector(xpos_local, ypos_local, zpos_local ),
			INTT_formed_CFRP_slant_partLV[l],            //its logical volume
			slant_upper_name,       //its name
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // the formed carbon, slantpart on the lower side
      G4String slant_lower_name = "INTT_formed_carbonfiber_slant_lower" + to_string(l+1);
      INTT_formed_CFRP_slant_partLV[l] = new G4LogicalVolume(INTT_formed_CFRP_slant,
							     CFRP, slant_lower_name );
			      
      ypos_local = -INTT_formed_CFRP_straight_length / 2;
      G4RotationMatrix* slant_lower_rotation = new G4RotationMatrix();
      slant_lower_rotation->rotateY( 90.0 * deg );
      slant_lower_rotation->rotateX( 180.0 * deg );

      new G4PVPlacement(slant_lower_rotation,
			G4ThreeVector(xpos_local, ypos_local, zpos_local ),
			INTT_formed_CFRP_slant_partLV[l],            //its logical volume
			slant_lower_name,       //its name
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // the formed carbon, straight part on the center
      G4String formed_CFRP_straightLV_name = "INTT_formed_carbonfiber_straight" + to_string(l+1);
      INTT_formed_CFRP_straightLV[l]
	= new G4LogicalVolume(INTT_formed_CFRP_straight, CFRP, formed_CFRP_straightLV_name );

      ypos_local = 0;
      zpos_local = INTT_stave_area_thickness / 2 - INTT_CFRP_thickness / 2;
      new G4PVPlacement(0, 
			G4ThreeVector(xpos_local, ypos_local, zpos_local ), 
			INTT_formed_CFRP_straightLV[l],            //its logical volume
			formed_CFRP_straightLV_name,
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // the rohacel form
      G4String form_name = "INTT_form" + to_string( l+1 );
      INTT_foam_LV[l]
	= new G4LogicalVolume(INTT_foam, foam, form_name );
    
      ypos_local = 0;
      //  zpos_local = -INTT_stave_area_thickness / 2 + INTT_CFRP_thickness + INTT_CFRP_tube_outer_radius;
      zpos_local = -INTT_stave_area_thickness / 2 + INTT_CFRP_thickness;
      new G4PVPlacement(foam_rotation, 
			G4ThreeVector(xpos_local, ypos_local, zpos_local ), 
			INTT_foam_LV[l],            //its logical volume
			form_name,       //its name
			INTT_stave_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);


      ////////////////////////////////////////////////////////////
      // mother volume of the CFRP tube and water, it's filled with epoxy, it means glue
      G4String tube_area_name = "INTT_carbonfiber_tube_areaLV" + to_string(l+1);
      INTT_CFRP_tube_areaLV[l] = new G4LogicalVolume(INTT_CFRP_tube_area,
							    G4Material::GetMaterial("Epoxy"), tube_area_name );
			      
      xpos_local = INTT_CFRP_tube_area_thickness / 2;
      ypos_local = 0.0;
      zpos_local = 0.0;
      new G4PVPlacement(tubeRotation,
			G4ThreeVector(xpos_local, ypos_local, zpos_local ),
			INTT_CFRP_tube_areaLV[l],            //its logical volume
			tube_area_name,       //its name
			INTT_foam_LV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);


      // CFRP tube
      INTT_CFRP_tubeLV[l]
	= new G4LogicalVolume(INTT_CFRP_tube,CFRP, INTT_CFRP_tubeLV_name[l]);
    
      new G4PVPlacement(tubeRotation, 
			G4ThreeVector(0, 0, 0), 
			INTT_CFRP_tubeLV[l],            //its logical volume
			INTT_CFRP_tubeLV_name[l],       //its name
			INTT_CFRP_tube_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // cooling water
      INTT_waterLV[l]
	= new G4LogicalVolume(INTT_water,water, INTT_waterLV_name[l]);
    
      new G4PVPlacement(tubeRotation, 
			G4ThreeVector(0, 0, 0), 
			INTT_waterLV[l],            //its logical volume
			INTT_waterLV_name[l],       //its name
			INTT_CFRP_tube_areaLV[l],                //its mother  volume
			false,                   //no boolean operation
			0,                       //copy number
			checkOverlaps);

      // end of the tube area
      ////////////////////////////////////////////////////////////
    }  // end block for for loop
  
  //always return the physical World
  return worldPV;
}

void EDDetectorConstruction::ConstructSDandField()
{
  // EDChamberSD*INTT_siSD[4];
  // G4String INTT_siSD_name[4]={"INTT_siSD_1","INTT_siSD_2","INTT_siSD_3","INTT_siSD_4"};
  // G4String INTT_siSD_HitsCollection_name[4]={"INTT_siSD_HitsCollection_1","INTT_siSD_HitsCollection_2","INTT_siSD_HitsCollection_3","INTT_siSD_HitsCollection_4"};
  // G4String INTT_siLV_name[4]={"INTT_siLV1","INTT_siLV2","INTT_siLV3","INTT_siLV4"};

  EDChamberSD* chamber1SD = new EDChamberSD("Chamber1SD", "Chamber1HitsCollection", 0);
  G4SDManager::GetSDMpointer()->AddNewDetector(chamber1SD);
  SetSensitiveDetector("INTT_siLV_all_typeA", chamber1SD);

  EDChamberSD* chamber2SD = new EDChamberSD("Chamber2SD", "Chamber2HitsCollection", 0);
  G4SDManager::GetSDMpointer()->AddNewDetector(chamber2SD);
  SetSensitiveDetector("INTT_siLV_all_typeB", chamber2SD);

  EDEmCalorimeterSD* calorimeterSD
    = new EDEmCalorimeterSD("EmCalorimeterSD", "EmCalorimeterHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(calorimeterSD);
  SetSensitiveDetector("INTT_sciLV1", calorimeterSD);

  // EDChamberSD* trigger_1 = new EDChamberSD("Trigger_1", "Trigger_1HitsCollection", 1);
  // G4SDManager::GetSDMpointer()->AddNewDetector(trigger_1);
  // SetSensitiveDetector("INTT_sciLV1", trigger_1);


  /*for (int i2=0; i2<4; i2++)
    {
    INTT_siSD[i2]=new EDChamberSD(INTT_siSD_name[i2],INTT_siSD_HitsCollection_name[i2],i2);
    G4SDManager::GetSDMpointer()->AddNewDetector(INTT_siSD[i2]);
    SetSensitiveDetector(INTT_siLV_name[i2], INTT_siSD[i2]);
    }*/
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // 
  // Sensitive detectors
  //
}  
