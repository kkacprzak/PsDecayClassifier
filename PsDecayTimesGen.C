#include "include/cpp-json/json.h"
#include "SmallEvent.h"
#include "BigEvent.h"

Double_t SOURCE_INTENSITY = 0.0; //[Bq]
Double_t SIMULATION_TIME = 0.0; //[s]
Double_t PPS_LIFETIME = 0.0; //[ps]
Double_t OPS_LIFETIME = 0.0; //[ps]
Double_t OPS2PPS_RATIO = 0.0; // number
Double_t PROB_DEEX = 0.0; // not used now
Double_t PROB_PPS = 0.0; // not used now
Double_t PROB_OPS = 0.0; // not used now
Bool_t GEN_BACKGROUND = false; // not used now
Bool_t GEN_WEIGHTS = false; // not used now
std::string PARTIAL_OUTPUT_FILE_NAME = "task.root";
std::string FULL_OUTPUT_FILE_NAME = "solution.root";

void readParamFile(std::string filename){
  std::ifstream file(filename);
  json::value jsonObject = json::parse(file);
  SOURCE_INTENSITY = json::to_number(jsonObject["PositronSourceIntensity"]);
  SIMULATION_TIME = json::to_number(jsonObject["SimulationTime"]);
  PPS_LIFETIME = json::to_number(jsonObject["pPsLifetime"]);
  OPS_LIFETIME = json::to_number(jsonObject["oPsLifetime"]);
  OPS2PPS_RATIO = json::to_number(jsonObject["oPs2pPsRatio"]);
  PROB_DEEX = json::to_number(jsonObject["ProbDeexPhoton"]);
  PROB_PPS = json::to_number(jsonObject["Prob_pPs"]);
  PROB_OPS = json::to_number(jsonObject["Prob_oPs"]);
  GEN_BACKGROUND = json::to_bool(jsonObject["GenerateBackground"]);
  GEN_WEIGHTS = json::to_bool(jsonObject["AssignRecoWeights"]);
  PARTIAL_OUTPUT_FILE_NAME = json::to_string(jsonObject["PartialOutputFileName"]);
  FULL_OUTPUT_FILE_NAME = json::to_string(jsonObject["FullOutputFileName"]);
}

void PsDecayTimesGen(std::string filename="PsGenParams.json"){

  // Reading generation parameters from input JSON
  readParamFile(filename);

  // Number of events to gerate
  Long_t numberOfEvents = SOURCE_INTENSITY*SIMULATION_TIME;
  TRandom2 *rand = new TRandom2();

  Double_t deex_t = -1.0;
  Double_t oPs_t = -1.0;
  Double_t pPs_t = -1.0;

  TTree *partTree = new TTree("part", "Partial Gen Tree");
  SmallEvent *smallEvent;
  partTree->Branch("small", &smallEvent);

  TTree *fullTree = new TTree("full", "Full Gen Tree");
  BigEvent *bigEvent;
  fullTree->Branch("big", &bigEvent);

  // Histograms
  TH1F *oPsLifetime = new TH1F("oPsLifetime", "oPsLifetime [ps]", 100, 0.0, pow(10,6));
  TH1F *pPsLifetime = new TH1F("pPsLifetime", "pPsLifetime [ps]", 100, 0.0, pow(10,3));

  for(int i=0;i<numberOfEvents;i++){

    //Generating times of deexcitation photons - inital moment of event
    deex_t = rand->Uniform(0.0,SIMULATION_TIME*pow(10,12));
    smallEvent = new SmallEvent(i, deex_t, SmallEvent::deexcitation);
    SmallEvent tmp_deex = smallEvent;
    SmallEvent tmp_Ps;
    partTree->Fill();

    if(rand->Uniform() < 1.0/OPS2PPS_RATIO){
      //pPs generation
      Double_t temp_pPs_t = rand->Exp(PPS_LIFETIME);
      pPsLifetime->Fill(temp_pPs_t);
      pPs_t = temp_pPs_t+deex_t;
      smallEvent = new SmallEvent(i, pPs_t, SmallEvent::decay_pPs);
      tmp_Ps = *smallEvent;
      partTree->Fill();

    } else {
      //oPs generation
      Double_t temp_oPs_t = rand->Exp(OPS_LIFETIME);
      oPsLifetime->Fill(temp_oPs_t);
      oPs_t = temp_oPs_t+deex_t;
      smallEvent = new SmallEvent(i, oPs_t, SmallEvent::decay_oPs);
      tmp_Ps = *smallEvent;
      partTree->Fill();
    }
    bigEvent = new BigEvent(tmp_deex, tmp_Ps);
    fullTree->Fill();
  }

  TFile *output_part = new TFile(PARTIAL_OUTPUT_FILE_NAME.c_str(), "RECREATE");
  partTree->Write();
  output_part->Close();

  TFile *output_full = new TFile(FULL_OUTPUT_FILE_NAME.c_str(), "RECREATE");
  fullTree->Write();
  output_full->Close();

}
