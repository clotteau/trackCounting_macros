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

/// \file macro_sIP_N123.C
/// \brief Macro to plot the probability distrib. of signed Impact Parameter (2D) for b,c,lf jets from N largest (1st,2nd...) IP. Uses the AO2D outputs of bjetTreeMerger.cxx
///
/// \author Clement Lotteau <clement.lotteau@cern.ch> IP2I

#include <math.h>

void macro_sIP_N123()
{
    std::unique_ptr<TFile> myFile( TFile::Open("myFile.root") );
    TTree *mytree = myFile->Get<TTree>("bjet-tree-merger/myTree");

    
    // creating the histogram for sIP
    TH1 *hist_sIP_lf = new TH1F("h1", "sIP_lf", 400, -0.4, 0.4);
    TH1 *hist_sIP_c = new TH1F("h2", "sIP_c", 400, -0.4, 0.4);
    TH1 *hist_sIP_b = new TH1F("h3", "sIP_b", 400, -0.4, 0.4);

    int flavor;
    float signedIP2D[10];
    mytree->SetBranchAddress("mJetFlavor", &flavor);
    mytree->SetBranchAddress("mSignedIP2D", &signedIP2D);
    

    // Filling of the histograms
    // Change the number in 'signedIP2D[x]' to choose the Nth largest sIP from 0 to 9 in descending order
    for (int i=0; i<mytree->GetEntries(); i++)
    {
        mytree->GetEntry(i);

        if (flavor==0 or flavor==3) // flavor: none or light flavor
        {
            hist_sIP_lf->Fill(signedIP2D[0]);
        }
        if (flavor==1) // flavor: charm
        {
            hist_sIP_c->Fill(signedIP2D[0]);
        }
        if (flavor==2) // flavor: beauty
        {
            hist_sIP_b->Fill(signedIP2D[0]);
        }
        /*
        if (i==5000000) // limit on number of jets analysed
        {
            break;
        }
        */
        printf("%d\n", i);
    }

    // normalization of histograms
    Double_t factor = 1.;
    hist_sIP_b->Scale(factor/hist_sIP_b->GetEntries());
    hist_sIP_c->Scale(factor/hist_sIP_c->GetEntries());
    hist_sIP_lf->Scale(factor/hist_sIP_lf->GetEntries());

    // creation of the graphs
    TGraph *graph_b = new TGraph(hist_sIP_b);
    TGraph *graph_c = new TGraph(hist_sIP_c);
    TGraph *graph_lf = new TGraph(hist_sIP_lf);
    graph_b->SetMarkerStyle(20);
    graph_c->SetMarkerStyle(20);
    graph_lf->SetMarkerStyle(20);
    graph_b->SetMarkerSize(0.8);
    graph_c->SetMarkerSize(0.8);
    graph_lf->SetMarkerSize(0.8);
    graph_b->SetMarkerColor(2);
    graph_c->SetMarkerColor(3);
    graph_lf->SetMarkerColor(4);

    // creation of multi graph to plot the 3 sIP distributions
    TMultiGraph *mg = new TMultiGraph();
    mg->Add(graph_lf);
    mg->Add(graph_c);
    mg->Add(graph_b);
    mg->Draw("apl");
    // add legend and title to multi plot
    TLegend *legend = new TLegend();
    legend->AddEntry(graph_b,"b");
    legend->AddEntry(graph_c,"c");
    legend->AddEntry(graph_lf,"lf");
    legend->Draw();
    // don't forget to change the title if you change the Nth largest sIP
    mg->SetTitle("sIP distribution for b-jet tagger - 1st largest IP; 2D signed impact parameter (sIP) (cm); Normalized counts");
    mg->GetHistogram()->SetMinimum(1); // log scale does not work without this line
    gPad->SetLogy(); // on/off log scale on y axis
    mg->GetYaxis()->SetRangeUser(pow(10,-7),1);
    gPad->Modified();
}