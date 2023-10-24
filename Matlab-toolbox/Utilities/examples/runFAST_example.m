%% Documentation   
% Example script to run a set of FAST simulations using FAST exe.
%
% NOTE: This script is only an example. 
%       The example data is suitable for a given version of OpenFAST and might need to be adapted
%     
% Adapt this script to your need, by calling the different subfunctions presented.
%
%% Initialization
clear all; close all; clc; 
restoredefaultpath;
% addpath(genpath('C:/Work/FAST/matlab-toolbox')); % TODO adapt me
addpath(genpath('G:\Drive condivisi\MsC Cocchi - Controller development\Controller_MATLAB_interface\matlab-toolbox-main\matlab-toolbox-main')); % TODO adapt me


%% Parameters
FASTexe = 'G:\Drive condivisi\MsC Cocchi - Controller development\Controller_MATLAB_interface\matlab-toolbox-main\matlab-toolbox-main/openfastx64s.exe'; % path to an OpenFAST executable
OUText  = '.outb'; % Output extension

FSTfilenames={ % path to OpenFAST input files
'G:\Drive condivisi\MsC Cocchi - Controller development\Controller_MATLAB_interface\matlab-toolbox-main\matlab-toolbox-main\_ExampleData\5MW_Land_Simulations/Main_5MW_Land_8mps.fst'};


%% Run FAST an all fst files 
runFAST(FSTfilenames, FASTexe); %, 'flag','-VTKLin');

%% List of output files
OUTfilenames =  cellfun(@(s)strrep(s,'.fst',OUText),  FSTfilenames, 'UniformOutput',false);

for i = 1:length(OUTfilenames)
    if ~exist(OUTfilenames{i},'file')
        warning('Simulation failed: %s', FSTfilenames{i})
    end
end
