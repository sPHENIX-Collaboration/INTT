# InttRawHitQA
This is an analysis module for QA using INTTRAWHIT.

## How to use

### Condition
#### Input
DST(s) file containing INTTRAWHIT and INTTEVENTHEADER (may not be mandatory).

#### Output
By default, outputs are saved in the QA directory of INTT commissioning (/sphenix/tg/tg01/commissioning/INTT/QA/raw_hit).
If you want to change it, for example, the current directory as a test, use  void SetOutputDir( std::string dir ).
Output files are
- ROOT file (directory: {root_dir}/root/{year}/): ROOT objects are saved.
- PDF file (directory: {root_dir}/plots/{year}/): Plots are saved.
- txt file (directory: {root_dir}/txt/{year}/): Some information is stored for easy access.
