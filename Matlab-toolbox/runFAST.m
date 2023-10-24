
% -------------------------------------------------------------------------
% This script runs a single FAST simulation with UNICO Simulink controller.  

clear;
[this_dir,~,~] = fileparts(mfilename('fullpath'));

input_file = ['E:\RUN\UNICO\GitHubRelease\Test_cases\UNIFI_SRWT' ...
    '\StepTest\discon.in'];                                                 % Controller input file

fast.FAST_SFuncDir     = 'E:\RUN\UNICO\OFv3.5.0';                           % Path to SFunction. Compile FAST for use with simulink & mex using openfast docs
fast.FAST_InputFile    = 'OF_stepUP_SL.fst';                              % OpenFAST input file (ext=.fst)
fast.FAST_directory    = ['E:\RUN\UNICO\GitHubRelease\Test_cases' ...
   '\UNIFI_SRWT\StepTest'];                                                % Path to fst directory files

simu.SimModel          = fullfile('E:\RUN\UNICO\GitHubRelease\src', ...   
                         'UNICO_simulink.slx');                                                                 % Path to simulink model


addpath(genpath('E:\RUN\UNICO\GitHubRelease\matlab-toolbox'));              % Import OpenFAST Matlab toolbox and ROSCO controller toolbox
% OpenFAST Matlab toolbox (https://github.com/OpenFAST/matlab-toolbox/tree/
% main) and ROSCO toolbox (https://github.com/NREL/ROSCO/tree/main/ROSCO_to
% olbox) are distributed under Apache 2.0 license. The toolbox distributed
% with the UNICO controller is adapted from the ROSCO and OpenFAST matlab 
% toolboxes. 

%% Simulink Setup

addpath(fast.FAST_SFuncDir);

%% Read FAST Files & Load UNICO Params from DISCON.IN

[Param,Cx] = ReadWrite_FAST(fast);

% Simulation Parameters
simu.TMax   = Param.FP.Val{contains(Param.FP.Label,'TMax')};
simu.dt   = Param.FP.Val{contains(Param.FP.Label,'DT')};

%% Premake OutList for Simulink

OutList = {'Time'};
OutList = [OutList;
    Param.IWP.OutList;
    Param.EDP.OutList;
    Param.ADP.OutList;
    Param.SvDP.OutList;
    ];

for iOut = 2:length(OutList)
    OutList{iOut} = OutList{iOut}(2:end-1); %strip "s
end
% simu.TMax = 1;


%% Exectute FAST


Control.DT = simu.dt;

% Using Simulink/S_Func
FAST_InputFileName = [fast.FAST_directory,filesep,fast.FAST_InputFile];
TMax               = simu.TMax;

SimulinkModel = simu.SimModel;

Out         = sim(SimulinkModel, 'StopTime', num2str(simu.TMax));
% sigsOut     = get(Out,'sigsOut');   %internal control signals

%% Get OutData

SFuncOutStr = '.SFunc';

% Try text first, then binary
[OutData,OutList] = ReadFASTtext([fast.FAST_directory,filesep,fast.FAST_InputFile(1:end-4),SFuncOutStr,'.out']);
if isempty(OutData)
    [OutData,OutList] = ReadFASTbinary([fast.FAST_directory,filesep,fast.FAST_InputFile(1:end-4),SFuncOutStr,'.outb']);
end

% Dump data to structure
for i = 1:length(OutList)
    simout.(OutList{i}) = OutData(:,i);
end

%% Plot
Pl_FastPlots(simout)



