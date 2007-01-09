function [timeStamps wifiData] = parsewifidata( file )
% parse file created by orca logger (ASCII)
% output args
%    timeStamp
%    wifiData

% time stamp format
timeStampFormat = '%2d%*1s%2d%*1s%2d %2d%*1s%2d%*1s%f'; 
wifiFormat ='%s %u %u %u %u %u %u %u %u %u %u %u %u %s %u %u %u %u %u';   

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: "%s"',file));
end

i=1;

while 1

    % read time stamp
    timeTmp = fscanf(fid, timeStampFormat, 6)
    
    % check if we have reached the end of the file
    if size(timeTmp,1) ~= 6
        break;
    end
    
    % convert into Matlab timestamp format
    timeStamps(i,:) = timeTmp';
    timeStamps(i,1) = timeTmp(3)+2000;
    timeStamps(i,2) = timeTmp(1);
    timeStamps(i,3) = timeTmp(2);
    
    % read number of interfaces
    numInterfaces = fscanf(fid, '%d');
    for k=1:numInterfaces
        data = textscan(fid, wifiFormat, 1)
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
    
    
  