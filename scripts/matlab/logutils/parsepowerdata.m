function [time, voltage, charge, chargingState, secRemaining ] = parsepowerdata( filename )
%
% function [time, batteries] = parsepowerdata( filename )
%
% Author: Tobias Kaupp
%
% Parses the power data from orca logger (ASCII)
% output arguments
%    time =          each row is a Unix timestamp [sec usec]
%    voltage =       (numRecords x numBatteries) in V
%    charge =        (numRecords x numBatteries) in %
%    chargingState = cell array (numRecords x numBatteries)
%    secRemaining =  (numRecords x numBatteries) in s
%

fid = fopen( filename );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',filename));
end

i=1;
while true
    % read time stamp
    timeTmp = fscanf(fid, '%f %f', 2);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 2
        break;
    end
    
    time(i,:) = timeTmp'

    % read number of batteries
    numBatteries = fscanf(fid, '%d', 1)
    
    for j=1:numBatteries
        
        % read voltage, percent
        batteryTmp = fscanf(fid, '%f %f', 2);
        if ( length(batteryTmp) ~= 2 )
            error('Malformed battery record.');
        end
        voltage(i,j) = batteryTmp(1);
        charge(i,j) = batteryTmp(2);
        
        % read charging state (need to read character by character)
        fscanf(fid,'%c',1);
        chargingStateTmp = [];
        while(true)
            singleChar = fscanf(fid,'%c',1);
            if singleChar==' ' break; end;
            chargingStateTmp = [chargingStateTmp singleChar];
        end
        chargingState{i,j} = chargingStateTmp;
        
        % read seconds remaining
        secRemaining(i,j) = fscanf(fid, '%f', 1);
    end

    i=i+1;
    
end

fclose(fid);