// IMBS literals

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1.h>
#include <vector>
#include <numeric>
#include <iostream>

using namespace std;

#define IBMS_NUM_CHANNELS 80
#define IBMS_TRACE_LENGTH 1024
#define IBMS_NUM_TRIGGERS 12
#define IBMS_LVDS_BITS 16

struct ibmsRaw{

    ULong64_t sys_clock[IBMS_NUM_CHANNELS];
    ULong64_t dev_clock[IBMS_NUM_CHANNELS];
    UShort_t trace[IBMS_NUM_CHANNELS][IBMS_TRACE_LENGTH];
    UShort_t trigger[IBMS_NUM_TRIGGERS][IBMS_TRACE_LENGTH];
    UShort_t lvds_bits[3];

};

void plot_ibms_data(int eventNum){

    gStyle->SetLabelSize(0.05,"xy");
    gStyle->SetTitleSize(0.05,"xy");
    gStyle->SetTitleH(0.1);

    // Access to a TTree
    TFile *file = new TFile("ibms_run_00089.root");
    TTree *tree = (TTree*)file->Get("t_ibms");

    // Prefill a vector of time (0..1023)
    std::vector<int> time(1024);
    std::iota (std::begin(time), std::end(time), 0); 

    // Instantiate a vector to store waveform buffer
    std::vector<std::vector<int>> trace(80,std::vector<int>(1024,0));

    // Declare an ibms struct holder
    ibmsRaw ibms;
    tree->SetBranchAddress("ibms",&ibms);

    // Output number of events
    std::cout<<"Number of events taken = "<<tree->GetEntries()<<endl;
    int nEvent = tree->GetEntries();




    if(eventNum>-1){

        // Create a canvas and split the canvas into a 4x4 grid (each event)
        TCanvas *c1 = new TCanvas("c1","c1",1400,800);
        c1->Divide(4,4);

        TH1D *h[16];

        tree->GetEntry(eventNum);

        for(int i=0; i<16; i++){
        
            c1->cd(i+1);

            h[i] = new TH1D(Form("Event%i, CH%i",eventNum, i+1),Form("Event%i, CH%i",eventNum, i+1),1024,0,1024);

            for(int j=0; j<1024; j++){
                h[i]->SetBinContent(j+1,ibms.trace[i][j]);
                trace[i][j] += trace[i][j];
            }

            h[i]->GetXaxis()->SetTitle("time [ns]");
            h[i]->Draw();
        }

    }

    if(eventNum<0){

        // Create a canvas and split the canvas into a 4x4 grid (average)
        TCanvas *c2 = new TCanvas("c2","c2",1400,800);
        c2->Divide(4,4);

        for(int iEvent=0;iEvent<nEvent;iEvent++){
            tree->GetEntry(iEvent);

            for(int i=0; i<16; i++){
                for(int j=0; j<1024; j++){
                    trace[i][j] += ibms.trace[i][j];
                }
            }

        }

        // Creating avg 
        TH1D *h_avg[16];
        for(int i=0; i<16; i++){

            c2->cd(i+1);
            h_avg[i] = new TH1D(Form("CH%i",i+1),Form("CH%i",i+1),1024,0,1024);

            for(int j=0; j<1024; j++){
                h_avg[i]->SetBinContent(j+1,trace[i][j]/nEvent);
            }

            h_avg[i]->GetXaxis()->SetTitle("time [ns]");
            h_avg[i]->Draw();

        }

    }
}
