function [time, ranges, intensities] = parselaserdata( file )
% parse laser data from orca logger (ASCII)
% output args
%    time = each row is a separate individual timestamp
%    ranges = each row represents ranges from one scan corresponding to one timestamp
%    intensities = each row represents intensities from one scan corresponding to one timestamp

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',file));
end

% skip the header
for i=1:15
    % read a line
    fgetl(fid);
end

% time stamp format
s1 = '%f %f';

i=1;

while 1
    % read time stamp
    timeTmp = fscanf(fid, s1, 2);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 2
        break;
    end
    
    time(i,:) = timeTmp';

    % read start angle
    startAngle = fscanf(fid, '%d', 1);

    % read number of points in a scan
    numPoints = fscanf(fid, '%d', 1);

    % ranges and intensities format
    % only need to do this once
    if i==1    
        s2 = '%f ';
        sCat = '%f ';
        for j=1:numPoints-1
            s2 = strcat(s2,sCat);
        end
    end
    
    % read ranges
    rangesTmp = fscanf(fid, s2, numPoints);
    ranges(i,:) = rangesTmp;

    % read intensities
    intensitiesTmp = fscanf(fid, s2, numPoints);
    intensities(i,:) = intensitiesTmp;

    i=i+1;
    
end

fclose(fid);