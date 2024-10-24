// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file macro_eff_pur_sIPmin_N123.C
/// \brief Macro to plot the efficiency of b tagging with signed Impact Parameter (2D) and purity of c-lf jets vs. sIP threshold. Uses the AO2D outputs of bjetTreeMerger.cxx
///
/// \author Clement Lotteau <clement.lotteau@cern.ch> IP2I

#include <math.h>

void macro_eff_pur_sIPmin_N123()
{
    std::unique_ptr<TFile> myFile( TFile::Open("myFile.root") );
    TTree *mytree = myFile->Get<TTree>("bjet-tree-merger/myTree");
    
    // creating the histograms for sIP
    TH1 *hist_sIP_lf = new TH1F("h1", "sIP_lf", 400, -0.4, 0.4);
    TH1 *hist_sIP_c = new TH1F("h2", "sIP_c", 400, -0.4, 0.4);
    TH1 *hist_sIP_b = new TH1F("h3", "sIP_b", 400, -0.4, 0.4);

    int flavor;
    float signedIP2D[10];
    mytree->SetBranchAddress("mJetFlavor", &flavor);
    mytree->SetBranchAddress("mSignedIP2D", &signedIP2D);
    
    // Filling of the histograms
    // Change the number in 'signedIP2D[x]' to choose the Nth largest IP from 0 to 9 in descending order
    for (int i=0; i<mytree->GetEntries(); i++)
    {
        mytree->GetEntry(i);

        if (flavor==0 or flavor==3) // flavor: none or light flavor
        {
            hist_sIP_lf->Fill(signedIP2D[2]);
        }
        if (flavor==1) // flavor: charm
        {
            hist_sIP_c->Fill(signedIP2D[2]);
        }
        if (flavor==2) // flavor: beauty
        {
            hist_sIP_b->Fill(signedIP2D[2]);
        }
        
        if (i==10000000) // limit on number of jets analysed
        {
            break;
        }
        
        printf("%d\n", i);
    }

    // creation of variable for the computation of efficiency and purities
    double purity_c = 0.;
    double purity_lf = 0.;
    double efficiency = 0.;
    
    // setting limits on sIP (cm)
    double sIPmin = -0.4; // minimum value of sIP - Threshold of the tagger
    double sIPmax = 0.4; // maximum value of sIP

    // number of points from sIPmin to sIPmax
    int nb_points = 101;

    // incrementation of sIPmin
    double increment = (sIPmax - sIPmin) / nb_points;

    // tuples for list of sIPmin (threshold), efficiency and purities
    double sIPmin_tuple[nb_points];
    double efficiency_tuple[nb_points];
    double purity_tuple_c[nb_points];
    double purity_tuple_lf[nb_points];

    for (int i=0; i<nb_points+1; i++)
    {
        // add the sIPmin (threshold) value to the tuple
        sIPmin_tuple[i] = sIPmin;

        // computing efficiency and purities at threshold sIPmin and add to the tuples
        efficiency = hist_sIP_b->Integral(hist_sIP_b->FindFixBin(sIPmin), hist_sIP_b->FindFixBin(sIPmax)) / hist_sIP_b->GetEntries();
        purity_c = hist_sIP_c->Integral(hist_sIP_c->FindFixBin(sIPmin), hist_sIP_c->FindFixBin(sIPmax)) / hist_sIP_c->GetEntries();
        purity_lf = hist_sIP_lf->Integral(hist_sIP_lf->FindFixBin(sIPmin), hist_sIP_lf->FindFixBin(sIPmax)) / hist_sIP_lf->GetEntries();
        efficiency_tuple[i] = efficiency;
        purity_tuple_c[i] = purity_c;
        purity_tuple_lf[i] = purity_lf;
        
        // add increment to sIPmin (threshold)
        sIPmin = sIPmin + increment;
    }

    // creation of the graph: efficiency and purities vs. sIPs threshold
    TGraph *efficiency_graph = new TGraph(nb_points, sIPmin_tuple, efficiency_tuple);
    TGraph *purity_graph_c = new TGraph(nb_points, sIPmin_tuple, purity_tuple_c);
    TGraph *purity_graph_lf = new TGraph(nb_points, sIPmin_tuple, purity_tuple_lf);

    efficiency_graph->SetMarkerStyle(20);
    purity_graph_c->SetMarkerStyle(20);
    purity_graph_lf->SetMarkerStyle(20);
    efficiency_graph->SetMarkerSize(0.8);
    purity_graph_c->SetMarkerSize(0.8);
    purity_graph_lf->SetMarkerSize(0.8);
    efficiency_graph->SetMarkerColor(2);
    purity_graph_c->SetMarkerColor(3);
    purity_graph_lf->SetMarkerColor(4);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(efficiency_graph);
    mg->Add(purity_graph_c);
    mg->Add(purity_graph_lf);
    mg->Draw("apl");
    // add legend and title to multi plot
    TLegend *legend = new TLegend();
    legend->AddEntry(efficiency_graph,"Efficiency");
    legend->AddEntry(purity_graph_c,"Purity c");
    legend->AddEntry(purity_graph_lf,"Purity lf");
    legend->Draw();

    // don't forget to change the title if you change the Nth largest IP
    mg->SetTitle("Efficiency and purity of sIP b-jet tagger - 3rd largest IP; 2D signed impact parameter (sIP) threshold (cm); Efficiency and purities");
    gPad->SetLogy(); // on/off log scale on y axis
}