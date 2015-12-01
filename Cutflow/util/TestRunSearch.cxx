#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ScanDir.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoopGrid/PrunDriver.h"
#include "SampleHandler/DiskListLocal.h"
#include <TSystem.h>
#include "SampleHandler/ScanDir.h"
#include "xAODMuon/MuonContainer.h"
#include "Cutflow/SearchInxAOD.h"
#include <EventLoopAlgs/NTupleSvc.h>
#include <EventLoop/OutputStream.h>
#include "Cutflow/Boildown.h"

using namespace std;

int main( int argc, char* argv[]) {
char gridon = '1';
    // Take the submit directory from the input if provided:
    std::string submitDir = "submitDir";
    if( argc > 1 ) submitDir = argv[ 1 ];

    // Set up the job for xAOD access:
    xAOD::Init().ignore();

    // Construct the samples to run on:
    SH::SampleHandler sh;

    // use SampleHandler to scan all of the subdirectories of a directory for particular MC single file:


    // for grid
    if (*argv[2] == gridon) {
        SH::scanDQ2(sh,
                    "mc15_13TeV.361610.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZqqll_mqq20mll20.merge.AOD.e4054_s2608_s2183_r6630_r6264/");
    }
    else {
        const char *inputFilePath = gSystem->ExpandPathName("/hep/storage/thoresen/ZZllqq_xAOD/mc15_13TeV.361610.PowhegPy8EG_CT10nloME_AZNLOCTEQ6L1_ZZqqll_mqq20mll20.merge.AOD.e4054_s2608_s2183_r6630_r6264/");
        SH::ScanDir().filePattern("AOD.05771747._000001.pool.root.1").scan(sh, inputFilePath);
    }


    // Set the name of the input TTree. It's always "CollectionTree"
    // for xAOD files.
    sh.setMetaString( "nc_tree", "CollectionTree" );

    // Print what we found:
    sh.print();


    // Create an EventLoop job:
    EL::Job job;
    job.sampleHandler( sh );
    //job.options()->setDouble (EL::Job::optMaxEvents, 4000);

    // Add our analysis to the job:
    Boildown* alg = new Boildown();
    //SearchInxAOD* alg = new SearchInxAOD();
    job.algsAdd( alg );

    // define an output and an ntuple associated to that output
    EL::OutputStream output("search_out");
    job.outputAdd (output);
    EL::NTupleSvc *ntuple = new EL::NTupleSvc ("search_out");
    job.algsAdd (ntuple);
    alg->outputName = "search_out"; // give the name of the output to our algorithm


    // for grid
    if (*argv[2] == gridon) {
        EL::PrunDriver driver;
        driver.options()->setString("nc_outputSampleName",
                                    "user.fthorese.Searching_for_Z.TEST.%in:name[2]%.%in:name[6]%");
        driver.submit( job, submitDir );
    }
    else {
        EL::DirectDriver driver;
        driver.options()->setString("nc_outputSampleName",
                                    "user.fthorese.Searching_for_Z.TEST.%in:name[2]%.%in:name[6]%");
        driver.submit( job, submitDir );
    }



    return 0;
}