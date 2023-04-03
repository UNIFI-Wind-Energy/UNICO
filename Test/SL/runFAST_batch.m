% runFAST.m
%% This script runs multiple OPENFast simulations using matlab simulink
%When launching the code the user MUST be in the folder containing the simulink model. In this case Controller_eff 
% the file launches all the simulations defined by the .fst file in the direcory fast.FAST_rirectory
clear;

model_path = 'D:\Tesi_Magistrale\simu_openf_controller\SRWT_50kWUnifi\Matlab_Toolbox\Simulink'; %the model path is used as input to move where the model file is
cd(model_path)
% [this_dir,~,~] = fileparts(mfilename('fullpath'));
[this_dir,~,~] = fileparts(pwd);   %get parent directory of where the model is found

% Compile FAST for use with simulink & mex using openfast docs
% fast.FAST_SFuncDir     = '/Users/dzalkind/Tools/openfast-sim/glue-codes/simulink/src';  %%%% NEED FOR SIMULINK
fast.FAST_SFuncDir     = 'D:\Tesi_Magistrale\simu_openf_controller\openfast-2.4.0\build\bin';
% fast.FAST_InputFile    = 'OF_ws12_ti0_s213_y0_i0.fst';   % FAST input file (ext=.fst)
fast.FAST_directory    = 'D:\Tesi_Magistrale\simu_openf_controller\SRWT_50kWUnifi\Test_Cases';   % Path to fst directory files
list_fst=dir(strcat(fast.FAST_directory,'\*.fst'));  %returns list of fst files

for i =1:length(list_fst)
    fast.FAST_InputFile    = list_fst(i).name;   % FAST input file (ext=.fst)

% Simulink Parameters
% Model
% simu.SimModel           = fullfile(this_dir,'Simulink','Controller_eff');
simu.SimModel           ='Controller_eff';   %sbatch only works providing the file name and by launching from the same folder

% Script for loading parameters
simu.ParamScript        = fullfile(this_dir,'Utilities','load_SRWT_params');

%% Simulink Setup

[ControlScriptPath,ControScript] = fileparts(simu.ParamScript);
addpath(ControlScriptPath);
addpath(fast.FAST_SFuncDir);
% addpath('Utilities')
 addpath(genpath('D:\Tesi_Magistrale\simu_openf_controller\matlab-toolbox-main\matlab-toolbox-main')); % TODO adapt me
%% Read FAST Files & Load ROSCO Params from DISCON.IN

[Param,Cx] = ReadWrite_FAST(fast);

% Simulation Parameters
simu.TMax   = Param.FP.Val{contains(Param.FP.Label,'TMax')};
simu.dt   = Param.FP.Val{contains(Param.FP.Label,'DT')};
[R,F] = feval(ControScript,Param,simu);

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


%% Exectute FAST

% Using Simulink/S_Func
FAST_InputFileName = [fast.FAST_directory,filesep,fast.FAST_InputFile];
TMax               = simu.TMax;

SimulinkModel = simu.SimModel;


simIn = Simulink.SimulationInput(SimulinkModel); %create simulink input file for sbatch


job = batchsim(simIn,"TransferBaseWorkspaceVariables","on"); %launch batch simulation. The variables from the workspace are loaded into the sbatch function, as it requires the inputs
end

% Out = fetchOutputs(job1);

% % Out         = sim(SimulinkModel, 'StopTime', num2str(simu.TMax));
% sigsOut     = get(Out,'sigsOut');   %internal control signals
% 
% %% Get OutData
% 
% SFuncOutStr = '.SFunc';
% 
% % Try text first, then binary
% [OutData,OutList] = ReadFASTtext([fast.FAST_directory,filesep,fast.FAST_InputFile(1:end-4),SFuncOutStr,'.out']);
% if isempty(OutData)
%     [OutData,OutList] = ReadFASTbinary([fast.FAST_directory,filesep,fast.FAST_InputFile(1:end-4),SFuncOutStr,'.outb']);
% end
% 
% % Dump data to structure
% for i = 1:length(OutList)
%     simout.(OutList{i}) = OutData(:,i);
% end
% 
% %% Plot
% Pl_FastPlots(simout)



