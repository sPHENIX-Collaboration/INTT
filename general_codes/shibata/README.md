# readtree

2021/12/22 Mika Shibata Nara Women's University

This macro is for the INTT beam test and MC analysis. This analysis uses the first INTT event per one CAMAC trigger signal. (There are multiple INTT events with different bcofull in a CAMAC trigger signal.)
You can analysis two things for three ladders.
- INTT hits per bcofull and bco (= one INTT event) (` t.next(); `)
- Efficiency calculation (` t.next_continue(); `)

You can check analysis of both data and MC.

## Installation

Please put readtree.C and readtree.h in the same directory.
root file name and three module numbers are needed.

## Running readtree

In a ROOT session, you can do
```
.L readtree.C
readtree t
```
To check the INTT hits per bcofull and bco
```
t.next(); 
```
This function show and fill t data members per bco and bcofull (The first INTT hit with a CAMAC trigger signal) Example output plots are below.

<img src="https://github.com/mikashibata/INTT/blob/main/general_codes/shibata/hitmap.png" width="500">

To check the efficiency calculation,
```
t.next_continue();
```
This function loop t.next() on all entries (The first INTT hit with a CAMAC trigger signal) Example output plots are below.

<img src="https://github.com/mikashibata/INTT/blob/main/general_codes/shibata/efficiency.png" width="500">

You will see the result of efficiency on the terminal. Example result is below.

```
m_ef_GOODhit[0]=613
m_ef_Allhit[0]=675
efficincy = 90.8148%
**********************************
**********************************
m_ef_GOODhit[1]=459
m_ef_Allhit[1]=477
efficincy = 96.2264%
**********************************
**********************************
m_ef_GOODhit[2]=610
m_ef_Allhit[2]=639
efficincy = 95.4617%
```

## Changing variables and situation in readtree.C
In readtree.C, you can change same variables and conditions.
INTT hit cutting window for efficiency calculation:
- noisy chips and channel
  
  If you want to cut noisy channel and chip, add `if( -condition you want to cut- )continue;` around l.777(readtree.C)
- chips for efficiency calculation
  
  If you want to calculate the efficiency of limited chips, you can select chips.
  
  `
  int CHOOSE_CHIP1 = 7;
  int CHOOSE_CHIP2 = 8;
  int CHOOSE_CHIP3 = 9;
  `
  
  Also you choose the next bool.
  
  `
  SELECT_CHIP = SELECT_CHIP1==true||SELECT_CHIP2==true||SELECT_CHIP3==true;
  `
  
  If you want to calculate all chips (one ladder) efficiency, you choose the next bool.
  
  `
  SELECT_CHIP = chip_for2dhist[0]==chip_for2dhist[1];
  `
- adc
  
  `SELECT_CHIP_ADC = clustered_adc[0]>2&&clustered_adc[1]>2;`
- offset 
  
  You can change the offset value form the mean of fitting function (gaus) for regidual distribution. 
  
  `residual_chan_13[3] = {mean value for ladder 0, mean value for ladder 1, mean value for ladder 2};`
- chip
  
  `if(expect_ChipAxis[1]<=ef_ChipAxis_new+1.&&expect_ChipAxis[1]>=ef_ChipAxis_new-1.)`
- channel (from the sigma of fit gaus function of residual distribution)
  
  You can change the offset value form the sigma of fitting function (gaus) for regidual distribution.
  
  `if(m_boolMC)  cutwindow = "sigma value"*3.`
- number of clusters in the ladder to calculate the detection efficiency 
  
  If you want to allow multiple clusters, comment out this line. 
  
  `if(COSMIC[0][0]==1&&COSMIC[1][0]==1&&COSMIC[2][0]==1)`
