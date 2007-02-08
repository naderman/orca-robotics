function [time, voltage, charge, secRemaining ] = parsepowerdata( file )
% function [time, batteries] = parselocalise2ddata( file )
%
% parse power data from orca logger (ASCII)
% output arguments
%    time =         each row is a Unix timestamp [sec usec]
%    voltage =      (numTimeStamps x numBatteries) in V
%    charge =       (numTimeStamps x numBatteries) in %
%    secRemaining = (numTimeStamps x numBatteries) in s


fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',file));
end

% time stamp format
timeStampFormat = '%f %f';

% battery format
formatBattery = '%f %f %f';

i=1;
while true
    % read time stamp
    timeTmp = fscanf(fid, timeStampFormat, 2);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 2
        break;
    end
    
    time(i,:) = timeTmp';

    % read number of batteries
    numBatteries = fscanf(fid, '%d', 1);
    
    for j=1:numBatteries
        
        % read [voltage, percent, secondsRemaining]
        batteryTmp = fscanf(fid, formatBattery, 3);
        if ( length(batteryTmp) ~= 3 )
            error('Malformed battery record.');
        end
        voltage(i,j) = batteryTmp(1);
        charge(i,j) = batteryTmp(2);
        secRemaining(i,j) = batteryTmp(3);
    end

    i=i+1;
    
end

fclose(fid);