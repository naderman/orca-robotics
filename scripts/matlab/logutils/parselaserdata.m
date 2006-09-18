function [time, ranges, intensities] = parselaserdata( file )
% parse laser data from orca logger (ASCII)
% output args
%    time = each row is a separate individual timestamp
%    ranges = each row represents ranges from one scan corresponding to one timestamp
%    intensities = each row represents intensities from one scan corresponding to one timestamp


nscans = 361;

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
s1 = '%2d%*1s%2d%*1s%2d %2d%*1s%2d%*1s%f';

% ranges and intensities format
s2 = '%f ';
sCat = '%f ';
for j=1:nscans-1
   s2 = strcat(s2,sCat);
end

i=1;

while 1
    % read time stamp
    timeTmp = fscanf(fid, s1, 6);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 6
        break;
    end
    
    time(i,:) = timeTmp';

    % read start angle
    startAngle = fscanf(fid, '%d', 1);
    
    % read ranges
    rangesTmp = fscanf(fid, s2, nscans);
    ranges(i,:) = rangesTmp;

    % read intensities
    intensitiesTmp = fscanf(fid, s2, nscans);
    intensities(i,:) = intensitiesTmp;

    i=i+1;
    
end

fclose(fid);