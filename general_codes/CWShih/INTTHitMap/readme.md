# The module for making the INTT half-ladder by half-ladder HitMap
* You will need to compile the module
  * you may need to do `make clean` after you done the `autogen.sh --prefix=$MYINSTALL`
* The module reads the `INTTRAWHIT`
* The module applies the bad channels based on the map directory you offer.
* If you have the tag of `ApplyBcoDiff_in = true`, it basically assumes that the `bco_diff` peaks of all the half-ladders are the same, which is the case for most of the run 2024 data. But may not be applicable to the data taken in run 2023.
* If you have the tag of `ApplyBcoDiff_in = true`, it will only accept the hits with the `bco_diff` within `plus and minus 1 bco of bco_diff_peak_in`.
* You can remove the clone hits by having the `clone_hit_remove_BCO_tag_in = true`.
  * Here the definition of clone hits is, you have at least two hits with the same `FELIX ID`, `FELIX_channel_ID`, `chip_id`, `channel_id`, `hit_bco`.

* You can have the hit quality assurance by having the `ApplyHitQA_in = true`.
  * What it does is, it checks the `FELIX ID`, `FELIX_channel_ID`, `chip_id`, `channel_id`, `hit_bco` and `adc`. They should have to be within the regions we expect.
  * Such as the `hit_bco` should be `0 ~ 127`.
  * Such as the `hit adc` should be `0 ~ 7`  

* The inputs
``` C++

INTTHitMap(
    const std::string &name = "INTTHitMap",
    const int process_id_in = 0,
    const int runnumber_in = 0,
    const std::string output_directory_in = "output.root",
    const bool isBadChannelMask_in = true,
    const std::string hotchannel_full_directory_in = "",
    const bool ApplyBcoDiff_in = true,
    const int bco_diff_peak_in = 0,
    const bool ApplyHitQA_in = true,
    const bool clone_hit_remove_BCO_tag_in = true
  );

```
