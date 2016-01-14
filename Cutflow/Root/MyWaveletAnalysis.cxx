#include "Cutflow/MyWaveletAnalysis.h"
#include <iostream>


using namespace std;
/*
* Tool for wavelet analysis
*/

MyWaveletAnalysis::MyWaveletAnalysis(int nPixel, double yRange){
}

vector<TLorentzVector> MyWaveletAnalysis ::  getChPFOwave(){
    return chPFOwave;
}

vector<TLorentzVector> MyWaveletAnalysis ::  getNeuPFOwave(){
    return neuPFOwave;
}


void MyWaveletAnalysis :: analyzeNeuPFO(vector<TLorentzVector> neuPFO) {

}


void MyWaveletAnalysis :: analyzeChPFO(vector<TLorentzVector> chPFO) {

}
