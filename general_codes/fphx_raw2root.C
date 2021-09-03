#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <TBenchmark.h>

int getFileSize(std::ifstream& s);

//
// Input is the file name to process
//
// Second arg is the name of the output file to produce.  If null or zero length, 
// the code creates a filename based on the input, but with ".dat" changed to ".root"
//
// Hit formats:
//
//  0 = first FPHX1 format from first Xilinx board (repacked in the fpga output)
//  1 = FPHX1 format but not repacked
//  2 = 8/13/2009 Test beam ROC format (FPHX1)
//  3 = 8/13/2009 Test beam ROC format (FPHX1) and second file format (updated daq code midway thru day)
//

void
fphx_raw2root(
	      const char* fname="fphx_raw.dat", // input file name
	      const char* oname=0,  // output file name
	      int fphxrev=0, // revision of the fphx data hit format
	      unsigned int maxbuffers=-1 // numbuffers to process, -1 means all
	      )
{
  std::ifstream f(fname,std::ifstream::binary);

  TString ostr;
  if ( ! oname || oname[0] == '\0' )
    {
      ostr = fname;
      ostr.ReplaceAll(".dat",".root");
    }
  else
    {
      ostr = oname;
    }

  if ( ! f.is_open() )
    {
      std::cout << "Failed to open input file " << fname << std::endl;
      return;
    }

  // get size of file
  std::cout << "Input file " << fname << std::endl;
  unsigned int size = getFileSize(f);
  std::cout << "Number of bytes in file = " << size << std::endl;

  TTree* t = (TTree*)gROOT->FindObject("T");
  if ( t )
    {
      std::cout << "Found TTree " << t->GetName() << ", deleting" << std::endl;
      delete t;
    } 

  TTree* t1 = (TTree*)gROOT->FindObject("T1");
  if ( t1 )
    {
      std::cout << "Found TTree " << t1->GetName() << ", deleting" << std::endl;
      delete t1;
    } 

  std::cout << "Output file = " << ostr << std::endl;
  TFile of(ostr.Data(),"RECREATE");
  of.SetCompressionLevel(1);
  of.cd();

  // T is the tree containing the hits for the run
  //
  unsigned int word = 0;
  unsigned int bufcnt = 0;
  unsigned int datacnt = 0;
  unsigned short chan = 0;
  unsigned short amp = 0;
  unsigned short adc = 0;
  unsigned short bco = 0;
  unsigned short roc_chip_id = 0;
  unsigned short mod_chip_id = 0;
  unsigned short chip_id = 0;
  unsigned short side = 0;
  unsigned short fpga_id = 0;
  unsigned short module = 0;
  unsigned int hitcnt = 0;
  unsigned short last_word = 0;
  unsigned short fifo_full = 0;
  std::cout << "Create T tree" << std::endl;
  t = new TTree("T","FPHX Hits");
  t->Branch("buf",&datacnt,"buf/i");
  t->Branch("hit",&hitcnt,"hit/i");
  t->Branch("word",&word,"word/i"); // uncomment for debugging
  t->Branch("chan",&chan,"chan/s");
  t->Branch("amp",&amp,"amp/s");
  t->Branch("adc",&adc,"adc/s");
  t->Branch("bco",&bco,"bco/s");
  t->Branch("fpga",&fpga_id,"fpga/s");
  //t->Branch("roc_chip",&roc_chip_id,"roc_chip/s");
  t->Branch("module",&module,"module/s");
  t->Branch("side",&side,"side/s");
  t->Branch("chip",&chip_id,"chip/s");
  //t->SetAutoSave(64000);

  // T1 is a tree containing the chip configuration for the run
  //
  unsigned int runnumber = 0;
  unsigned short config_module = 0;
  unsigned short config_side = 0;
  unsigned short config_chip = 0;
  unsigned short enable_masks[8] = { 0 };
  char reg[16] = { '\0' };
  std::cout << "Create T1 tree" << std::endl;
  t1 = new TTree("T1","FPHX Configuration");
  t1->Branch("runnumber",&runnumber,"runnumber/i");
  t1->Branch("module",&config_module,"module/s");
  t1->Branch("side",&config_side,"side/s");
  t1->Branch("chip",&config_chip,"chip/s");
  t1->Branch("enable_masks",&enable_masks[0],"enable_masks[8]/s");
  t1->Branch("reg",&reg[0],"reg[16]/b");
  //t1->SetAutoSave(64000);

  // Format of record (fphxrev == 3) is a series of n 32-bit words:
  //
  //   record[0] = buflen = n-1, # of record's words not including record[0]
  //   record[1] = buffer id
  //   record[2..n-2] = data words
  //   record[n-1] = checksum for buffer, including record[0]
  //
  // Note that this is valid for data taken in the 8/13 testbeam.  Other data may need
  // modification.

  std::cout << "Start looping over input records" << std::endl;
  while ( 1 && bufcnt <= maxbuffers )
    {
      // Read the buffer length from the first word
      //
      int buflen = 0;
      f.read((char*)&buflen,sizeof(buflen));
      if ( f.fail() )
        {
          std::cout << "eof, fail, or bad condition during read" << std::endl; 
	  std::ifstream::iostate state = f.rdstate();
	  if ( state & std::ios::eofbit )
	    std::cout << " eof bit set" << std::endl;
          break;
        }

      // Read the data.  CINT interpreter can't grok the vector for some reason.
#ifdef __CINT__
      unsigned int* data = new unsigned int[buflen];
#else
      std::vector<unsigned int> data(buflen,0);
#endif
      f.read((char*)&data[0],buflen*sizeof(int));
      if ( f.fail() )
        {
          std::cout << "eof, fail, or bad condition during read" << std::endl; 
	  std::ifstream::iostate state = f.rdstate();
	  if ( state & std::ios::eofbit )
	    std::cout << "End of file.  All your bits are belong to us." << std::endl;
          break;
        }
      
      int cnt = buflen;
      int bufid = data[0];
      unsigned int cksum = data[buflen-1];
      unsigned int checksum = 0;
      checksum ^= buflen; // Accidentally included the buflen word in checksum in read_DAQ
      for(int i=0; i<buflen-1; i++) checksum ^= data[i];
      const char* ckstate = "OK";
      if ( checksum != cksum ) ckstate = "Bad";
      std::cout << "Buffer " << bufcnt << ": cnt = " << cnt 
		<< " id = " << bufid 
		<< " cksum = 0x" << std::hex << std::setw(8) << std::setfill('0') << cksum << std::dec 
		<< " checksum = 0x" << std::hex << std::setw(8) << std::setfill('0') << checksum << std::dec << " "
		<< ckstate << std::endl;

      bufcnt++;

      if ( checksum != cksum )
 	{
 	  std::cout << "WARNING: bad checksum: cksum = 0x"
 		    << std::hex << std::setw(8) << std::setfill('0') << cksum << ", calc cksum = 0x"
 		    << std::hex << std::setw(8) << std::setfill('0') << checksum << std::dec << std::endl;
 	}
      
      // Unpack the record according to the id.
      if ( bufid == 101 )
	{
	  // This record is a time stamp
	  // Format of timestamp record is
	  //   data[0] = id (101)
	  //   data[1] = time in cpu clock ticks 
	  //   data[2] = CLOCKS_PER_SEC taken from the system value
	  //
	  double time_in_run = double(data[1]) / double(data[2]);
	  std::cout << "    Timestamp: time in run = " << time_in_run << " sec" << std::endl;
	}
      else if ( bufid == 100 )
 	{
 	  // This buffer is a configuration record
	  // Format of the data is as follows:
	  //   data[0] = id ( 100 )
	  //   data[1] = run number
	  //   data[2..7] = 10 short words:
	  //      0 = packed chip id (includes wedge id)
	  //      1..9 = enable masks for channels turned on
	  //   
	  int idx = 1;
 	  std::cout << "    Configuration Record" << std::endl;
 	  runnumber = data[idx++];
 	  std::cout << "    Run Number " << runnumber << std::endl;
	  while ( idx < buflen - 2 )
	    {
	      unsigned short* p = (unsigned short*)&data[idx++];
	      int n = 0;
	      for ( n=0; n<9; )
		{
		  unsigned short packed_chipid = p[n++];
		  //unsigned short masks[8];
		  for(int m=0; m<8; m++, n++) enable_masks[m] = p[n];
		  unsigned short wedgeaddr = packed_chipid >> 5;
		  config_chip = 0x1F & packed_chipid;
		  config_module = 0xF & wedgeaddr;
		  config_side   = (wedgeaddr>>4)&0xF;
		  std::cout << "    Chip " << packed_chipid 
			    << " (Module " << config_module << " Side " << config_side 
			    << " Chip " << config_chip << ")" << std::endl;
		  std::cout << "      enable masks ";
		  for(int m=0; m<8; m++) std::cout << "0x" << std::hex << enable_masks[m] << " ";
		  std::cout << std::dec << std::endl;
		}
	      idx += 9;
	      unsigned char* p2 = (unsigned char*)&p[n];
	      for(n=0; n<16; n++)
		{
		  reg[n] = p2[n];
		  std::cout << "      Reg " << n+2 << ": " << (int) reg[n] << std::endl;
		}
	      idx += 16;
	    }
	  t1->Fill();
	}
      else if ( bufid == 102 )
 	{
	  datacnt++;
 	  // This buffer is a data record
	  // Format of the data is as follows:
	  //   data[0] = id ( 102 )
	  //   data[1] = hit 0
	  //   data[2] = hit 1
	  //     ...
	  //
 	  std::cout << "    Data record " << std::endl;
	  unsigned int cnt = buflen - 2; // hit count is the buffer length - (id + checksum)
	  unsigned int* ptr = &data[1];
	  hitcnt = 0;
 	  for ( unsigned int i=0; i<cnt; i++)
 	    {
 	      if ( ((i+1)*sizeof(unsigned int)) > size )
		{
		  // Safety belt against a partially-written buffer
		  // (it will have the full length field, but the
		  // whole buffer hasn't been read in).  This can
		  // happen, for instance, if we are reading a file
		  // that is actively being written.
		  std::cout << "Partial buffer detected, bailing" << std::endl;
		  break;
 		}
	      if ( fphxrev == 0 )
		{
		  // This is copied straight out of ReadDigChan-IntClk-DigRef.c, 
		  // but I do not know why this is necessary.  Should be redundant, though.
		  if ( (ptr[i]&0x00000001)==0 ) continue;
		  bco  = ( ptr[i] & 0x3F000000 ) >> 24;
		  adc  = ( ptr[i] & 0x00380000 ) >> 19;
		  chan = ( ptr[i] & 0x0003F800 ) >> 11;
		  amp  = ( ptr[i] & 0x000007FE ) >> 1;
		  side = 0;
		  fpga_id = 0;
		  chip_id = 0;
		}
	      else if ( fphxrev == 3 )
		{
		  // This revision is for the ROC output used in the 8/13 testbeam.  The data was packed
		  // in a way that includes what wedges the hit comes from.
		  //if ( (ptr[i]&0x00000001)==0 ) continue;

		  // There is a bug in the version of read_DAQ that was used for the testbeam.  The wordcount
		  // was incremented for each new data word, and then used to index into the output array when
		  // storing the hit.  The order should have been the opposite (was it a fortran programmer that 
		  // wrote this?!?).  As a result, the first word in the buffer is bogus, and the last hit (for
		  // the buffer)was lost.
		  if ( fphxrev == 3 && i == 0 ) continue; // allow for a new version that doesn't have this bug
		  word        = ptr[i];
		  adc         = ( ptr[i] & 0xe0000000 ) >> 29;
		  bco         = ( ptr[i] & 0x1f800000 ) >> 23;
		  amp         = ( ptr[i] & 0x007e0000 ) >> 17;
		  chan        = ( ptr[i] & 0x0000fe00 ) >> 9;
		  fpga_id     = ( ptr[i] & 0x00000080 ) >> 7;
		  roc_chip_id = ( ptr[i] & 0x0000007c ) >> 2;

		  // Unpack the chip id.  The top 4 chips on each side were read out, and a single FPGA read two modules.
		  // The FPGA numbered the chips from 1 to 16, which means one module was chips 1-8 and the other was 9-16.
		  // However, the physical chip ids are 1-4 on the right side and 14-18 on the other. So
		  // this chip is actually not the physical id, but a logical one that starts with 1 from 
		  // the top, regardless of which side.  This will matter if you want to match up the 
		  // configuration to the chips, since the configuration is wrt the physical chip id.
		  mod_chip_id = (roc_chip_id < 9) ? roc_chip_id : roc_chip_id - 8;

		  // Determine which module this is based on what the ROC's logical chip id is for 
		  // the particular FPGA.  Note the order of FPGA 1's modules.  This was because of
		  // how the cabling was done when the device was set up in the testbeam.  The phyicsl
		  // order of the modules was PCB, PCB, PCB, Kapton (up to downstream). (or possibly the opposite)
		  if (fpga_id == 0) module = (roc_chip_id < 9) ? 0 : 1;
		  else		    module = (roc_chip_id < 9) ? 3 : 2;

		  side = mod_chip_id < 5 ? 0 : 1;
		  chip_id = mod_chip_id < 5 ? mod_chip_id : mod_chip_id - 4;
		}
	      else if ( fphxrev == 4 )
		{
		  if ( i == 0 ) continue; // A bug in read_DAQ -- see rev 3 comments
		  // V1 of the chip, read out by the Spartan3 with no rearranging of bits on output.
		  //
		  word        = ptr[i];
		  amp         = ( word & 0x000007fe ) >> 1;
		  bco         = ( word & 0x0001f800 ) >> 11;
		  chan        = ( word & 0x00fe0000 ) >> 17;
		  adc         = ( word & 0x0e000000 ) >> 25;
		  last_word   = ( word & 0x10000000 ) >> 28;
		  fifo_full   = ( word & 0x20000000 ) >> 29;
		  side        = 0;
		  fpga_id     = 0;
		  roc_chip_id = 0;
		  chip_id     = 0;		  
		}
	      else if ( fphxrev == 5 )
		{
		  if ( i == 0 ) continue; // A bug in read_DAQ -- see rev 3 comments
		  // V2 of the chip, read out by the Spartan3 with no rearranging of bits on output.
		  //
		  word        = ptr[i];
		  amp         = ( word & 0x000007fe ) >> 1;  // 10 bits of amplitude
		  bco         = ( word & 0x0003f800 ) >> 11; // 7 bits of BCO
		  chan        = ( word & 0x01fc0000 ) >> 18; // 7 bits of channel number
		  adc         = ( word & 0x1c000000 ) >> 26; // 3 bits of ADC (skip over the word mark)
		  fifo_full   = ( word & 0x20000000 ) >> 29;
		  side        = 0;
		  fpga_id     = 0;
		  roc_chip_id = 0;
		  chip_id     = 0;
		}
	      else if ( (fphxrev == 6) || (fphxrev==7) )
		{
		  // V2 of the chip, read out by the ROC with no rearranging of bits on output.
		  // Initial work is on the single-chip board.
		  // ROCv1 output (FPHX-2) format
		  // ----------------------------
		  // DDxB BBBB BAAA AAA1 CCCC CCBC FIII IID0
		  // adc = DDD, bco = BBBBBB, ampl = AAAAAA, channel = CCCC CCCC, 
		  // chip id = IIIII
		  //
		  // Version 7 == Version 6, except read out via the small interface HDI (Rev1),
		  // which means all chips are read out by FPGA0, except two lines (chips 6 & 7,
		  // which are chips 1 and 2 from side 1).  We compensate for this split readout.
		  //
		  word     = ptr[i];
		  adc      = ((word>>30) & 0x03) | ((word<<1) & 0x04);
		  bco      = ((word>>23) & 0x3F) | ((word>>3) & 0x40);
		  amp      =  (word>>17) & 0x3F;
		  chan     = ((word>>10) & 0x3F) | ((word>>2) & 0x40);
		  fpga_id  =  (word>>7)  & 0x01;
		  chip_id  =  (word>>2)  & 0x1F;
		  //
		  fifo_full   = 0;
		  side        = 0;
		  //fpga_id     = 0;
		  roc_chip_id = 0;
		  //chip_id     = 0;

		  if ( (fphxrev == 7) && (chip_id>5) )
		    {
		      // Remap the chip and fpga ids
		      chip_id -= 5;
		      fpga_id = 1;
		    }
		}
	      else if ( fphxrev == 8 )
		{
		  // V1 of the chip, read out by the ROC with no rearranging of bits on output.
		  // Initial work is on the 13-chip kapton HDI from UNM.
		  // ROCv1 output (FPHX-1) format
		  // ----------------------------
		  //
		  word        = ptr[i];
		  adc         = ( ptr[i] & 0xe0000000 ) >> 29;
		  bco         = ( ptr[i] & 0x1f800000 ) >> 23;
		  amp         = ( ptr[i] & 0x007e0000 ) >> 17;
		  chan        = ( ptr[i] & 0x0000fe00 ) >> 9;
		  fpga_id     = ( ptr[i] & 0x00000080 ) >> 7;
		  chip_id     = ( ptr[i] & 0x0000007c ) >> 2;
		  side = 0; // TODO
		  fifo_full = 0; // TODO
		  roc_chip_id = 0;
		}
	      else 
		{
		  std::cout << "Unknown hit format" << std::endl;
		  break;
		}
 	      hitcnt++;
	      t->Fill();
 	    }
	  std::cout << "    Read " << hitcnt << " hits" << std::endl;
 	}
      else 
	{
	  std::cout << "Unknown buffer id" << std::endl;
	}
#ifdef __CINT__ // No need to explicitly clean up the vector
      delete [] data;
#endif
    }

  t->AutoSave();
  t1->AutoSave();

  of.Write("",TObject::kWriteDelete);

  gDirectory->Purge();

  return;
}

int
getFileSize(std::ifstream& f)
{
  f.seekg(0,std::ifstream::end);
  if ( f.fail() )
    {
      std::ifstream::iostate state = f.rdstate();
      std::cout << "error seeking to end, read state = " << state << std::endl;
      if ( state & std::ios::eofbit )
	std::cout << " eof bit set" << std::endl;
      if ( state & std::ios::failbit )
	std::cout << " fail bit set" << std::endl;
      if ( state & std::ios::badbit )
	std::cout << " bad bit set" << std::endl;
      return -1;
    }
  int size = f.tellg();
  if ( size < 0 )
    {
      std::ifstream::iostate state = f.rdstate();
      std::cout << "error in tellg, read state = " << state << std::endl;
      if ( state & std::ios::eofbit )
	std::cout << " eof bit set" << std::endl;
      if ( state & std::ios::failbit )
	std::cout << " fail bit set" << std::endl;
      if ( state & std::ios::badbit )
	std::cout << " bad bit set" << std::endl;
      return size;
    }

  // Return the file stream to the start of the file
  f.seekg(0,std::ifstream::beg);

  return size;
}
