function [times, gps] = gpsdata( file )
% function [times, gps] = gpsdata( file )
%
% parse gps data from orca logger (ASCII)
% output args
%    times =      Each row is a Unix timestamp [sec usec]
%    gps = Each cell represents gps information corresponding to
%                 one timestamp.
%                 Each cell is a matrix, where each each row
%                 describes gps info in the form:
%        [ utcTime.hours, utcTime.minutes, utcTime.seconds, latitude, longitude, altitude, horizontalPositionError, verticalPositionError,...
%        heading, speed, climbRate, satellites, positionType, geoidalSeparation]

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',file));
end

% time stamp format
timeStampFormat = '%f %f';

% gps format
formatGps = '%f %f %f %f %f %f %f %f %f %f %f %f %f %f';

i=1;
while true
    % read time stamp
    timeTmp = fscanf(fid, timeStampFormat, 2);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 2
        break;
    end
    
    times(i,:) = timeTmp';

    gpsTmp = fscanf(fid, formatGps, 14);
    if ( length(gpsTmp) ~= 14 )
        error('Malformed gps.');
    end

    gps(i,:) = gpsTmp;

    i=i+1;
    
end

fclose(fid);