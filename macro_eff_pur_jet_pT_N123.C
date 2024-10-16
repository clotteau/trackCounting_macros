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

/// \file macro_eff_pur_jet_pT_N123.C
/// \brief Macro to plot the efficiency of b tagging with signed Impact Parameter (2D) and purity of c-lf jets vs. jet pT. Uses the AO2D outputs of bjetTreeMerger.cxx
///
/// \author Clement Lotteau <clement.lotteau@cern.ch> IP2I

#include <math.h>

void macro_eff_pur_jet_pT_N123()
{
    std::unique_ptr<TFile> myFile( TFile::Open("myFile.root") );
    TTree *mytree = myFile->Get<TTree>("bjet-tree-merger/myTree");

    // creating the histograms for sIP and jet pT
    TH2 *hist_sIP_jetpt_lf = new TH2D("h1", "sIP_jetpt_lf", 400, -0.4, 0.4, 195, 5, 200);
    TH2 *hist_sIP_jetpt_c = new TH2D("h2", "sIP_jetpt_c", 400, -0.4, 0.4, 195, 5, 200);
    TH2 *hist_sIP_jetpt_b = new TH2D("h3", "sIP_jetpt_b", 400, -0.4, 0.4, 195, 5, 200);

    int flavor;
    float signedIP2D[10];
    float jetpt;
    mytree->SetBranchAddress("mJetFlavor", &flavor);
    mytree->SetBranchAddress("mSignedIP2D", &signedIP2D);
    mytree->SetBranchAddress("mJetpT", &jetpt);
    
    // Filling of the histograms
    // Change the number in 'signedIP2D[x]' to choose the Nth largest IP from 0 to 9 in descending order
    for (int i=0; i<mytree->GetEntries(); i++)
    {
        mytree->GetEntry(i);

        if (flavor==0 or flavor==3) // flavor: none or light flavor
        {
            hist_sIP_jetpt_lf->Fill(signedIP2D[0],jetpt);
        }
        if (flavor==1) // flavor: charm
        {
            hist_sIP_jetpt_c->Fill(signedIP2D[0],jetpt);
        }
        if (flavor==2) // flavor: beauty
        {
            hist_sIP_jetpt_b->Fill(signedIP2D[0],jetpt);
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
    double sIPmin = 0.008; // minimum value of sIP - Threshold of the tagger
    double sIPmax = 0.4; // maximum value of sIP

    // variables for jet pT
    int jetptMin = 5; // minimum value of jet pT
    int jetptMax = 200; // maximum value of jet pT

    // number of points for jet pT : 5 Gev/c to 200 GeV/c
    int nb_points = jetptMax - jetptMin;

    // tuples for list of jet pT, efficiency and purities
    double jetpt_tuple[nb_points];
    double efficiency_tuple[nb_points];
    double purity_tuple_c[nb_points];
    double purity_tuple_lf[nb_points];

    // get y axis (jet pT) of histograms
    TAxis *yAxis_b = hist_sIP_jetpt_b->GetYaxis();
    TAxis *yAxis_c = hist_sIP_jetpt_c->GetYaxis();
    TAxis *yAxis_lf = hist_sIP_jetpt_lf->GetYaxis();

    for (int i=0; i<nb_points+1; i++)
    {
        // to compute efficiency and purities between jetptMin and jetptMin+1
        int jetptBinMin_b = yAxis_b->FindBin(jetptMin);
        int jetptBinMax_b = yAxis_b->FindBin(jetptMin+1);
        TH1D *b_jets = hist_sIP_jetpt_b->ProjectionX("b_jets", jetptBinMin_b, jetptBinMax_b);
        int jetptBinMin_c = yAxis_c->FindBin(jetptMin);
        int jetptBinMax_c = yAxis_c->FindBin(jetptMin+1);
        TH1D *c_jets = hist_sIP_jetpt_c->ProjectionX("c_jets", jetptBinMin_c, jetptBinMax_c);
        int jetptBinMin_lf = yAxis_lf->FindBin(jetptMin);
        int jetptBinMax_lf = yAxis_lf->FindBin(jetptMin+1);
        TH1D *lf_jets = hist_sIP_jetpt_lf->ProjectionX("lf_jets", jetptBinMin_lf, jetptBinMax_lf);

        // add the jet pT to the tuple
        jetpt_tuple[i] = jetptMin;

        // computing efficiency and purities at threshold sIPmin and add to the tuples
        efficiency = b_jets->Integral(b_jets->FindFixBin(sIPmin), b_jets->FindFixBin(sIPmax)) / b_jets->GetEntries();
        purity_c = c_jets->Integral(c_jets->FindFixBin(sIPmin), c_jets->FindFixBin(sIPmax)) / c_jets->GetEntries();
        purity_lf = lf_jets->Integral(lf_jets->FindFixBin(sIPmin), lf_jets->FindFixBin(sIPmax)) / lf_jets->GetEntries();
        efficiency_tuple[i] = efficiency;
        purity_tuple_c[i] = purity_c;
        purity_tuple_lf[i] = purity_lf;

        // add increment to jet pT
        jetptMin++;
    }

    // creation of the graph: efficiency and purities vs. jet pT
    TGraph *efficiency_graph = new TGraph(nb_points, jetpt_tuple, efficiency_tuple);
    TGraph *purity_graph_c = new TGraph(nb_points, jetpt_tuple, purity_tuple_c);
    TGraph *purity_graph_lf = new TGraph(nb_points, jetpt_tuple, purity_tuple_lf);

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
    mg->SetTitle("Efficiency and purity of sIP b-jet tagger - 1st largest IP; jet pT (GeV/c); Efficiency and purities");
    gPad->SetLogy(); // on/off log scale on y axis
    mg->GetXaxis()->SetRangeUser(0,200);
}