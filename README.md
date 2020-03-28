# C++ implementation of REST API client for Xilinx ML Suit

## Run the Test Client

The test client will just send an array of zeros

1. Install `libcurl4-gnutls-dev`
2. Change the server address in `Config.cpp`
3. Run `make`
4. Run `main.out`

## Run the Test Sever

The test sever will just return `None` as the result

1. Replace `app.py` in the [Xilinx example](https://github.com/Xilinx/ml-suite/blob/master/examples/caffe/REST/app.py) with the `runserver.py` in this repo.


## Run the FPGA Server

1. Add inbound rule for the port in AWS to allow external connection
2. Add port forwarding in the docker (add the `-p 5000:5000` in `docker_run.sh`)
3. Replace `app.py` in the [Xilinx example](https://github.com/Xilinx/ml-suite/blob/master/examples/caffe/REST/app.py) with the version in this repo.

## Run SONIC Client in CMS LPC

1. Download `setup.sh` from [SONIC](https://github.com/LouYu2015/SonicCMS)
2. Set up SONIC: `./setup.sh -f LouYu2015 -p RestAPI -j 8`
  (or replace `LouYu2015` with your own GitHub username if you have another fork)
3. Go to folder `SonicCMS/CMSSW_10_6_6/src/SonicCMS/RestAPI/python`
4. Enter CMS environment:
  * `source /cvmfs/cms.cern.ch/cmsset_default.sh`
  * `cmsenv`
5. Run `cmsRun jetImageTest_mc_cfg.py address=ec2-54-187-97-197.us-west-2.compute.amazonaws.com port=5000 batchSize=10 maxEvents=25` (Replace address and port with your server address/port. Change batch size and max events if needed. Max events is the number of events that we want to test for.) The initialization process is slow, so please wait patiently.

## Update SONIC Client:

1. Go to folder `SonicCMS/CMSSW_10_6_6/src/SonicCMS/`
2. Fetch update: `git pull`
3. Build: `scram b`

## Troubleshoot: If Failed to Open the Test Data:

copy the data from CMS LPC with

```
cp /uscms_data/d3/pedrok/phase2/brainwave/abs/test/CMSSW_10_6_6/src/SonicCMS/Core/data/store_mc_RunIISpring18MiniAOD_BulkGravTohhTohbbhbb_narrow_M-2000_13TeV-madgraph_MINIAODSIM_100X_upgrade2018_realistic_v10-v1_30000_24A0230C-B530-E811-ADE3-14187741120B.root SonicCMS/CMSSW_10_6_6/src/SonicCMS/Core/data/store_mc_RunIISpring18MiniAOD_BulkGravTohhTohbbhbb_narrow_M-2000_13TeV-madgraph_MINIAODSIM_100X_upgrade2018_realistic_v10-v1_30000_24A0230C-B530-E811-ADE3-14187741120B.root
```
