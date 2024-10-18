# Root macros for b-jet tagging using track counting method

These macros use the ```.root``` output of the ```O2Physics``` task ```bjetTreeMerger```. This task merges the TTrees created with the task ```bjetTreeCreator``` and, unlike ```bjetTreeCreator```, ```bjetTreeMerger``` cannot be used on the Hyperloop - only locally.

```N123``` is written in the names of the macros because they were created to compare different distribution (efficiency, purity, sIP) for the 3 tracks with the highest Impact Parameter.

## First macro: ```macro_sIP_N123.C```

This macro plots the probability distributions of the 2D signed Impact Parameter (IP) for b,c and lf jets for the tracks with the highest IP (1st, 2nd and 3rd). To choose if you want to plot the distribution for the 1st largest IP, 2nd or 3rd, simply change the value of ```x``` in ```signedIP2D[x]```. In this tupple, ```x``` goes from 0 to 9, 0 being the largest IP, 1 being the 2nd largest IP...etc


## Second macro: ```macro_eff_pur_sIPmin_N123.C```

This macro computes the efficiency and the purity of the b-tagging for different values of the sIP threshold (from -0.4 to +0.4 cm).

The efficiency of the tagger is the number of b-jets tagged as b over the total number of b-jets. The purity of the tagger is the number of c/lf-jets tagged as b over the total number of c/lf-jets

Once the purity and the efficiency have been computed for different values of the sIP threshold, this macro plots the efficiency and purity distributions vs. the sIP threshold.

This analysis should be done multiple times by modifying ```signedIP2D[x]``` like in the macro ```macro_sIP_N123.C```

## Third macro: ```macro_eff_pur_jet_pT_N123.C```

This macro computes the efficiency and the purity of the b-tagging for different intervals of jet pT. The sIP threshold is chosen manually (0.008cm in my analysis) and stays the same. Instead of dividing by the total number of b/c/lf-jets to compute the purity and the efficiency, we divide by the number of b/c/lf-jets in the jet pT interval we chose. The default interval is between pT and pT+1.

Once the purity and efficiency have been calculated for all jet pT intervals (from 5 GeV/c to 200 GeV/c for example), the macro plots these distributions vs. the jet pT.

This analysis should be done multiple times by modifying ```signedIP2D[x]``` like in the macro ```macro_sIP_N123.C```

## Fourth macro: ```macro_eff_pur_jet_pT_min_N123.C```

This macro is very similar to ```macro_eff_pur_jet_pT_N123.C```. Instead of choosing an interval of 1 GeV/c for the jet pT, we chose a maximum value that will stay the same like 200 GeV/c for example. Only the minimum value of the interval will change, from 5 to 199 GeV/c for example.

This analysis should be done multiple times by modifying ```signedIP2D[x]``` like in the macro ```macro_sIP_N123.C```