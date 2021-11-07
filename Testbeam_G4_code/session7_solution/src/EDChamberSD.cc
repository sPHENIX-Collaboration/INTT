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
/// \file EDChamberSD.cc
/// \brief Implementation of the EDChamberSD class
//
#include "G4MTRunManager.hh"
#include "EDChamberSD.hh"
#include "EDAnalysis.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4AutoLock.hh"
#include "EDDetectorConstruction.hh"
#include "public_variable.hh"
//#include "Encoder64Cal.hh"
//#include "VSensitiveDetector.hh"

namespace { 
   G4Mutex myGlobalValueMutex = G4MUTEX_INITIALIZER;
}

G4double* myGlobalValue = new G4double(1.);

EDChamberSD::EDChamberSD(const G4String& name, 
                         const G4String& hitsCollectionName,
                         G4int ntupleId
                         )
 : G4VSensitiveDetector(name),
   fHitsCollection(0),
   fNtupleId(ntupleId)
   //HCID1(-1)
{
  collectionName.insert(hitsCollectionName);
  //VEncoder*theEncoder = new Encoder64Cal();
}

EDChamberSD::~EDChamberSD()
{}

void EDChamberSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection 
    = new EDChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    //G4cout<<"the name test"<<collectionName[0]<<G4endl;

  // Add this collection in hce
  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection); 

  G4int upordown=2;
  G4int xposition=13;
  G4int yposition=256;
  G4int zposition=4;
  G4int silicon_type=2;
  G4int copyNo_counting=0;

for (G4int i1=0; i1<zposition; i1++)
    {
      for (G4int i2=0; i2<yposition; i2++)
        {
          if (i2<128) //Down, U14 ~ U26
            {
              for (G4int i3=0; i3<xposition; i3++)
                {
                  if (i3<8)
                    {
                      EDChamberHit *newHit = new EDChamberHit();
                      newHit->SetEncoderID(0,0,i1,i3,i2);
                      fHitsCollection->insert(newHit);
                      //G4cout<<"TESTChmaberSD : "<<copyNo_counting<<" "<<i1<<" "<<i2<<" "<<i3<<" up : "<<0<<" type : "<<0<<G4endl;
                      copyNo_counting+=1;

                    }
                  else 
                    {
                      EDChamberHit *newHit = new EDChamberHit();
                      newHit->SetEncoderID(0,1,i1,i3,i2);
                      fHitsCollection->insert(newHit);
                      //G4cout<<"TESTChmaberSD : "<<copyNo_counting<<" "<<i1<<" "<<i2<<" "<<i3<<" up : "<<0<<" type : "<<1<<G4endl;
                      copyNo_counting+=1;
                    }  
                }
            }
          else //upper, U1 ~ U13 
            {
              for (G4int i3=0; i3<xposition; i3++)
                {
                  if (i3<8)
                    {
                      EDChamberHit *newHit = new EDChamberHit();
                      newHit->SetEncoderID(1,0,i1,i3,i2-128);
                      fHitsCollection->insert(newHit);
                      //G4cout<<"TESTChmaberSD : "<<copyNo_counting<<" "<<i1<<" "<<i2<<" "<<i3<<" up : "<<1<<" type : "<<0<<G4endl;
                      copyNo_counting+=1;
                    }
                  else 
                    {
                      EDChamberHit *newHit = new EDChamberHit();
                      newHit->SetEncoderID(1,1,i1,i3,i2-128);
                      fHitsCollection->insert(newHit);
                      //G4cout<<"TESTChmaberSD : "<<copyNo_counting<<" "<<i1<<" "<<i2<<" "<<i3<<" up : "<<1<<" type : "<<1<<G4endl;
                      copyNo_counting+=1;
                    }
                }
            }  

        }
    }


  // for (G4int i1=0; i1<upordown; i1++)
  //   {
  //     for (G4int i2=0; i2<silicon_type; i2++)
  //       {
  //         for(G4int i3=0; i3<zposition; i3++)
  //           {
  //             for(G4int i4=0; i4<xposition; i4++)
  //               {
  //                 for(G4int i5=0; i5<yposition; i5++)
  //                   {
  //                     EDChamberHit *newHit = new EDChamberHit();
  //                     newHit->SetEncoderID(i1,i2,i3,i4,i5);
  //                     fHitsCollection->insert(newHit);
  //                   }
  //               }
  //           }
  //       }
  //   }
}

G4bool EDChamberSD::ProcessHits(G4Step* step, 
                                G4TouchableHistory* /*history*/)
{

  G4int eID_1=0;
  const G4Event* evt_1=G4MTRunManager::GetRunManager()->GetCurrentEvent();
  if (evt_1) eID_1=evt_1->GetEventID();

  G4AutoLock myLock(&myGlobalValueMutex); 
  // simulate data race
  if ( (*myGlobalValue) > 0.) {
    delete myGlobalValue;
    myGlobalValue = new G4double(-1);
  }
  else  {
    delete myGlobalValue;
    myGlobalValue = new G4double(1);
  }  
  myLock.unlock();
  const G4VTouchable* touchable
    = step->GetPreStepPoint()->GetTouchable();
  G4VPhysicalVolume* motherPhysical = touchable->GetVolume(0); // mother

  G4String motherPhysical_name_test = motherPhysical->GetName(); //step->GetPreStepPoint()->GetTouchable()->GetVolume(0)->GetName();
  if (eID_1%10000==0)G4cout<<"!!!! pre_TESTnameTEST : "<<motherPhysical_name_test<<G4endl;
  
  G4int copyNo =  step->GetPreStepPoint()->GetTouchable()->GetVolume(0)-> GetCopyNo();   //motherPhysical->GetCopyNo();   <- this is the original one
  if (eID_1%10000==0)G4cout<<"!!!! pre_getcopynumberTEST : "<<copyNo<<G4endl;
  // G4ThreeVector preStepPoint_object_position = step->GetPreStepPoint()->GetTouchable()->GetVolume(0)->GetPosition();
  // if (eID_1%10000==0)G4cout<<"!!!! pre_getcopynumberTEST position: "<<preStepPoint_object_position[0]<<" "<<preStepPoint_object_position[1]<<" "<<preStepPoint_object_position[2]<<" "<<G4endl;

  //newHit->SetLayerNumber(copyNo);
  //G4int PID = Control::GetControl()->GetPIDForCalHit(step);
  G4int PDG= step->GetTrack()->GetDefinition()->GetPDGEncoding();
  G4int particle_order = step->GetTrack() -> GetTrackID();
  

  G4String volume_test= step->GetTrack()->GetVolume()->GetName(); // !!maybe!! it is from poststepposition
  //G4String volume_test_2= step->GetPostStepPoint()->GetTouchable()->GetVolume(1)->GetName();
  G4int post_copyNo = step->GetTrack()->GetVolume()->GetCopyNo();
  if (eID_1%10000==0)G4cout<<"!!!! pos_TESTnameTEST : "<<volume_test<<G4endl;
  //G4cout<<"!!!! pos_TESTnameTEST2 : "<<volume_test_2<<G4endl;
  if (eID_1%10000==0)G4cout<<"!!!! pos_getcopynumberTEST : "<<post_copyNo<<G4endl;
  if (eID_1%10000==0)G4cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ "<<G4endl;

  G4double posX = step->GetTrack()->GetPosition().x();
  G4double posY = step->GetTrack()->GetPosition().y();
  G4double posZ = step->GetTrack()->GetPosition().z();
  G4double edep = step->GetTotalEnergyDeposit(); // it doesn't be classified as prePosition or posPosition

  G4double secondaryParticleKineticEnergy =  step->GetTrack()->GetKineticEnergy(); 
  if (eID_1%10000==0)G4cout<<"Pos event ID : "<<eID_1<<"  particle order : "<<particle_order<<"        PID "<<PDG<<"         energy : "<<secondaryParticleKineticEnergy<<"     volume : "<<volume_test<< "  position : " << posX<<"||"<<posY<<"||"<<posZ <<" copy number : "<<copyNo<<G4endl;
  
  //if (volume_test=="INTT_siLV_all_typeA") {cout<<"testing "<<volume_test<<" "<<0<<endl;}
  //else if (volume_test=="INTT_siLV_all_typeB") {cout<<"testing "<<volume_test<<" "<<1<<endl;}
  
  

  //==========================================from Mokka========================================================

  // energy deposit
  //G4double edep = step->GetTotalEnergyDeposit();
  
  if ( edep==0. ) return false;
  G4double time = step->GetTrack()->GetGlobalTime() ;
  //based on my memory, the GetPreStepPoint is the first, and the GetPostStepPoint is the end position
  G4ThreeVector touchposition = step->GetPreStepPoint()->GetPosition(); //this is the postion when particle hit on the material
  if (eID_1%10000==0)G4cout<<" GetPreStepPointTEST : "<<touchposition[0]<<" "<<touchposition[1]<<" "<<touchposition[2]<<G4endl;
  
  G4ThreeVector the_post_step_position = step->GetPostStepPoint()->GetPosition();
  if (eID_1%10000==0)G4cout<<" GetPostStepPointTEST "<<the_post_step_position[0]<<" "<<the_post_step_position[1]<<" "<<the_post_step_position[2]<<" "<<G4endl;
  
  G4ThreeVector thePosition =(step->GetPreStepPoint()->GetPosition()+step->GetPostStepPoint()->GetPosition())*0.5;
  if (eID_1 % 10000 == 0)G4cout << "Mid event ID : " << eID_1 << "  particle order : " << particle_order << "        PID " << PDG << "         edep : " << edep << "     volume : " << volume_test << "  position : " << thePosition[0] << "||" << thePosition[1] << "||" << thePosition[2] << G4endl;
  if (eID_1 % 10000 == 0)G4cout << "=======================================" << G4endl;
      //G4cout<<"GGGGGGGGGGGGGGGGGG "<<thePosition[0]<<" "<<thePosition[1]<<" "<<thePosition[2]<<" ENERGY : "<<edep<<" PID "<<PDG<<G4endl;
  G4ThreeVector columnPosition;
  G4double      phiposition_S1;
  G4double Zseparation;
  G4double Zseparation_1; 
  G4int material;
  G4int side ;
  G4int xposition;
  G4int yposition;
  G4int zposition;
  G4bool found=false;
  G4int n_hit = fHitsCollection->entries();
  G4int silicon_type;
  G4int upordown;
  G4int volume_test_ID=0;
  
  if (volume_test=="INTT_siLV_all_typeA") {volume_test_ID=0;}
  else if (volume_test=="INTT_siLV_all_typeB") {volume_test_ID=1;}

  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(2, 0, particle_order);
  analysisManager->FillNtupleIColumn(2, 1, PDG);
  analysisManager->FillNtupleDColumn(2, 2, secondaryParticleKineticEnergy);//Total energy, not edep
  analysisManager->FillNtupleDColumn(2, 3, posX); //posPosition
  analysisManager->FillNtupleDColumn(2, 4, posY); //posPosition
  analysisManager->FillNtupleDColumn(2, 5, posZ); //posPosition
  analysisManager->FillNtupleIColumn(2, 6, volume_test_ID);
  analysisManager->FillNtupleIColumn(2, 7, eID_1);
  analysisManager->AddNtupleRow(2);



  //==========================================keep========================================================
  //G4int PID = Control::GetControl()->GetPIDForCalHit(step);
  //assert(PID!=-1);
  //G4int PDG= step->GetTrack()->GetDefinition()->GetPDGEncoding();
  //==========================================keep========================================================

  //columnPosition[0] = r
  //columnPosition[1] = phi
  //columnPosition[2] = z

  //if((fabs(thePosition[2])-510.345) < 0 ) // part one or part two
  // if (fabs(thePosition[1])<10. && thePosition[2]>0.)
  //   {
      

  //     if (thePosition[0]>-115.1 && thePosition[0]<12.9)
  //      {
         

  //        if (thePosition[1]>-10. && thePosition[1]<-0.016)
  //          {
  //            upordown=1; 
  //            silicon_type=1;
  //            xposition=ceil((thePosition[0]+115.1)/16.);

  //            zposition=ceil((fabs(thePosition[2])-99.84)/35.);  

  //            yposition=ceil((thePosition[1]+10.)/0.078);

  //            //sensorposition[0] = -107.1+(xposition-1)*16.;
  //            //sensorposition[1] = -9.961+(yposition-1)*0.078;
  //            //sensorposition[2] = 100.+(zposition-1)*35.;
  //          }
  //        else if ( thePosition[1]>0.016 && thePosition[1]<10. ) 
  //          {
  //            upordown=0; 
  //            silicon_type=1;
  //            xposition=ceil((thePosition[0]+115.1)/16.);

  //            zposition=ceil((fabs(thePosition[2])-99.84)/35.);

  //            yposition=ceil((thePosition[1]-0.016)/0.078); 

  //            //sensorposition[0] = -107.1+(xposition-1)*16.;
  //            //sensorposition[1] = 0.055+(yposition-1)*0.078;
  //            //sensorposition[2] = 100.+(zposition-1)*35.;
  //          }
  //        else {} //found = true;   

         

  //      }
  //     else if (thePosition[0]>15.1 && thePosition[0]<115.1)
  //       {

  //        if (thePosition[1]>-10. && thePosition[1]<-0.016)
  //          { 
  //            upordown=1;
  //            silicon_type=0;
  //            xposition=ceil((thePosition[0]-15.1)/20.);

  //            zposition=ceil((fabs(thePosition[2])-99.84)/35.);  

  //            yposition=ceil((thePosition[1]+10.)/0.078);
             
  //            //sensorposition[0] = 25.1+(xposition-1)*20.;
  //            //sensorposition[1] = -9.961+(yposition-1)*0.078;
  //            //sensorposition[2] = 100.+(zposition-1)*35.;
  //          }
  //        else if ( thePosition[1]>0.016 && thePosition[1]<10. ) 
  //          {
  //            upordown=0;
  //            silicon_type=0;
  //            xposition=ceil((thePosition[0]-15.1)/20.);

  //            zposition=ceil((fabs(thePosition[2])-99.84)/35.);

  //            yposition=ceil((thePosition[1]-0.016)/0.078); 

  //            //sensorposition[0] = 25.1+(xposition-1)*20.;
  //            //sensorposition[1] = 0.055+(yposition-1)*0.078;
  //            //sensorposition[2] = 100.+(zposition-1)*35.; 
  //          }
  //        else {}//found = true;  



  //       }
  //     else {}//found = true;
  //   }
  // else 
  //   {
  //     //found = true;
  //   }

  //==========================================from Mokka========================================================

    //G4cout<<"Iamtest~~~~~~~~"<<upordown<<" "<<silicon_type<<" "<<xposition<<" "<<yposition<<" "<<zposition<<G4endl;


  // Change the following line to get the charge of the tracked particle
  //G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
  //if ( charge == 0. ) return false;

  //G4double edep = step->GetTotalEnergyDeposit();
  //if ( edep==0. ) return false;
  
  // Create new hit
  //EDChamberHit* newHit = new EDChamberHit();
  
  // Layer number
  // = copy number of mother volume
  /*G4StepPoint* preStepPoint = step->GetPreStepPoint();
  const G4VTouchable* touchable
    = step->GetPreStepPoint()->GetTouchable();*/
  

  // Time
  //G4double time = preStepPoint->GetGlobalTime();
  //newHit->SetTime(time);
 
  // Position
  //G4ThreeVector position = preStepPoint->GetPosition();
  //newHit->SetPosition(position);
 
  // Add hit in the collection
  //fHitsCollection->insert(newHit);

  // Add hits properties in the ntuple
  /*G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleIColumn(fNtupleId, 0, copyNo);
  analysisManager->FillNtupleDColumn(fNtupleId, 1, position.x());
  analysisManager->FillNtupleDColumn(fNtupleId, 2, position.y());
  analysisManager->FillNtupleDColumn(fNtupleId, 3, position.z());
  analysisManager->FillNtupleDColumn(fNtupleId, 4, edep);
  analysisManager->FillNtupleDColumn(fNtupleId, 5, eID);  
  analysisManager->AddNtupleRow(fNtupleId); */ 
  // G4int subX=8;
  // G4int subY=128;
  // G4int subZ=4;
  // G4int Hit_object_number;

  //Hit_object_number=(zposition-1)*subX*subY+ (xposition-1)*subY   +(yposition-1);
  EDChamberHit* HitThisUnit = (*fHitsCollection)[copyNo];
  HitThisUnit->AddEdep(edep);


  // if (upordown==0 && silicon_type==0)
  //   {
  //     //G4cout<<" 1test~~~~~~~~~~~~~ "<<upordown<<" "<<silicon_type<<G4endl;
  //     Hit_object_number=(zposition-1)*subX*subY+ (xposition-1)*subY   +(yposition-1);
  //     EDChamberHit* HitThisUnit = (*fHitsCollection)[Hit_object_number];
  //     HitThisUnit->AddEdep(edep);
  //   }
  // else if (upordown==0 && silicon_type==1)
  //   {
  //     //G4cout<<"anothertest "<<zposition<<" "<<xposition<<" "<<yposition<<G4endl;
  //     Hit_object_number=3328+(zposition-1)*subX*subY+(xposition-1)*subY+(yposition-1);
  //     //G4cout<<" 2test~~~~~~~~~~~~~ "<<upordown<<" "<<silicon_type<<" "<<Hit_object_number<<G4endl;
  //     EDChamberHit* HitThisUnit = (*fHitsCollection)[Hit_object_number];
  //     HitThisUnit->AddEdep(edep);
  //   }
  // else if (upordown==1 && silicon_type==0)
  //   {
  //     //G4cout<<" 3test~~~~~~~~~~~~~ "<<upordown<<" "<<silicon_type<<G4endl;
  //     Hit_object_number=6656+(zposition-1)*subX*subY+(xposition-1)*subY+(yposition-1);
  //     EDChamberHit* HitThisUnit = (*fHitsCollection)[Hit_object_number];
  //     HitThisUnit->AddEdep(edep);
  //   }  
  // else if (upordown==1 && silicon_type==1)
  //   {
  //     //G4cout<<" 4test~~~~~~~~~~~~~ "<<upordown<<" "<<silicon_type<<G4endl;
  //     Hit_object_number=9984+(zposition-1)*subX*subY+(xposition-1)*subY+(yposition-1);
  //     EDChamberHit* HitThisUnit = (*fHitsCollection)[Hit_object_number];
  //     HitThisUnit->AddEdep(edep);
  //   }

  //= layerNo*funitofside*funitofside + RowNo*funitofside + ColumnNo;
  //EDScintillatorHit* HitThisUnit = (*fHitsCollection)[Hit_object_number];
  // HitThisUnit->SetCoordinate(layerNO,RowNO,ColumnNo);
  
  return false;
}

void EDChamberSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  //  std::cerr << "void EDChamberSD::EndOfEvent(G4HCofThisEvent* /*hce*/)";
  
  G4int eID=0;
  const G4Event* evt=G4MTRunManager::GetRunManager()->GetCurrentEvent();
  if (evt) eID=evt->GetEventID();

  G4ThreeVector sensorposition;
  //G4double theoffset_chamber = 

  //G4cout<<" EDChamberSD offset TEST : "<<public_variable::theoffset<<G4endl;


  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4int n_hit = fHitsCollection->entries();
  //G4cout<<"TESTTESTTEST number of total hits"<<n_hit<<G4endl;
  G4ThreeVector the_edep_position;

  for (G4int i = 0; i < n_hit; i++) {
      G4double fEdep = (*fHitsCollection)[i]->GetEnergyDeposit();
      //G4cout<<"whoareyou~~~~~~ "<<i<<" "<<fEdep<<G4endl;
      if (fEdep > 0.) {

        G4int fupordown = (*fHitsCollection)[i]->Getupordown();
        G4int fxposition = (*fHitsCollection)[i]->Getxposition();
        G4int fyposition = (*fHitsCollection)[i]->Getyposition();
        G4int fzposition = (*fHitsCollection)[i]->Getzposition();
        G4int fsilicon_type = (*fHitsCollection)[i]->Getsilicon_type();
        //the_edep_position[0]=(*fHitsCollection)[i] -> GetObjectTranslation (0);
        //the_edep_position[1]=(*fHitsCollection)[i] -> GetObjectTranslation (1);
        //the_edep_position[2]=(*fHitsCollection)[i] -> GetObjectTranslation (2);

        if (fupordown==1 && fsilicon_type ==1)
          {
               sensorposition[0] = 25.1+(fxposition-8)*20.;
               sensorposition[1] = 0.055+(fyposition-0)*0.078;
               sensorposition[2] = 100.+(fzposition-0)*35.;             
          }
        else if (fupordown==1 && fsilicon_type ==0)
          {
               sensorposition[0] = -107.1+(fxposition-0)*16.;
               sensorposition[1] = 0.055+(fyposition-0)*0.078;
               sensorposition[2] = 100.+(fzposition-0)*35.;
          } 
        else if (fupordown==0 && fsilicon_type ==1)
          {
               sensorposition[0] = 25.1+(fxposition-8)*20.;
               sensorposition[1] = -9.961+(fyposition-0)*0.078;
               sensorposition[2] = 100.+(fzposition-0)*35.;
          }   
        else if (fupordown==0 && fsilicon_type ==0) 
          {
               sensorposition[0] = -107.1+(fxposition-0)*16.;
               sensorposition[1] = -9.961+(fyposition-0)*0.078;
               sensorposition[2] = 100.+(fzposition-0)*35.;            
          } 
	
        //G4cout<<"particle hit ID 1:      "<<i<<" "<<fupordown<<" "<<fsilicon_type<<G4endl;
        //G4cout<<"particle hit ID 2:      "<<fxposition<<" "<<fyposition<<" "<<fzposition<<G4endl;
        //G4cout<<"particle hit position : "<<sensorposition[0]<<" "<<sensorposition[1]<<" "<<sensorposition[2]<<" ENERGY deposit : "<<fEdep<<G4endl;
        //G4cout<<" "<<G4endl;
        // (*fHitsCollection)[i]->Print(); 

        analysisManager->FillNtupleIColumn(fNtupleId, 0, eID);
        analysisManager->FillNtupleIColumn(fNtupleId, 1, fupordown);
        analysisManager->FillNtupleIColumn(fNtupleId, 2, fxposition);
        analysisManager->FillNtupleIColumn(fNtupleId, 3, fyposition);
        analysisManager->FillNtupleIColumn(fNtupleId, 4, fzposition);
        analysisManager->FillNtupleIColumn(fNtupleId, 5, fsilicon_type);
        analysisManager->FillNtupleDColumn(fNtupleId, 6, fEdep);        
        analysisManager->AddNtupleRow(fNtupleId);

        if ( eID % 10000 == 0)//if (1==1)//
          {
            G4cout << "Event Process: " << eID << G4endl;
            G4cout<<"particle hit ID 1:      "<<i<<" "<<" positionID : "<<  fxposition<<" "<<fyposition<<" "<<fzposition<<" up : "  <<fupordown<<" type : "<<fsilicon_type<<G4endl;
            //G4cout<<"particle hit ID 2:      "<<fxposition<<" "<<fyposition<<" "<<fzposition<<G4endl;
            G4cout<<"particle hit position : "<<sensorposition[0]<<" "<<sensorposition[1]<<" "<<sensorposition[2]<<" ENERGY deposit : "<<fEdep<<G4endl;
            //G4cout<<"positoin cout test : "<< the_edep_position[0]<<" "<<the_edep_position[1]<<" "<<the_edep_position[2]<<endl;
            G4cout<<" "<<G4endl;
          }
        
      }
    }

  //std::cerr << "     ------> ends" << std::endl;
}
