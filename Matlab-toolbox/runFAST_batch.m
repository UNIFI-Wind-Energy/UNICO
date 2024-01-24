
% -------------------------------------------------------------------------
% This script runs multiple FAST simulation with UNICO Simulink controller.
% The Parallel computing toolbox must be installed to use this function
% runFAST_batch runs the simulations for all fst files in the directory
% fast.FAST_directory

clear;
[this_dir,~,~] = fileparts(mfilename('fullpath'));

input_file = ['C:\Users\utente\Desktop\TEST_UNICO_dev\Test_cases\UNIFI_SRWT_simulink\StepTest' ...
    '\discon.in'];                                                 % Controller input file

fast.FAST_SFuncDir     = 'C:\Users\utente\Desktop\TEST_UNICO_dev\Test_cases\openfast-3.5';                           % Path to SFunction. Compile FAST for use with simulink & mex using openfast docs
fast.FAST_directory    = ['D:\Controller_TORQUE\Simulink\Test_cases'...
    '\StepTest'];                                                % Path to fst directory files

simu.SimModel          = fullfile('C:\Users\utente\Desktop\UNICO_dev_test\simulink', ...   
                         'UNICO_simulink.slx'); % Path to simulink model
  
    [filepath,simu.SimModel,ext] = fileparts(simu.SimModel);
cd(filepath) % the working directory must be the same as the simulink model to make the parallel simulations work

addpath(genpath('C:\Users\utente\Desktop\TEST_UNICO_dev\Matlab-toolbox'));              % Import OpenFAST Matlab toolbox and ROSCO controller toolbox
% OpenFAST Matlab toolbox (https://github.com/OpenFAST/matlab-toolbox/tree/
% main) and ROSCO toolbox (https://github.com/NREL/ROSCO/tree/main/ROSCO_to
% olbox) are distributed under Apache 2.0 license. The toolbox distributed
% with the UNICO controller is adapted from the ROSCO and OpenFAST matlab
% toolboxes.
list_fst=dir(strcat(fast.FAST_directory,'\*.fst'));  %returns list of fst files


%% Simulink Setup

addpath(fast.FAST_SFuncDir);

%% Read FAST Files & Load UNICO Params from DISCON.IN
for i =1:length(list_fst)

    fast.FAST_InputFile    = list_fst(i).name;   % FAST input file (ext=.fst) get all fst files from directory

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
    simIn = Simulink.SimulationInput(SimulinkModel); %create simulink input file for sbatch
    job = batchsim(simIn,"TransferBaseWorkspaceVariables","on"); %launch batch simulation. The variables from the workspace are loaded into the sbatch function, as it requires the inputs


end




