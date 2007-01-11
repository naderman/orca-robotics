function [timeStamps wifiData] = parsewifidata( file )
%
% function [timeStamps wifiData] = parsewifidata( file )
%
% Parses a wifi log file created by the Orca logger (ascii).
% Number of records is N. 
%
% Output arguments:
%    timeStamp:     
%       - array ( N x 2 ) 
%       - each row is a Unix timestamp [sec usec]
%    wifiData:      
%       - struct with fields:   interface, status, linkQuality, signalLevel, noiseLevel,
%                               numInvalidNwid, numInvalidCrypt, numInvalidFrag, 
%                               numRetries, numInvalidMisc, numMissedBeacons, mode,
%                               bitrate, accessPoint, throughPut, linkQualityType,
%                               maxLinkQuality, maxSignalLevel, maxNoiseLevel
%       - each field is a cell array of size ( N x numWirelessInterfaces )

% time stamp format
timeStampFormat = '%f %f'; 
wifiFormat ='%s %u %u %u %u %u %u %u %u %u %u %u %u %s %u %u %u %u %u';   

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: "%s"',file));
end

i=1;

while 1

    % read time stamp
    timeTmp = fscanf(fid, timeStampFormat, 2);
    
    % check if we have reached the end of the file
    if size(timeTmp,1) ~= 2
        break;
    end

    timeStamps(i,:) = timeTmp';
    
    % read number of interfaces
    numInterfaces = fscanf(fid, '%d');
    for k=1:numInterfaces
        data = textscan(fid, wifiFormat, 1);
        wifiData.interface(i,k)         = data{1};
        wifiData.status(i,k)            = data{2};
        wifiData.linkQuality(i,k)       = data{3};
        wifiData.signalLevel(i,k)       = data{4};
        wifiData.noiseLevel(i,k)        = data{5};
        wifiData.numInvalidNwid(i,k)    = data{6};
        wifiData.numInvalidCrypt(i,k)   = data{7};
        wifiData.numInvalidFrag(i,k)    = data{8};
        wifiData.numRetries(i,k)        = data{9};
        wifiData.numInvalidMisc(i,k)    = data{10};
        wifiData.numMissedBeacons(i,k)  = data{11};
        wifiData.mode(i,k)              = data{12};
        wifiData.bitrate(i,k)           = data{13};
        wifiData.accessPoint(i,k)       = data{14};
        wifiData.throughPut(i,k)        = data{15};
        wifiData.linkQualityType(i,k)   = data{16};
        wifiData.maxLinkQuality(i,k)    = data{17};
        wifiData.maxSignalLevel(i,k)    = data{18};
        wifiData.maxNoiseLevel(i,k)     = data{19};
    end
    fgetl(fid); % to get to the next line
    
    i=i+1;

end

fclose(fid);
    
    
  