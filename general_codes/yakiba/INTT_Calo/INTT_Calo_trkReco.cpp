#include "INTT_Calo_trkReco.h"

void LoadTrackerData(void) {
  // Set Beam center
  xBC=-0.018;
  yBC=0.126;
  
  // clear vectors of the hits
  vEmc_r.clear();
  vEmc_phi.clear();
  vEmc_z.clear();
  vEmc_e.clear();

  vTop_r.clear();
  vTop_phi.clear();
  vTop_z.clear();
  vTop_e.clear();
  vTop_emc_e.clear();
  vTop_ihc_e.clear();
  vTop_ohc_e.clear();

  vINTT0r.clear();
  vINTT0phi.clear();
  vINTT0z.clear();
  
  vINTT1r.clear();
  vINTT1phi.clear();
  vINTT1z.clear();

  vMvtx0r.clear();
  vMvtx0phi.clear();
  vMvtx0z.clear();

  vMvtx1r.clear();
  vMvtx1phi.clear();
  vMvtx1z.clear();

  vMvtx2r.clear();
  vMvtx2phi.clear();
  vMvtx2z.clear();

  int nEmc = emc_r->size();
  int nTop = top_r->size();
  int nSiAll = a_layer->size();
  // store hits in the vectors with correction of (xBC, yBC)

  for(int iemc=0;iemc<nEmc;iemc++) {
    if(!Is_Hot_Emc(iemc)) {
      vEmc_r.push_back(emc_r->at(iemc));
      vEmc_phi.push_back(emc_phi->at(iemc));
      vEmc_z.push_back(emc_z->at(iemc));
      vEmc_e.push_back(energy->at(iemc));
    }//if(!Is_Hot_Emc)
  }//for(iemc)

  for(int itop=0; itop < nTop ;itop++) {
    vTop_r.push_back(top_r->at(itop));
    vTop_phi.push_back(top_phi->at(itop));
    vTop_z.push_back(top_z->at(itop));
    vTop_e.push_back(top_e->at(itop));
    vTop_emc_e.push_back(emc_e->at(itop));
    vTop_ihc_e.push_back(ihc_e->at(itop));
    vTop_ohc_e.push_back(ohc_e->at(itop));
  }//for(itop)

  for(int is = 0; is< nSiAll ;is++) {
    int layer = a_layer->at(is);
    float xhit = a_si_x->at(is)-xBC;
    float yhit = a_si_y->at(is)-yBC;
    float zhit = a_si_z->at(is);
    float phi_hit = atan2(yhit,xhit);
    float rhit = sqrt(xhit*xhit+yhit*yhit);

    if(layer == 0) {
      vMvtx0r.push_back(rhit);
      vMvtx0phi.push_back(phi_hit);
      vMvtx0z.push_back(zhit);
    } else if(layer == 1) {
      vMvtx1r.push_back(rhit);
      vMvtx1phi.push_back(phi_hit);
      vMvtx1z.push_back(zhit);
    } else if(layer == 2) {
      vMvtx2r.push_back(rhit);
      vMvtx2phi.push_back(phi_hit);
      vMvtx2z.push_back(zhit);
    } else if(layer == 3|| layer==4) {
      vINTT0r.push_back(rhit);
      vINTT0phi.push_back(phi_hit);
      vINTT0z.push_back(zhit);
    } else {
      vINTT1r.push_back(rhit);
      vINTT1phi.push_back(phi_hit);
      vINTT1z.push_back(zhit);
    }
  }//for(is)
}

bool Is_Hot_Emc(int iemc) {
  float phiEMC = emc_phi->at(iemc);
  float zEMC = emc_z->at(iemc);
  if( (2.4 < phiEMC && phiEMC < 2.7) && (zEMC < 0)) return true;
  else return false;
}

float rz2eta(float r,float z) {
  if(z>=0) {
    return log((sqrt(r*r+z*z)+z)/r);
  } else {
    return -log((sqrt(r*r+z*z)-z)/r);
  }
}

void FindInttPair(void) {
  const float dPhiMax = 0.1;
  int nINTT1=vINTT1phi.size();
  int nINTT0=vINTT0phi.size();

  for(int i1=0;i1<nINTT1;i1++){
    for(int i0=0;i0<nINTT0;i0++){
      float dphi = vINTT1phi.at(i1)-vINTT0phi.at(i0);
      if(abs(dphi)<dPhiMax) hINTTdphi->Fill(dphi);
    }//for(i0)
  }//for(i1)
}

void FindEmcIntt(void) {
}

void FindInttMvtx(void) {
}

void FindCaloInttMvtx(void) {
}
