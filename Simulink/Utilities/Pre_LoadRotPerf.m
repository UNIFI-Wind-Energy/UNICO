function RotPerf = Pre_LoadRotPerf(RotPerfFile)
% Load rotor performance information from a text file. 
% Loads OpenFast model output into a MATLAB structure to be post processed
%
% Inputs: RotPerfFile - Cp_Ct_Cq.txt file from CCBlade/ROSCO
% Outputs: RotorPerformance - Structure containing rotor performance data
%
% Nikhar Abbas

FixedPitch = -5.0;   % ADDED_SRWT % Fixed blade pitch angle [deg] 

fid = fopen(RotPerfFile, 'r');
if fid == -1, error('Error loading file'), end

% Read first line to start tline
tline = fgetl(fid);
while ~feof(fid)
    tline = fgetl(fid);
    l_str = strsplit(tline);
    if tline
        % Find pitch angle vector
        if strcmpi(l_str{2},'Pitch')
           tline = fgetl(fid);
           BlPitch = str2num(tline);

           % ADDED_SRWT
           for i = 1:length(BlPitch)          % Find fixed pitch angle column index
               if BlPitch(i) == FixedPitch 
                   Angleind = i;
                   break
               end
           end
           % END-ADDED

        % Find TSR vector
        elseif strcmpi(l_str{2},'TSR')
            tline = fgetl(fid);
            TSR = str2num(tline);   
        % Find Wind speed vector
        elseif strcmpi(l_str{2},'Wind')
            tline = fgetl(fid);
            Wind = str2num(tline);   

         % Read Cp table
        elseif strcmpi(l_str{2},'Power')
            tline = fgetl(fid);
            Cpmat = zeros(length(TSR),length(BlPitch));
            for pind = 1:length(TSR)
                tline = fgetl(fid);
                CpMat(pind,:) = str2num(tline);
            end
            

            % ADDED_SRWT
            if ~isempty(Angleind)           
                CpInv = CpMat(:,Angleind)';    % Create 1D array with power coefficient values at fixed pitch angle 
                [Cpmax,maxind] = max(CpInv);   % Extract maximum power coefficient value and its array index
                for i = length(CpInv):-1:1     % Delete CpMat elements with higher index than maximum Cp value:
                    if i > maxind              % this is for Cp (x-axis) vs TSR (y-axis) interpolation chart
                        CpInv(i) = [];
                    end
                end
                TSRcp = TSR;
                for i = length(TSR):-1:1       % ...same for TSR array
                    if i > maxind
                        TSRcp(i) = [];
                    end
                end
            end
            CpMat = CpMat(:,Angleind)';
            % END_ADDED
            
        % Read Ct table
        elseif strcmpi(l_str{2},'Thrust')
            tline = fgetl(fid);
            Ctmat = zeros(length(TSR),length(BlPitch));
            for pind = 1:length(TSR)
                tline = fgetl(fid);
                CtMat(pind,:) = str2num(tline);
            end
            
        % Read Cq table
        elseif strcmpi(l_str{2},'Torque')
            tline = fgetl(fid);
            Cqmat = zeros(length(TSR),length(BlPitch));
            for pind = 1:length(TSR)
                tline = fgetl(fid);
                CqMat(pind,:) = str2num(tline);
            end
            % ADDED_SRWT
            if ~isempty(Angleind)
                CqMat = CqMat(:,Angleind)';
            end
            % END_ADDED
        end
    end
end
    
    


% Save Structure
RotPerf.BlPitch = BlPitch;
RotPerf.TSR = TSR;
RotPerf.TSRcp = TSRcp;  % ADDED_SRWT
RotPerf.Wind = Wind;
RotPerf.Cpinv = CpInv;  % ADDED_SRWT
RotPerf.Cpmat = CpMat;
RotPerf.Ctmat = CtMat;
RotPerf.Cqmat = CqMat;
RotPerf.Cpmax = Cpmax;  % ADDED_SRWT

end