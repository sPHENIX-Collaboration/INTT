#ifndef GETBINWITHOUTOVERUNDERFLOW_20250609
#define GETBINWITHOUTOVERUNDERFLOW_20250609

int GetPreviousWithoutUnderflow(int bin){
  if (bin == 1) {
    return 128;
  } else {
    return bin-1;
  }
};

// same situation as get_previous_without_underflow(int bin)
int GetNextWithoutOverflow(int bin) {
  if (bin == 128) {
    return 1;
  } else {
    return bin+1;
  }
};

#endif /* GETBINWITHOUTOVERUNDERFLOW_20250609 */