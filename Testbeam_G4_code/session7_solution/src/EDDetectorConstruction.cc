//
// ********************************************************************
// *License and Disclaimer                                           *
// *                                                                  *
// *The  Geant4 software  is  copyright of the Copyright Holders  of *
// *the Geant4 Collaboration.  It is provided  under  the terms  and *
// *conditions of the Geant4 Software License,  included in the file *
// *LICENSE and available at  http://cern.ch/geant4/license .  These *
// *include a list of copyright holders.                             *
// *                                                                  *
// *Neither the authors of this software system, nor their employing *
// *institutes,nor the agencies providing financial support for this *
// *work  make  any representation or  warranty, express or implied, *
// *regarding  this  software system or assume any liability for its *
// *use.  Please see the license in the file  LICENSE  and URL above *
// *for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// *This  code  implementation is the result of  the  scientific and *
// *technical work of the GEANT4 collaboration.                      *
// *By using,  copying,  modifying or  distributing the software (or *
// *any work based  on the software)  you  agree  to acknowledge its *
// *use  in  resulting  scientific  publications,  and indicate your *
// *acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file EDDetectorConstruction.cc
/// \brief Implementation of the EDDetectorConstruction class
#include "EDDetectorConstruction.hh"

// the ultra-stupid units
const G4double EDDetectorConstruction::inch = 25.4; // 1 inch = 25.4 mm

EDDetectorConstruction::EDDetectorConstruction()
  : kSilicon_strip_width( 78.0 * um ),
    kSilicon_strip_thickness( 320.0 * um ),
    kSilicon_length_type_a( 16.0 * mm ),
    kSilicon_length_type_b( 20.0 * mm ),
    kFPHX_width( 2.7 * mm ),
    kFPHX_thickness( 320.0 * um ),
    kFPHX_length( 9.1 * mm ),
    kSilver_epoxy_glue_silicon_thickness( 14 * um ),
    kSilver_epoxy_glue_FPHX_thickness( 50 * um ),
    kINTT_CFRP_thickness( 300 * um ),
    kDarkbox_wall_thickness_body( 3.0 / 4 * inch ),   // 3/4 inch
    kDarkbox_wall_thickness_side( 1.0 / 2 * inch ),  // 1/2 inch
    G4VUserDetectorConstruction()
{

    // Option to switch on/off checking of volumes overlaps
    checkOverlaps = true;

    // assign very primitive paraleters here
    // size of the world
    world_size[0] = 0.4 * m;
    world_size[1] = 0.2 * m;
    world_size[2] = 0.7 * m;

    // size of the dark box
    INTT_testbeam_BOX_size[0] = 25.0 / 2  * inch;  // total length is 25 inch. In this simulation, half box should be enough
    INTT_testbeam_BOX_size[1] = INTT_testbeam_BOX_size[2] = 6.54 * inch;
    
    // check whether the dark box is larger than the world or not
    for (int i = 0; i < 3; i++)
        assert(world_size[i] > INTT_testbeam_BOX_size[i]);

}

EDDetectorConstruction::~EDDetectorConstruction() {}

void EDDetectorConstruction::DefineMaterials()
{

    // Get nist material manager
    G4NistManager *nistManager = G4NistManager::Instance();
    G4bool fromIsotopes = false;

    // Build materials
    DefaultMaterial = nistManager->FindOrBuildMaterial("G4_AIR", fromIsotopes);
    nistManager->FindOrBuildMaterial("G4_Al", fromIsotopes);
    nistManager->FindOrBuildMaterial("G4_Ag", fromIsotopes);
    nistManager->FindOrBuildMaterial("G4_Cu", fromIsotopes);
    // There is no need to test if materials were built/found
    // as G4NistManager would issue an error otherwise
    // Try the code with "XYZ".      

    G4int nel, natoms;
    Silicon = nistManager->FindOrBuildMaterial("G4_Si", fromIsotopes);

    //vacuum 
    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008 *g / mole;
    G4double density = 1.e-25 *g / cm3;
    G4double temperature = 2.73 * kelvin;
    G4double pressure = 3.e-18 * pascal;
    G4Material *Vacuum = new G4Material("interGalactic", atomicNumber, massOfMole, density, kStateGas, temperature, pressure);

    //element
    G4double fractionmass;
    G4NistManager *man = G4NistManager::Instance();
    G4Element *elH = man->FindOrBuildElement("H");
    G4Element *elC = man->FindOrBuildElement("C");
    G4Element *elN = man->FindOrBuildElement("N");
    G4Element *elO = man->FindOrBuildElement("O");
    G4Element *elementH = new G4Element("Hydrogen", "H", 1., 1.0079 *g / mole);
    G4Element *elementC = new G4Element("Carbon", "C", 6., 12.011 *g / mole);

    //sci_BC-408
    G4double d_PolyVinylToluene = 0.57 *g / cm3;
    G4double d_Anthracene = 1.28 *g / cm3;
    G4double d_BBHodo_Scinti = 1.023 *g / cm3;
    G4Material *PolyVinylToluene = new G4Material("PolyVinylToluene", d_PolyVinylToluene, 2);
    PolyVinylToluene->AddElement(elC, fractionmass = 0.91471);
    PolyVinylToluene->AddElement(elH, fractionmass = 0.08529);

    G4Material *Anthracene = new G4Material("Anthracene", d_Anthracene, 2);
    Anthracene->AddElement(elC, fractionmass = 0.943447);
    Anthracene->AddElement(elH, fractionmass = 0.056553);

    BBHodo_Scinti = new G4Material("BBHodo_Scinti", d_BBHodo_Scinti, 2);
    BBHodo_Scinti->AddMaterial(PolyVinylToluene, fractionmass = 0.36);
    BBHodo_Scinti->AddMaterial(Anthracene, fractionmass = 0.64);

    //kapton
    density = 1.42 *g / cm3;
    Kapton = new G4Material("Kapton", density, nel = 4);
    Kapton->AddElement(elH, fractionmass = 0.0264);
    Kapton->AddElement(elC, fractionmass = 0.6911);
    Kapton->AddElement(elN, fractionmass = 0.0733);
    Kapton->AddElement(elO, fractionmass = 0.2092);

    //Copper
    density = 8.96 *g / cm3;
    G4double a = 63.54 *g / mole;
    Copper = new G4Material("Copper", 29.0, a, density);

    // Glue for the carbon fiber
    // actually, it's the same as one used in the sPHENIX Geant simulation
    Epoxy = new G4Material("Epoxy", 1.2 *g / cm3, 2);
    Epoxy->AddElement(G4Element::GetElement("H"), natoms = 2);
    Epoxy->AddElement(G4Element::GetElement("C"), natoms = 2);

    //carbon fiber
    density = 0.145 *g / cm3;
    CFRP = new G4Material("CFRP", density, nel = 1);
    CFRP->AddElement(elC, 1);

    // Foam: Polystyrene-based
    foam = new G4Material("foam", 0.05 *g / cm3, 2);
    foam->AddElement(elementC, 0.90);
    foam->AddElement(elementH, 0.10);

    //water
    density = 1.000 *g / cm3;
    Water = new G4Material("water", density, 2);
    Water->AddElement(elH, natoms = 2);
    Water->AddElement(elO, natoms = 1);

    // Silver epoxy glue LOCTITE ABLESTIK 2902 for the silicon sensors and FPHX chips of INTT
    SilverEpoxyGlue = new G4Material("SilverEpoxyGlue", density = 3.2 *g / cm3, 2);
    SilverEpoxyGlue->AddMaterial(Epoxy, fractionmass = 0.79);
    SilverEpoxyGlue->AddMaterial(G4Material::GetMaterial("G4_Ag"), fractionmass = 0.21);

    // material of the darkbox (polyacetal)
    DarkBox = new G4Material("SilverEpoxyGlue", density = 1.42 *g / cm3, 3);
    DarkBox->AddElement(G4Element::GetElement("H"), natoms = 2);
    DarkBox->AddElement(G4Element::GetElement("C"), natoms = 1);
    DarkBox->AddElement(G4Element::GetElement("O"), natoms = 1);
}

void EDDetectorConstruction::ConstructDarkBox(G4LogicalVolume *worldLog)
{

  G4Box *INTT_testbeam_BOX = new G4Box("INTT_testbeam_BOX",
				       this->INTT_testbeam_BOX_size[0] / 2,
				       this->INTT_testbeam_BOX_size[1] / 2,
				       this->INTT_testbeam_BOX_size[2] / 2);
  
  INTT_testbeam_BOXLV
    = new G4LogicalVolume(INTT_testbeam_BOX, DefaultMaterial, "INTT_testbeam_BOXLV");
  INTT_testbeam_BOXLV->SetVisAttributes(color_invisible);
  //zpos_testbeam_box = 108.85 * mm;
  zpos_testbeam_box = 0.0;
  //zpos_testbeam_box_1 = 108.85 * mm;  
  zpos_testbeam_box_1 = 0.0;
  
  INTT_testbeam_BOXPV =
    new G4PVPlacement(0,
		      G4ThreeVector(0, 0, zpos_testbeam_box_1),
		      INTT_testbeam_BOXLV,  //its logical volume
		      "INTT_testbeam_BOXLV",  //its name
		      worldLog, //its mother  volume
		      false,  //no boolean operation
		      0,  //copy number
		      checkOverlaps);

  ////////////////////////////////
  // walls of the dark box      //
  ////////////////////////////////
  // walls on the top and bottom /////////////////////////
    G4Box *darkbox_wall_tb = new G4Box("darkbox_wall_tb",
				       INTT_testbeam_BOX_size[0] / 2 - this->kDarkbox_wall_thickness_side,
				       this->kDarkbox_wall_thickness_body / 2,
				       INTT_testbeam_BOX_size[2] / 2 );

    auto darkbox_wall_tbLV = new G4LogicalVolume(darkbox_wall_tb, this->DarkBox, "darkbox_wall_tb");
    darkbox_wall_tbLV->SetVisAttributes(color_darkbox_wall);
    // put on the bottom
    new G4PVPlacement(0, G4ThreeVector(0, -INTT_testbeam_BOX_size[1] / 2 + this->kDarkbox_wall_thickness_body / 2, 0),
        darkbox_wall_tbLV, "darkbox_wall_bottom", INTT_testbeam_BOXLV,
        false, 0, checkOverlaps);

    // put on the top
    new G4PVPlacement(0, G4ThreeVector(0, +INTT_testbeam_BOX_size[1] / 2 - this->kDarkbox_wall_thickness_body / 2, 0),
        darkbox_wall_tbLV, "darkbox_wall_top", INTT_testbeam_BOXLV,
        false, 0, checkOverlaps);


    // walls on the left and right /////////////////////////////////////////////////
    G4Box *darkbox_wall_lr = new G4Box("darkbox_wall_lr",
				       this->kDarkbox_wall_thickness_side / 2,
				       INTT_testbeam_BOX_size[1] / 2,
				       INTT_testbeam_BOX_size[2] / 2  );
    
    auto darkbox_wall_lrLV = new G4LogicalVolume(darkbox_wall_lr, this->DarkBox, "darkbox_wall_lr");
    darkbox_wall_lrLV->SetVisAttributes(color_darkbox_wall);
    // put on the left
    new G4PVPlacement(0, G4ThreeVector(-INTT_testbeam_BOX_size[0] / 2 + this->kDarkbox_wall_thickness_side / 2, 0, 0),
        darkbox_wall_lrLV, "darkbox_wall_right", INTT_testbeam_BOXLV,
        false, 0, checkOverlaps);

    // put on the right
    new G4PVPlacement(0, G4ThreeVector(+INTT_testbeam_BOX_size[0] / 2 - this->kDarkbox_wall_thickness_side / 2, 0, 0),
        darkbox_wall_lrLV, "darkbox_wall_left", INTT_testbeam_BOXLV,
        false, 0, checkOverlaps);

    // walls on the upstream and downstream sides, beam windows are on them /////////////////////
    G4Box *darkbox_wall_upstream = new G4Box("darkbox_wall_upstream",
					     INTT_testbeam_BOX_size[0] / 2 - this->kDarkbox_wall_thickness_side,
					     INTT_testbeam_BOX_size[1] / 2 - this->kDarkbox_wall_thickness_body,
					     this->kDarkbox_wall_thickness_body / 2);

    //! @TODO measure the dimension
    auto darkbox_beam_window = new G4Box("darkbox_beam_window",
					 this->kSilicon_length_type_a / 2 * 8 + this->kSilicon_length_type_b / 2 * 5, // horizontal direction, same as the active area for the moment
					 1 * cm, // vertical direction
					 kDarkbox_wall_thickness_body );

    auto darkbox_wall_upstream_window = new G4SubtractionSolid("darkbox_beam_window", darkbox_wall_upstream, darkbox_beam_window);
    auto darkbox_wall_upstreamLV = new G4LogicalVolume(darkbox_wall_upstream_window, this->DarkBox, "darkbox_wall_upstream");
    darkbox_wall_upstreamLV->SetVisAttributes(color_darkbox_wall);
    new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * this->INTT_testbeam_BOX_size[2] / 2 + this->kDarkbox_wall_thickness_body / 2),  // rotation, position
        darkbox_wall_upstreamLV, "darkbox_wall_upstream", INTT_testbeam_BOXLV,  // logical volume, name, mother volume
        false, 0, checkOverlaps); // boolean operation, copy number, check overlap

    new G4PVPlacement(0, G4ThreeVector(0, 0, +INTT_testbeam_BOX_size[2] / 2 - this->kDarkbox_wall_thickness_body / 2),
        darkbox_wall_upstreamLV, "darkbox_wall_downstream", INTT_testbeam_BOXLV,
        false, 0, checkOverlaps);

}

G4VPhysicalVolume *EDDetectorConstruction::Construct()
{
  DefineMaterials();
  DefineVisAttributes();

    // World
    // world volume
    G4Box *worldS = new G4Box("World", this->world_size[0] / 2, this->world_size[1] / 2, this->world_size[2] / 2);

    G4LogicalVolume *worldLog = new G4LogicalVolume(worldS, DefaultMaterial, "worldLog");
    worldLog->SetVisAttributes(color_invisible);

    G4VPhysicalVolume *worldPV = new G4PVPlacement(0, //no rotation
        G4ThreeVector(),  //at (0,0,0)
        worldLog, //its logical volume
        "worldLog", //its name
        0,  //its mother  volume
        false,  //no boolean operation
        0,  //copy number
        checkOverlaps); //overlaps checking

    this->ConstructDarkBox(worldLog);

    //======================G4solid===============================================  
    const int kLadder_num = 4;
    const bool kIs_silicon_off = false;  // flag for debugging, turning all silicon strips off to save time

    //G4LogicalVolume* INTT_siLV[kLadder_num];
    vector<G4String> INTT_siLV_name, INTT_HDI_copperLV_name, INTT_HDI_KaptonLV_name,
        INTT_CFRPLVf_name, INTT_foam_LVup_name, INTT_foam_LVdown_name,
        INTT_CFRP_tubeLV_name, INTT_waterLV_name, INTT_CFRPLVr_name,
        INTT_CFRP_glueLV_name,INTT_siLV_glue_typeA_name, INTT_siLV_glue_typeB_name, INTT_siLV_gap_name;

    for (int i = 0; i < kLadder_num; i++)
    {
        INTT_siLV_name.push_back(G4String("INTT_siLV") + to_string(i + 1));
        INTT_siLV_gap_name.push_back(G4String("INTT_siLV_gap_") + to_string(i + 1));
        INTT_siLV_glue_typeA_name.push_back(G4String("INTT_siLV_glue_typeA_") + to_string(i + 1));
        INTT_siLV_glue_typeB_name.push_back(G4String("INTT_siLV_glue_typeB_") + to_string(i + 1));
        INTT_HDI_copperLV_name.push_back(G4String("INTT_HDI_copperLV") + to_string(i + 1));
        INTT_HDI_KaptonLV_name.push_back(G4String("INTT_HDI_KaptonLV") + to_string(i + 1));
        INTT_CFRPLVf_name.push_back(G4String("INTT_carbonfiberLVf") + to_string(i + 1));
        INTT_foam_LVup_name.push_back(G4String("INTT_foam_LVup") + to_string(i + 1));
        INTT_foam_LVdown_name.push_back(G4String("INTT_foam_LVdown") + to_string(i + 1));
        INTT_CFRP_glueLV_name.push_back(G4String("INTT_carbonfiber_glueLV") + to_string(i + 1));
        INTT_CFRP_tubeLV_name.push_back(G4String("INTT_carbonfiber_tubeLV") + to_string(i + 1));
        INTT_waterLV_name.push_back(G4String("INTT_waterLV") + to_string(i + 1));
        INTT_CFRPLVr_name.push_back(G4String("INTT_carbonfiberLVr") + to_string(i + 1));
    }

    vector<G4LogicalVolume*> INTT_siLV_outer(kLadder_num);
    vector<G4LogicalVolume*> INTT_HDI_copperLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_HDI_KaptonLV(kLadder_num);

    vector<G4LogicalVolume*> INTT_stave_areaLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_CFRPLVf(kLadder_num);
    vector<G4LogicalVolume*> INTT_foam_LV(kLadder_num);
    vector<G4LogicalVolume*> INTT_CFRP_tube_areaLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_CFRP_glueLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_CFRP_tubeLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_waterLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_CFRPLVr(kLadder_num); // to be deleted
    vector<G4LogicalVolume*> INTT_formed_CFRPLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_formed_CFRP_straightLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_formed_CFRP_outer_partLV(kLadder_num);
    vector<G4LogicalVolume*> INTT_formed_CFRP_slant_partLV(kLadder_num);

    const G4double kLadder_horizontal_length = 232.2 * mm;
    const G4double kLadder_vertical_length = 38 * mm;
    //  1.47684 cm

    //  G4Box* INTT_si_box    = new G4Box("INTT_si_box", 0.3 *m, 0.3 *m, 0.3 *m);
    // container for the active silicon strips and inactive silicon block
    G4Box *INTT_si = new G4Box("INTT_si",
			       116.1 *mm,
			       11.25 *mm,
			       this->kSilicon_strip_thickness / 2 );
    
    // the gap between the silicon type-A and type-B
    G4Box *INTT_si_gap = new G4Box("INTT_si_gap", 0.1 *mm, 11.25 *mm, this->kSilicon_strip_thickness / 2 );

    //  G4Box* INTT_si_typeA    = new G4Box("INTT_si_typeA", 8. *mm, 0.039 *mm, 0.16 *mm);
    G4Box *INTT_si_typeA = new G4Box("INTT_si_typeA",
				     this->kSilicon_length_type_a / 2,
				     this->kSilicon_strip_width / 2,
				     this->kSilicon_strip_thickness / 2);
    //  G4Box* INTT_si_typeB    = new G4Box("INTT_si_typeB", 10. *mm, 0.039 *mm, 0.16 *mm);
    G4Box *INTT_si_typeB = new G4Box("INTT_si_typeB",
				     this->kSilicon_length_type_b / 2,
				     this->kSilicon_strip_width / 2,
				     this->kSilicon_strip_thickness / 2);

    //! @TODO which thickness 14 um or 50 um should be used for the silicon sensors?
    // G4Box * INTT_si_glue = new G4Box("INTT_si_glue", 116.1 *mm, 11.25 *mm, 25 *um);
    //G4Box * INTT_si_glue_typeA = new G4Box("INTT_si_glue_typeA", 65 *mm, 11.25 *mm, 25 *um);
    G4Box *INTT_si_glue_typeA = new G4Box("INTT_si_glue_typeA", 65 *mm, 11.25 *mm, this->kSilver_epoxy_glue_FPHX_thickness / 2 );
    G4Box *INTT_si_glue_typeB = new G4Box("INTT_si_glue_typeB", 51 *mm, 11.25 *mm, this->kSilver_epoxy_glue_FPHX_thickness / 2 );

    G4double INTT_sci_thin_thickness = 2.5*mm;
    G4double INTT_sci_thick_thickness = 5*mm;
    G4Box *INTT_sci = new G4Box("INTT_sci", 116.1 *mm, 11.25 *mm, INTT_sci_thick_thickness/2.);
    G4Box *INTT_sci_thin = new G4Box("INTT_sci_thin", 116.1 *mm, 11.25 *mm, INTT_sci_thin_thickness/2.);

  G4Box* INTT_Chip		= new G4Box("INTT_Chip",
					    this->kFPHX_length/2, this->kFPHX_width/2, this->kFPHX_thickness/2 );

  G4Box* INTT_Chip_glue		= new G4Box("INTT_Chip_glue",
					    this->kFPHX_length/2, this->kFPHX_width/2, this->kSilver_epoxy_glue_FPHX_thickness / 2); 

  // G4Box *INTT_Chip = new G4Box("INTT_Chip", 4.5 *mm, 1.5 *mm, 0.16 *mm);
  //   G4Box *INTT_Chip_glue = new G4Box("INTT_Chip_glue", 4.5 *mm, 1.5 *mm, 25 *um);
  //   G4Box *INTT_Chip_area = new G4Box("INTT_Chip_area", 4.5 *mm, 1.5 *mm, 0.16 *mm + 25 *um);
  //G4Box* INTT_Chip_area = new G4Box("INTT_Chip_area", 4.5 * mm, 1.5 * mm, 0.16 * mm + 50 * um); 
  G4Box* INTT_Chip_area = new G4Box("INTT_Chip_area", this->kFPHX_length/2, this->kFPHX_width/2, this->kFPHX_thickness + this->kSilver_epoxy_glue_FPHX_thickness );
    //G4VSolid *siinactive_box = new G4SubtractionSolid((boost::format("siinactive_box_%d_%d") % inttlayer % itype).str(),

    //sifull_box, siactive_box, 0, G4ThreeVector(0, 0, 0));
    G4double INTT_CFRP_tube_inner_radius = 1.5 * mm;
    G4double INTT_CFRP_tube_outer_radius = INTT_CFRP_tube_inner_radius + kINTT_CFRP_thickness;
    G4double INTT_CFRP_tube_area_thickness = INTT_CFRP_tube_outer_radius * 2;

    G4Box *INTT_CFRP = new G4Box("INTT_carbonfiber",
        kLadder_horizontal_length / 2,
        kLadder_vertical_length / 2,
				 kINTT_CFRP_thickness / 2);

    // the formed CFRP plate, which consits of 5 parts:
    //     - outer part (top)       ^ y-axis (vertical axis)
    //     - slant part (upper)     |
    //     - straight part (center) |
    //     - slant part (lower)     |
    //     - outer part (bottom)    |
    // 0.33227 cm
    G4double INTT_formed_CFRP_outer_length = 3.3227 * mm;
    G4double INTT_formed_CFRP_straight_length = 14.7684 * mm;
    G4double INTT_formed_CFRP_thickness = kINTT_CFRP_thickness;
    G4double INTT_formed_CFRP_block_thickness = INTT_CFRP_tube_outer_radius *2 + INTT_formed_CFRP_thickness;
    G4Box *INTT_formed_CFRP_straight = new G4Box("INTT_formed_carbonfiber_straight",
        kLadder_horizontal_length / 2,
        INTT_formed_CFRP_straight_length / 2,
        INTT_formed_CFRP_thickness / 2);

    G4Box *INTT_formed_CFRP_outer_part = new G4Box("INTT_formed_carbonfiber_outer_part",
        kLadder_horizontal_length / 2,
        INTT_formed_CFRP_outer_length / 2,
        INTT_formed_CFRP_thickness / 2);

    // the foam parts (rohacell)
    G4double trap_y_length = kLadder_vertical_length - 2 * INTT_formed_CFRP_outer_length;
    vector<G4TwoVector> form_trap_vectors;

  ////////////////////////////////////////////////////////////
  // the foam parts (rohacell), cross-section in y-z plane:
  //   ^ y-axis (vertical axis)
  //   |
  //   +-+-+ - - - - - -+- - - - - - - - - - - - - - - - - - -+
  //   |f|o|            |                                     |
  //   | |u|            | INTT_formed_CFRP                    | kLadder_vertical_length
  //   | |t|            |  _outer_length                      |
  //   |l|e|            |                                     |
  //   | |r|            |                                     |
  //   + +-+ - - - - - -+- - - - - - - - - - +                |
  //   |a|\ \                                |                |
  //   | | \s\                               | trap_y_length  |
  //   | |  \l\                              |                |
  //   |t|   \a\                             |                |
  //   | |    \n\                            |                |
  //   | |     \t\                           |                |
  //   |C|      \ \                          |                |
  //   | |       \ \                         |                |
  //   | | foam  +-+ - -+                    |                |
  //   |F|       | |    |                    |                |
  //   | |       |s|    | INTT_formed_CFRP   |                |
  //   | |       |t|    |  _straight_length  |                |
  //   |R|  ---  |r|    |                    |                |
  //   | | /   \ |a|    |                    |                |
  // - | ||water||i| -- | -----> z-axis      |                |
  //   |P| \   / |g|    |                    |                |
  //   | |  ---  |h|    |                    |                |
  //   | |       |t|    |                    |                |
  //   |p|       | |    |                    |                |
  //   | |       | |    |                    |                |
  //   | | foam  +-+ - -+                    |                |
  //   |l|       / /                         |                |
  //   | |      /s/                          |                |
  //   | |     /l/                           |                |
  //   | |    /a/                            |                |
  //   |a|   /n/                             |                |
  //   | |  /t/                              |                |
  //   | | / /                               |                |
  //   |t|/ /                                |                |
  //   + +-+ - - - - - - - - - - - - - - - - +                |
  //   | |o|                                                  |
  //   |e|u|                                                  |
  //   | |t|                                                  |
  //   | |e|                                                  |
  //   | |r|                                                  |
  //   +-+-+ - - - - - - - - - - - - - - - - - - - - - - - - -+
  //     | |
  //  -->| |<-- kINTT_CFRP_thickness
  //
  //
  
  ////////////////////////////////////////////////////////////   
    for (int i = 0; i < 2; i++) // for 8 vertices
    {
        form_trap_vectors.push_back(G4TwoVector(0. *cm, -trap_y_length / 2));
        form_trap_vectors.push_back(G4TwoVector(0. *cm, trap_y_length / 2));
        form_trap_vectors.push_back(G4TwoVector(INTT_CFRP_tube_outer_radius *2, INTT_formed_CFRP_straight_length / 2));
        form_trap_vectors.push_back(G4TwoVector(INTT_CFRP_tube_outer_radius *2, -INTT_formed_CFRP_straight_length / 2));
    }

    auto INTT_foam = new G4GenericTrap("INTT_form", kLadder_horizontal_length / 2, form_trap_vectors);
    G4RotationMatrix *foam_rotation = new G4RotationMatrix();
    foam_rotation->rotateY(90.0 *deg);

    //   cross-section in y-z plane:
    //   ^ y-axis (vertical axis)
    //   |
    //   +---    <---- 
    //   |\ \
    //   | \ \
    //   |  \ \
    //   |   \ \
    //   |    \					\
    //   +----+---+----> z-aixs (beam-axis)
    //   ^
    //   |
    //   origin (y=0, z=0)
    
    vector<G4TwoVector> formed_CFRP_slant_vectors;
    for (int i = 0; i < 2; i++) // for 8 vertices
    {
        formed_CFRP_slant_vectors.push_back(G4TwoVector(0.0, trap_y_length / 2 - INTT_formed_CFRP_straight_length / 2));
        formed_CFRP_slant_vectors.push_back(G4TwoVector(INTT_formed_CFRP_thickness, trap_y_length / 2 - INTT_formed_CFRP_straight_length / 2));

        formed_CFRP_slant_vectors.push_back(G4TwoVector(INTT_CFRP_tube_outer_radius *2 + INTT_formed_CFRP_thickness, 0.0));
        formed_CFRP_slant_vectors.push_back(G4TwoVector(INTT_CFRP_tube_outer_radius *2, 0.0));
    }

    auto INTT_formed_CFRP_slant = new G4GenericTrap("INTT_formed_CFRP_slant", kLadder_horizontal_length / 2, formed_CFRP_slant_vectors);

    G4Box *INTT_formed_CFRP = new G4Box("INTT_formed_carbonfiber",
        kLadder_horizontal_length / 2,
        INTT_formed_CFRP_straight_length / 2,
        INTT_formed_CFRP_thickness / 2);

    // for mother volue of the carbon fiber tune and the cooling water
    //G4double INTT_CFRP_tube_area_thickness = 4.32 * mm;
    G4Box *INTT_CFRP_tube_area = new G4Box("INTT_carbonfiber_glue_solid",
        kLadder_horizontal_length / 2,
        INTT_CFRP_tube_area_thickness / 2,
        INTT_CFRP_tube_area_thickness / 2);

    G4Tubs *INTT_CFRP_tube = new G4Tubs("INTT_carbonfiber_tube",
        INTT_CFRP_tube_inner_radius,
        INTT_CFRP_tube_outer_radius,
        kLadder_horizontal_length / 2, 0, 360 *degree);

    G4Tubs *INTT_water = new G4Tubs("INTT_water",
        0 *mm, INTT_CFRP_tube_inner_radius,
        kLadder_horizontal_length / 2, 0, 360 *degree);

    G4double INTT_form_vertical_length = (kLadder_vertical_length - 2 *INTT_CFRP_tube_outer_radius) / 2;
    G4double INTT_form_thickness = INTT_CFRP_tube_outer_radius * 2; // radius -> diameter

    G4double INTT_stave_area_thickness = kINTT_CFRP_thickness +
        INTT_formed_CFRP_thickness +
        INTT_CFRP_tube_area_thickness;

    G4Box *INTT_stave_area = new G4Box("INTT_stave_area",
        kLadder_horizontal_length / 2,
        kLadder_vertical_length / 2,
        INTT_stave_area_thickness / 2);

    G4double HDI_copper_thickness = 37.6 * um;
    //  G4Box* INTT_HDI_copper  = new G4Box("INTT_HDI_copper", 116.1*mm, 19.*mm, 37.6 *um / 2);
    G4Box *INTT_HDI_copper = new G4Box("INTT_HDI_copper",
        kLadder_horizontal_length / 2,
        kLadder_vertical_length / 2,
        HDI_copper_thickness / 2);
    G4double HDI_kapton_thickness = 380 * um;
    G4Box *INTT_HDI_Kapton = new G4Box("INTT_HDI_Kapton", 116.1 *mm, 19. *mm, HDI_kapton_thickness / 2);

    G4int counting_number = 0;

    //===============================================================================================================
    G4RotationMatrix *tubeRotation = new G4RotationMatrix();
    tubeRotation->rotateY(90.0 *deg);

    G4RotationMatrix *testbeam_BOX_rotation_X = new G4RotationMatrix(); //theta angle
    testbeam_BOX_rotation_X->rotateX(16.0 *deg);

    G4RotationMatrix *testbeam_BOX_rotation_Y = new G4RotationMatrix(); //phi angle
    testbeam_BOX_rotation_Y->rotateY(90.0 *deg);

  //===============================================================================================================
  // logical volume constructions and placements
  //===============================================================================================================
  // positions in z-axis for INTT ladders in the dark box
  //                       INTT_ladders_center
  //     dark box          |
  //   +-------------------|-------------------+
  //   |  |\        |\     |  |\        |\     |
  //   |  |  \      |  \   |  |  \      |  \   |
  //   |  |   |     |   |  |  |   |     |   |  |
  //   |  |   |     |   |  |  |   |     |   |  |
  //  -|- + - + - - + - + -+- + - + - - + - + -+- -
  //   |  |   |     |   |  |  |   |     |   |  |
  //   |  |   |     |   |  |  |   |     |   |  |
  //   |  |  /:     |  /:  |  |  /:     |  /   |
  //   |  |/  :     |/  :  |  |/  :     |/     |
  //   +--|---:-----:---:--+--:---:-----|------+
  //          +-----+   +-----+   +-----+
  //          | gap |   | gap |   | gap |
  
  const G4double kINTT_ladders_center = 0.0;
  //const G4double kGap = 35.0 * mm; // gap betwen ladders, good to use the same parameter for all ladders
  const G4double kGap = 0.5 * inch; // gap betwen ladders, good to use the same parameter for all ladders
  const G4double kStave_thickness = kINTT_CFRP_thickness + INTT_CFRP_tube_outer_radius * 2 + kINTT_CFRP_thickness; // flat CFRP thickness + CFRP tube + formed CFRP thickness

  // silicon   + silver epoxy glue: 320um + 14um
  // FPHX chip + silver epoxy glue: 320um + 50um
  const G4double kINTT_ladder_thickness
    = kStave_thickness + HDI_kapton_thickness + HDI_copper_thickness
    + this->kSilver_epoxy_glue_FPHX_thickness + this->kFPHX_thickness;

  G4double first_ladder_pos_z = kINTT_ladders_center
    - kGap / 2 - kINTT_ladder_thickness // gap between 2nd and 3rd ladders + thickness of the 2nd ladder
    - kGap - kINTT_ladder_thickness; // gap between 1st and 2nd ladders + thickness of the 1st ladder
  
  //===============================================================================================================
    G4LogicalVolume *INTT_siLV_typeA = new G4LogicalVolume(INTT_si_typeA, Silicon, "INTT_siLV_all_typeA");
    INTT_siLV_typeA->SetVisAttributes(color_silicon_active);

    G4LogicalVolume *INTT_siLV_typeB = new G4LogicalVolume(INTT_si_typeB, Silicon, "INTT_siLV_all_typeB");
    INTT_siLV_typeB->SetVisAttributes(color_silicon_active);

    G4LogicalVolume *INTT_siLV_glue_typeA = new G4LogicalVolume(INTT_si_glue_typeA, SilverEpoxyGlue, "INTT_siLV_glue_typeA");
    INTT_siLV_glue_typeA->SetVisAttributes(color_glue);

    G4LogicalVolume *INTT_siLV_glue_typeB = new G4LogicalVolume(INTT_si_glue_typeB, SilverEpoxyGlue, "INTT_siLV_glue_typeB");
    INTT_siLV_glue_typeB->SetVisAttributes(color_glue);

    G4LogicalVolume *INTT_siLV_gap = new G4LogicalVolume(INTT_si_gap,  DefaultMaterial, "INTT_siLV_gap");
    INTT_siLV_gap->SetVisAttributes(color_CFRP_foam);    

    // loop over all INTT ladders for the silicon strips
    for (G4int l = 0; l < kLadder_num; l++)
    {

        // if the debuggind flag is ture, skip silicon strips
        if (kIs_silicon_off == true)
            continue;

	// put the area for the active and inactive silicon
        INTT_siLV_outer[l] = new G4LogicalVolume(INTT_si, Silicon, INTT_siLV_name[l]);
        INTT_siLV_outer[l]->SetVisAttributes(color_silicon_inactive);

        //for silicon    
	G4double zpos = first_ladder_pos_z        // start from the position of the first ladder (upstream surface)
	  + l * kINTT_ladder_thickness + l * kGap // move to the ladder position which we are working in this loop
	  + this->kSilicon_strip_thickness / 2; // move the the center of the silicon strip

        G4VPhysicalVolume *INTT_siLV_outer_allPV =
            new G4PVPlacement(0,
			      // G4ThreeVector(0, 0, zpos - zpos_testbeam_box),
			      G4ThreeVector(0, 0, zpos), 
                INTT_siLV_outer[l], //its logical volume
                INTT_siLV_name[l],  //its name
                INTT_testbeam_BOXLV,  //its mother  volume
                false,  //no boolean operation
                0,  //copy number
                checkOverlaps);


        G4VPhysicalVolume *INTT_siLV_gap_PV =
              new G4PVPlacement(0,
                  G4ThreeVector(14*mm, 0, 0),
                  INTT_siLV_gap, //its logical volume
                  INTT_siLV_gap_name[l],  //its name
                  INTT_siLV_outer[l],  //its mother  volume
                  false,  //no boolean operation
                  0,  //copy number
                  checkOverlaps);    

	// the sillicon sensor part
        G4ThreeVector the_position; // needed?
        G4double xpos = 0.0, ypos = 0.0;
        for (G4int l1 = 0; l1 < 256; l1++)
        {

	  // for the silicon chips on the lower side
	  if (l1 < 128)
	      {
		ypos = (-9.961 + (l1 * 0.078)) *mm;

                for (G4int l2 = 0; l2 < 13; l2++)
                {
		  int type;
                    if (l2 < 8)
                    {

		      xpos = (-107.1 + (l2 *16)) * mm;
		      G4VPhysicalVolume *chip_channelsPV =
                            new G4PVPlacement(0,
                                G4ThreeVector(xpos, ypos, 0),
                                INTT_siLV_typeA,  //its logical volume
                                "INTT_siLV_all_typeA",  //its name
                                INTT_siLV_outer[l], //its mother  volume
                                false,  //no boolean operation
                                counting_number,  //copy number
                                false);

                        the_position = INTT_testbeam_BOXPV->GetTranslation()
			  + INTT_siLV_outer_allPV->GetTranslation()
			  + chip_channelsPV->GetTranslation();
			
			type = 0;
                        counting_number += 1;
                    }
                    else
                    {
                        xpos = (25.1 + ((l2 - 8) *20)) * mm;
                        G4VPhysicalVolume *chip_channelsPV =
                            new G4PVPlacement(0,
                                G4ThreeVector(xpos, ypos, 0),
                                INTT_siLV_typeB,  //its logical volume
                                "INTT_siLV_all_typeB",  //its name
                                INTT_siLV_outer[l], //its mother  volume
                                false,  //no boolean operation
                                counting_number,  //copy number
                                false);
                        the_position = INTT_testbeam_BOXPV->GetTranslation() +
                            INTT_siLV_outer_allPV->GetTranslation() +
                            chip_channelsPV->GetTranslation();
			type = 1;
                        counting_number += 1;
                    } // end of if( l2 < 8 )

		    // G4cout << "copy test : " << counting_number - 1
		    // 	   << " ID : " << l << " " << l1 << " " << l2
		    // 	   << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
		    // 	   << " up : " << 0
		    // 	   << " type : " << type
		    // 	   << G4endl;
		    
                }
            }
            else  // for the silicon chips on the upper side
            {

                ypos = (0.055 + ((l1 - 128) *0.078)) *mm;
                for (G4int l2 = 0; l2 < 13; l2++)
                {

		  int type;
                    if (l2 < 8)
                    {

                        xpos = (-107.1 + (l2 *16)) *mm;
                        G4VPhysicalVolume *chip_channelsPV =
                            new G4PVPlacement(0,
                                G4ThreeVector(xpos, ypos, 0),
                                INTT_siLV_typeA,  //its logical volume
                                "INTT_siLV_all_typeA",  //its name
                                INTT_siLV_outer[l], //its mother  volume
                                false,  //no boolean operation
                                counting_number,  //copy number
                                false);

                        the_position = INTT_testbeam_BOXPV->GetTranslation() +
                            INTT_siLV_outer_allPV->GetTranslation() +
                            chip_channelsPV->GetTranslation();
			type = 0;
                        counting_number += 1;
                    }
                    else
                    {

                        xpos = (25.1 + ((l2 - 8) *20)) *mm;
                        G4VPhysicalVolume *chip_channelsPV =
                            new G4PVPlacement(0,
                                G4ThreeVector(xpos, ypos, 0),
                                INTT_siLV_typeB,  //its logical volume
                                "INTT_siLV_all_typeB",  //its name
                                INTT_siLV_outer[l], //its mother  volume
                                false,  //no boolean operation
                                counting_number,  //copy number
                                false);

                        the_position = INTT_testbeam_BOXPV->GetTranslation() +
                            INTT_siLV_outer_allPV->GetTranslation() +
                            chip_channelsPV->GetTranslation();
			type = 1;
                        counting_number += 1;
                    }

		    // G4cout << "copy test : " << counting_number - 1
		    // 	   << " ID : " << l << " " << l1 << " " << l2
		    // 	   << " position : " << the_position[0] << " " << the_position[1] << " " << the_position[2]
		    // 	   << " up : " << 1 
		    // 	   << " type : " << type << G4endl;
		    
                } // end of for (G4int l2 = 0; l2 < 13; l2++)
            } // end of if (l1 < 128)
        } // end of for (G4int l1 = 0; l1 < 256; l1++)


	// the silver epoxy glue for the silcon chip type-A
	// move the position from the center of the silicon strip to the center of the glue
	zpos += this->kSilicon_strip_thickness / 2 + this->kSilver_epoxy_glue_FPHX_thickness / 2;
        G4VPhysicalVolume *INTT_siLV_glue_typeA_PV =
          new G4PVPlacement(0,
			    //G4ThreeVector(-51.1 * mm, 0, (zpos+25*um+160*um) - zpos_testbeam_box),
			    G4ThreeVector(-51.1 * mm, 0, zpos ),
              INTT_siLV_glue_typeA, //its logical volume
              INTT_siLV_glue_typeA_name[l],  //its name
              INTT_testbeam_BOXLV,  //its mother  volume
              false,  //no boolean operation
              0,  //copy number
              checkOverlaps);

	// the silver epoxy glue for the silcon chip type-A
        G4VPhysicalVolume *INTT_siLV_glue_typeB_PV =
          new G4PVPlacement(0,
			    //G4ThreeVector(65.1 * mm, 0, (zpos+25*um+160*um) - zpos_testbeam_box),
			    G4ThreeVector(65.1 * mm, 0, zpos ),
			    INTT_siLV_glue_typeB, //its logical volume
			    INTT_siLV_glue_typeB_name[l],  //its name
			    INTT_testbeam_BOXLV,  //its mother  volume
			    false,  //no boolean operation
			    0,  //copy number
			    checkOverlaps);  
	
    } // end block for for loop over all INTT ladders


    // for the thin trigger scintillator, to be put on the upstream surface of the darkbox
    G4LogicalVolume *INTT_sci_thinLV = new G4LogicalVolume(INTT_sci_thin, BBHodo_Scinti, "INTT_sci_thinLV");
    INTT_sci_thinLV->SetVisAttributes(color_scintillator);

    G4double zpos = -this->INTT_testbeam_BOX_size[2] / 2 - INTT_sci_thin_thickness / 2;
      
    new G4PVPlacement(0,
		      G4ThreeVector(0, 0, zpos ), 
		      INTT_sci_thinLV, //its logical volume
		      "INTT_sci_thinLV",  //its name
		      worldLog,  //its mother  volume
		      false,  //no boolean operation
		      0,  //copy number
		      checkOverlaps);

    // for the thicker trigger scintillators, to be put on the downstream surface of the darkbox
    G4LogicalVolume *INTT_sciLV = new G4LogicalVolume(INTT_sci, BBHodo_Scinti, "INTT_sciLV");
    INTT_sciLV->SetVisAttributes(color_scintillator);

    zpos = this->INTT_testbeam_BOX_size[2] / 2 + INTT_sci_thick_thickness / 2;
    new G4PVPlacement(0,
		      G4ThreeVector(0, 0, zpos ), 
        INTT_sciLV, //its logical volume
        "INTT_sciLV_middle",  //its name
        worldLog,  //its mother  volume
        false,  //no boolean operation
        1,  //copy number
        checkOverlaps);

    // to be written in better way
    G4double distance_middle_downstream_scintillators = 0.5 * mm;
    zpos += INTT_sci_thick_thickness + distance_middle_downstream_scintillators;
    new G4PVPlacement(0,
		      G4ThreeVector(0, 0, zpos ),
        INTT_sciLV, //its logical volume
        "INTT_sciLV_downstream",  //its name
        worldLog,  //its mother  volume
        false,  //no boolean operation
        2,  //copy number
        checkOverlaps);


    //for chip
    G4LogicalVolume *INTT_ChipLV = new G4LogicalVolume(INTT_Chip, Silicon, "INTT_ChipLV_name");
    INTT_ChipLV->SetVisAttributes(color_FPHX);
    G4LogicalVolume *INTT_Chip_glueLV = new G4LogicalVolume(INTT_Chip_glue, SilverEpoxyGlue, "INTT_Chip_glueLV_name");
    INTT_Chip_glueLV->SetVisAttributes(color_glue);

    G4LogicalVolume *INTT_Chip_areaLV = new G4LogicalVolume(INTT_Chip_area, Silicon, "INTT_Chip_areaLV_name");
    INTT_Chip_areaLV->SetVisAttributes(color_invisible);

    G4int counting_number_chip = 0;

    for (G4int l = 0; l < kLadder_num; l++)
    {

	G4double zpos = first_ladder_pos_z + l * kINTT_ladder_thickness + l * kGap + kFPHX_thickness / 2;

        for (G4int l1 = 0; l1 < 2; l1++)
        {
            for (G4int l2 = 0; l2 < 13; l2++)
            {
                if (l2 < 8) // not 12 but l2
                {

                    auto chip_position
                        = G4ThreeVector(-1 *(116.1 - 5 - 4.5 - l2 * 16.) *mm,
                            (19 - 3.775 - 1.5 - l1 * 27.45) *mm,
					//zpos - zpos_testbeam_box);
					zpos );
                    new G4PVPlacement(0,
                        chip_position,
                        INTT_ChipLV,  //its logical volume
                        "INTT_ChipLV_name", //its name
                        INTT_testbeam_BOXLV,  //its mother  volume
                        false,  //no boolean operation
                        counting_number_chip, //copy number
                        checkOverlaps);

                    auto chip_position_glue
                        = G4ThreeVector(-1 *(116.1 - 5 - 4.5 - l2 * 16.) *mm,
                            (19 - 3.775 - 1.5 - l1 * 27.45) *mm,
					//(zpos+160*um+25*um) - zpos_testbeam_box);
					(zpos + 160 * um + 25 * um) );

                    G4VPhysicalVolume *INTT_chip_gluePV =
                    new G4PVPlacement(0,
                        chip_position_glue,
                        INTT_Chip_glueLV, //its logical volume
                        "INTT_Chip_glueLV_name",  //its name
                        INTT_testbeam_BOXLV,  //its mother  volume
                        false,  //no boolean operation
                        0,  //copy number
                        checkOverlaps);


                }
                else
                {

                    auto chip_position
                        = G4ThreeVector(-1 *(116.1 - 141.7 - (l2 - 8) *20) *mm,
                            (19 - 3.775 - 1.5 - l1 * 27.45) *mm,
                            zpos - zpos_testbeam_box);
                    new G4PVPlacement(0,
                        chip_position,
                        INTT_ChipLV,  //its logical volume
                        "INTT_ChipLV_name", //its name
                        INTT_testbeam_BOXLV,  //its mother  volume
                        false,  //no boolean operation
                        counting_number_chip, //copy number
                        checkOverlaps);


                    auto chip_position_glue
                        = G4ThreeVector(-1 *(116.1 - 141.7 - (l2 - 8) *20) *mm,
                            (19 - 3.775 - 1.5 - l1 * 27.45) *mm,
					//(zpos+160*um+25*um) - zpos_testbeam_box);
					(zpos + 160 * um + 25 * um) );

                    G4VPhysicalVolume *INTT_chip_gluePV =
                    new G4PVPlacement(0,
                        chip_position_glue,
                        INTT_Chip_glueLV, //its logical volume
                        "INTT_Chip_glueLV_name",  //its name
                        INTT_testbeam_BOXLV,  //its mother  volume
                        false,  //no boolean operation
                        0,  //copy number
                        checkOverlaps);

                }

                counting_number_chip += 1;
            }
        }

        // INTT_ChipLV[l]
        //   = new G4LogicalVolume(INTT_Chip, Silicon, INTT_ChipLV_name[0]);  
    } // end block for for loop

    // loop over all INTT ladders to create HDI cables and the carbon staves
    for (G4int l = 0; l < kLadder_num; l++)
    {
        //for HDI copper
        INTT_HDI_copperLV[l] = new G4LogicalVolume(INTT_HDI_copper, Copper, INTT_HDI_copperLV_name[l]);
        INTT_HDI_copperLV[l]->SetVisAttributes(color_HDI_copper);

	G4double zpos = first_ladder_pos_z         // start from the position of the first ladder (upstream surface)
	  + l * kINTT_ladder_thickness + l * kGap   // move to the ladder position which we are working in this loop
	  + this->kSilicon_strip_thickness + this->kSilver_epoxy_glue_FPHX_thickness // skip the silicon strips and the silver epoxy glue
	  + HDI_copper_thickness / 2; // move to the center of the copper layer of the HDI

        new G4PVPlacement(0,
            G4ThreeVector(0, 0, zpos - zpos_testbeam_box),
            INTT_HDI_copperLV[l], //its logical volume
            INTT_HDI_copperLV_name[l],  //its name
            INTT_testbeam_BOXLV,  //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        //for HDI Kapton
        INTT_HDI_KaptonLV[l] = new G4LogicalVolume(INTT_HDI_Kapton, Kapton, INTT_HDI_KaptonLV_name[l]);
        INTT_HDI_KaptonLV[l]->SetVisAttributes(color_HDI_kapton);

	zpos += HDI_copper_thickness / 2 +  HDI_kapton_thickness / 2;
        new G4PVPlacement(0,
            G4ThreeVector(0, 0, zpos - zpos_testbeam_box),
            INTT_HDI_KaptonLV[l], //its logical volume
            INTT_HDI_KaptonLV_name[l],  //its name
            INTT_testbeam_BOXLV,  //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // mother volume of the carbon stave:
        //              flat CFRP plate, rohacell forms, CFRP tube, cooling water, and formed CFRP plate
        G4String stave_areaLV_name = "INTT_stave_area" + to_string(l + 1);
        INTT_stave_areaLV[l] = new G4LogicalVolume(INTT_stave_area, DefaultMaterial, stave_areaLV_name);
        INTT_stave_areaLV[l]->SetVisAttributes(color_invisible);

	zpos += HDI_kapton_thickness / 2 + INTT_stave_area_thickness / 2;
        new G4PVPlacement(0,
            //G4ThreeVector(0, 9.725*mm, zpos- zpos_testbeam_box), 
            G4ThreeVector(0, 0, zpos - zpos_testbeam_box),
            INTT_stave_areaLV[l], //its logical volume
            stave_areaLV_name,
            INTT_testbeam_BOXLV,  //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        //for flat carbon fiber layer
        INTT_CFRPLVf[l] = new G4LogicalVolume(INTT_CFRP, CFRP, INTT_CFRPLVf_name[l]);
        INTT_CFRPLVf[l]->SetVisAttributes(color_CFRP_carbon);

        G4double xpos_local = 0.0;
        G4double ypos_local = 0.0;
        G4double zpos_local = -INTT_stave_area_thickness / 2 + kINTT_CFRP_thickness / 2;
        new G4PVPlacement(0,
            ///     G4ThreeVector(0, 0, zpos- zpos_testbeam_box), 
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_CFRPLVf[l],  //its logical volume
            INTT_CFRPLVf_name[l], //its name
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        //for formed carbon fiber layer
        INTT_CFRPLVr[l] = new G4LogicalVolume(INTT_CFRP, CFRP, INTT_CFRPLVr_name[l]);
        INTT_CFRPLVr[l]->SetVisAttributes(color_CFRP_carbon);

        // the formed carbon, short outer part on the top
        G4String formed_CFRP_outer_partLV_name = "INTT_formed_carbonfiber_outer_top" + to_string(l + 1);
        INTT_formed_CFRP_outer_partLV[l] = new G4LogicalVolume(INTT_formed_CFRP_outer_part, CFRP, formed_CFRP_outer_partLV_name);
        INTT_formed_CFRP_outer_partLV[l]->SetVisAttributes(color_CFRP_carbon);

        ypos_local = kLadder_vertical_length / 2 - INTT_formed_CFRP_outer_length / 2;
        zpos_local = -INTT_stave_area_thickness / 2 + kINTT_CFRP_thickness *3 / 2;
        new G4PVPlacement(0,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_formed_CFRP_outer_partLV[l], //its logical volume
            formed_CFRP_outer_partLV_name,
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // the formed carbon, short outer part on the bottom
        formed_CFRP_outer_partLV_name = "INTT_formed_carbonfiber_outer_bottom" + to_string(l + 1);
        ypos_local = -ypos_local;
        new G4PVPlacement(0,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_formed_CFRP_outer_partLV[l], //its logical volume
            formed_CFRP_outer_partLV_name,
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // the formed carbon, slantpart on the upper side
        G4String slant_upper_name = "INTT_formed_carbonfiber_slant_upper" + to_string(l + 1);
        INTT_formed_CFRP_slant_partLV[l] = new G4LogicalVolume(INTT_formed_CFRP_slant,
            CFRP, slant_upper_name);
        INTT_formed_CFRP_slant_partLV[l]->SetVisAttributes(color_CFRP_carbon);

        ypos_local = INTT_formed_CFRP_straight_length / 2;
        zpos_local = -INTT_stave_area_thickness / 2 + INTT_formed_CFRP_thickness;
        new G4PVPlacement(tubeRotation,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_formed_CFRP_slant_partLV[l], //its logical volume
            slant_upper_name, //its name
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // the formed carbon, slantpart on the lower side
        G4String slant_lower_name = "INTT_formed_carbonfiber_slant_lower" + to_string(l + 1);
        INTT_formed_CFRP_slant_partLV[l] = new G4LogicalVolume(INTT_formed_CFRP_slant,
            CFRP, slant_lower_name);
        INTT_formed_CFRP_slant_partLV[l]->SetVisAttributes(color_CFRP_carbon);

        ypos_local = -INTT_formed_CFRP_straight_length / 2;
        G4RotationMatrix *slant_lower_rotation = new G4RotationMatrix();
        slant_lower_rotation->rotateY(90.0 *deg);
        slant_lower_rotation->rotateX(180.0 *deg);

        new G4PVPlacement(slant_lower_rotation,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_formed_CFRP_slant_partLV[l], //its logical volume
            slant_lower_name, //its name
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // the formed carbon, straight part on the center
        G4String formed_CFRP_straightLV_name = "INTT_formed_carbonfiber_straight" + to_string(l + 1);
        INTT_formed_CFRP_straightLV[l] = new G4LogicalVolume(INTT_formed_CFRP_straight, CFRP, formed_CFRP_straightLV_name);
        INTT_formed_CFRP_straightLV[l]->SetVisAttributes(color_CFRP_carbon);

        ypos_local = 0;
        zpos_local = INTT_stave_area_thickness / 2 - kINTT_CFRP_thickness / 2;
        new G4PVPlacement(0,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_formed_CFRP_straightLV[l], //its logical volume
            formed_CFRP_straightLV_name,
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // the rohacel form
        G4String form_name = "INTT_form" + to_string(l + 1);
        INTT_foam_LV[l] = new G4LogicalVolume(INTT_foam, foam, form_name);
        INTT_foam_LV[l]->SetVisAttributes(color_CFRP_foam);

        ypos_local = 0;
        //  zpos_local = -INTT_stave_area_thickness / 2 + INTT_CFRP_thickness + INTT_CFRP_tube_outer_radius;
        zpos_local = -INTT_stave_area_thickness / 2 + kINTT_CFRP_thickness;
        new G4PVPlacement(foam_rotation,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_foam_LV[l],  //its logical volume
            form_name,  //its name
            INTT_stave_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        ////////////////////////////////////////////////////////////
        // mother volume of the CFRP tube and water, it's filled with epoxy, it means glue
        G4String tube_area_name = "INTT_carbonfiber_tube_areaLV" + to_string(l + 1);
        INTT_CFRP_tube_areaLV[l] = new G4LogicalVolume(INTT_CFRP_tube_area,
            G4Material::GetMaterial("Epoxy"), tube_area_name);
        INTT_CFRP_tube_areaLV[l]->SetVisAttributes(color_glue);

        xpos_local = INTT_CFRP_tube_area_thickness / 2;
        ypos_local = 0.0;
        zpos_local = 0.0;
        new G4PVPlacement(tubeRotation,
            G4ThreeVector(xpos_local, ypos_local, zpos_local),
            INTT_CFRP_tube_areaLV[l], //its logical volume
            tube_area_name, //its name
            INTT_foam_LV[l],  //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // CFRP tube
        INTT_CFRP_tubeLV[l] = new G4LogicalVolume(INTT_CFRP_tube, CFRP, INTT_CFRP_tubeLV_name[l]);
        INTT_CFRP_tubeLV[l]->SetVisAttributes(color_CFRP_carbon);

        new G4PVPlacement(tubeRotation,
            G4ThreeVector(0, 0, 0),
            INTT_CFRP_tubeLV[l],  //its logical volume
            INTT_CFRP_tubeLV_name[l], //its name
            INTT_CFRP_tube_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // cooling water
        INTT_waterLV[l] = new G4LogicalVolume(INTT_water, Water, INTT_waterLV_name[l]);
        INTT_waterLV[l]->SetVisAttributes(color_CFRP_water);

        new G4PVPlacement(tubeRotation,
            G4ThreeVector(0, 0, 0),
            INTT_waterLV[l],  //its logical volume
            INTT_waterLV_name[l], //its name
            INTT_CFRP_tube_areaLV[l], //its mother  volume
            false,  //no boolean operation
            0,  //copy number
            checkOverlaps);

        // end of the tube area
        ////////////////////////////////////////////////////////////
    } // end block for for loop

    //always return the physical World
    return worldPV;
}

void EDDetectorConstruction::ConstructSDandField()
{
    // EDChamberSD*INTT_siSD[4];
    // G4String INTT_siSD_name[4]={"INTT_siSD_1","INTT_siSD_2","INTT_siSD_3","INTT_siSD_4"};
    // G4String INTT_siSD_HitsCollection_name[4]={"INTT_siSD_HitsCollection_1","INTT_siSD_HitsCollection_2","INTT_siSD_HitsCollection_3","INTT_siSD_HitsCollection_4"};
    // G4String INTT_siLV_name[4]={"INTT_siLV1","INTT_siLV2","INTT_siLV3","INTT_siLV4"};

    EDChamberSD *chamber1SD = new EDChamberSD("Chamber1SD", "Chamber1HitsCollection", 0);
    G4SDManager::GetSDMpointer()->AddNewDetector(chamber1SD);
    SetSensitiveDetector("INTT_siLV_all_typeA", chamber1SD);

    EDChamberSD *chamber2SD = new EDChamberSD("Chamber2SD", "Chamber2HitsCollection", 0);
    G4SDManager::GetSDMpointer()->AddNewDetector(chamber2SD);
    SetSensitiveDetector("INTT_siLV_all_typeB", chamber2SD);

    EDEmCalorimeterSD *calorimeterSD1 = new EDEmCalorimeterSD("EmCalorimeterSD1", "EmCalorimeterHitsCollection1");
    G4SDManager::GetSDMpointer()->AddNewDetector(calorimeterSD1);
    SetSensitiveDetector("INTT_sci_thinLV", calorimeterSD1);
    
    EDEmCalorimeterSD *calorimeterSD = new EDEmCalorimeterSD("EmCalorimeterSD", "EmCalorimeterHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(calorimeterSD);
    SetSensitiveDetector("INTT_sciLV", calorimeterSD);
    
    // EDChamberSD* trigger_1 = new EDChamberSD("Trigger_1", "Trigger_1HitsCollection", 1);
    // G4SDManager::GetSDMpointer()->AddNewDetector(trigger_1);
    // SetSensitiveDetector("INTT_sciLV1", trigger_1);

    /*for (int i2=0; i2 < 4; i2++)
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

void EDDetectorConstruction::DefineVisAttributes()
{

    color_invisible		= new G4VisAttributes(false	, G4Colour(0.0, 0.000, 0.0, 0.0)	);
    color_silicon_active	= new G4VisAttributes(true	, G4Colour(1.0, 0.000, 0.0, 0.5)	); // transparent red
    color_silicon_inactive	= new G4VisAttributes(true	, G4Colour(0.0, 0.000, 1.0, 0.5)	); // transparent blue
    color_glue			= new G4VisAttributes(true	, G4Colour(0.1, 0.100, 0.1, 0.4)	);
    color_FPHX			= new G4VisAttributes(true	, G4Colour(1.0, 0.843, 0.0, 0.5)	); // HTML gold
    color_HDI_copper		= new G4VisAttributes(true	, G4Colour(0.7, 0.400, 0.0, 0.5)	); // brown
    color_HDI_kapton		= new G4VisAttributes(true	, G4Colour(0.0, 0.590, 1.0, 0.5)	); // blue
    color_CFRP_carbon		= new G4VisAttributes(true	, G4Colour(0.4, 0.400, 0.4, 1.0)	);  // gray
    color_CFRP_water		= new G4VisAttributes(true	, G4Colour::Blue()			);
    color_CFRP_foam		= new G4VisAttributes(true	, G4Colour(0.9, 0.900, 0.9, 0.5)	);  // white
    color_scintillator		= new G4VisAttributes(true	, G4Colour(0.0, 0.000, 0.6, 0.5)	); // blue?
    color_darkbox_wall		= new G4VisAttributes(true	, G4Colour(1.0, 1.000, 1.0, 0.1)	); // transparent black
}
