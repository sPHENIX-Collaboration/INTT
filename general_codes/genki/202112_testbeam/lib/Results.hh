#pragma once

class Results
{
public:
  int ladder_ = -1;
  int chip_ = -1;
  int total_ = -1;
  int pass_ = -1;
  int sigma_ = -1;
  
  Results( int ladder, int chip, int sigma, int total, int pass )
  {
    ladder_ = ladder;
    chip_ = chip;
    total_ = total;
    pass_ = pass;
    sigma_ = sigma;
  };

  double GetEfficiency(){ return 1.0 * pass_ / total_;};
  double GetUpperError(){ return TEfficiency::ClopperPearson( total_, pass_, 0.68, true ) - this->GetEfficiency();};
  double GetLowerError(){ return this->GetEfficiency() - TEfficiency::ClopperPearson( total_, pass_, 0.68, false );};
  void Print()
  {
    cerr << ladder_ << "\t" << chip_ << "\t" << sigma_ << "\t" << pass_ << "\t" << total_ << "\t"
	 << this->GetEfficiency() << "\t"
	 << this->GetUpperError() << "\t"
	 << this->GetLowerError()
	 << endl;
  }
};
