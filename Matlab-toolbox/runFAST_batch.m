    
% -------------------------------------------------------------------------
% This script runs multiple FAST simulation with UNICO Simulink controller.
% The Parallel computing toolbox must be installed to use this function
% For each simulation a different discon.in file must be created. To run
%  multiple simulations the discon files and wind speed files must be found
%  divided in subfolders all within the same path. The subfolders must have
%  the same name as the .fst file of the respective simulation. Example
%  (test.fst, is the simulation file, the discon.in file should be found in
%  /../test/discon.in. For simulation test2, the discon file should be
%  found in /../test2/discon.in and so on)

%

clear;
[this_dir,~,~] = fileparts(mfilename('fullpath'));
input_file_name = ['discon.in'];                                                 % Controller input file

input_file_directory = ['C:\Users\utente\Desktop\Stable4Larisa\UNICO\Test_cases\UNIFI_SRWT\TurbulentTest\UNICO_files'];                                                 % Controller input file directory, folder where the discon.if file(s) are found

fast.FAST_SFuncDir     = 'C:\Users\utente\Desktop\TEST_UNICO_dev\Test_cases\openfast-3.5';                           % Path to SFunction. Compile FAST for use with simulink & mex using openfast docs
% fast.FAST_InputFile    = 'OF_stepDOWN_SL.fst';                              % OpenFAST input file (ext=.fst)
fast.FAST_directory    = ['C:\Users\utente\Desktop\Stable4Larisa\UNICO\Test_cases\UNIFI_SRWT' ...
    '\TurbulentTest'];                                                % Path to fst directory files

simu.SimModel          = fullfile('C:\Users\utente\Desktop\Stable4Larisa\UNICO\Src', ...
    'UNICO_simulink.slx');  
    [filepath,simu.SimModel,ext] = fileparts(simu.SimModel);
% Path to simulink model
cd(filepath)

addpath(genpath('C:\Users\utente\Desktop\Stable4Larisa\UNICO\Matlab-toolbox'));              % Import OpenFAST Matlab toolbox and ROSCO controller toolbox
% OpenFAST Matlab toolbox (https://github.com/OpenFAST/matlab-toolbox/tree/
% main) and ROSCO toolbox (https://github.com/NREL/ROSCO/tree/main/ROSCO_to
% olbox) are distributed under Apache 2.0 license. The toolbox distributed
% with the UNICO controller is adapted from the ROSCO and OpenFAST matlab
% toolboxes.
list_fst=dir(strcat(fast.FAST_directory,'\*.fst'));  %returns list of fst files
list_input=dir(input_file_directory);  %returns list of fst files
list_input(1:2) = [];

%% Simulink Setup

addpath(fast.FAST_SFuncDir);

%% Read FAST Files & Load UNICO Params from DISCON.IN
for i =1:length(list_fst)
    
    fast.FAST_InputFile    = list_fst(i).name;   % FAST input file (ext=.fst) get all fst files from directory
    [aux1,folder,aux2] = fileparts(fast.FAST_InputFile);
    input_file = strcat(input_file_directory,filesep,folder,filesep,input_file_name);
    
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


    % Out         = sim(SimulinkModel, 'StopTime', num2str(simu.TMax));
    % sigsOut     = get(Out,'sigsOut');   %internal control signals
end
%% Get OutData

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



