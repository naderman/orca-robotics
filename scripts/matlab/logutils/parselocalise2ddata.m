function [times, hypotheses] = parselocalise2ddata( file )
% function [times, hypotheses] = parselocalise2ddata( file )
%
% parse laser data from orca logger (ASCII)
% output args
%    times =      Each row is a separate individual timestamp
%    hypotheses = Each cell represents hypotheses corresponding to
%                 one timestamp.
%                 Each cell is a matrix, where each each row
%                 describes a hypothesis in the form:
%                 [meanX,meanY,meanO,Pxx,Pxy,Pyy,Pxt,Pyt,Ptt,weight]

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',file));
end

% time stamp format
formatTimestamp = '%2d%*1s%2d%*1s%2d %2d%*1s%2d%*1s%f';

% hypothesis format
formatHypothesis = '%f %f %f %f %f %f %f %f %f %f';

i=1;
while true
    % read time stamp
    timeTmp = fscanf(fid, formatTimestamp, 6);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 6
        break;
    end
    
    times(i,:) = timeTmp';

    % read num hypotheses
    numHypotheses = fscanf(fid, '%d', 1);
    
    for j=1:numHypotheses
        
        % read [meanX,meanY,meanO,Pxx,Pxy,Pyy,Pxt,Pyt,Ptt,weight]
        hypothesisTmp = fscanf(fid, formatHypothesis, 10);
        if ( length(hypothesisTmp) ~= 10 )
            error('Malformed hypothesis.');
        end
    
        hypotheses{i}(j,:) = hypothesisTmp;
    end

    i=i+1;
    
end

fclose(fid);