function [minMax, minMaxDev] = parse_histogram_fct( fileprefix )

filename_edges = [fileprefix 'edges.txt']
filename_bins = [fileprefix 'bins.txt']

fid = fopen( filename_edges );
if ( fid == -1 ), error(sprintf('Couldnt open file: %s\n',filename_edges)); end
C = textscan(fid, '%d');
edges = double(C{1,1});
fclose(fid);

fid = fopen( filename_bins );
if ( fid == -1 ), error(sprintf('Couldnt open file: %s\n',filename_bins)); end
C = textscan(fid, '%d');
bins = C{1,1};
fclose(fid);

if ( length(edges) ~= length(bins)+1 ), error('Size mismatch in edges/bins'); end

Xh = edges(1:end-1) + diff(edges)/2;

display('min/max interval')
minMax = Xh([min(find(bins)) max(find(bins))] )
display('min/max deviation from 106ms')
minMaxDev = Xh([min(find(bins)) max(find(bins))] ) - 106

%  figure(2),clf
figure
semilogy(Xh,bins,'b-*')
xlabel('Interval between messages (ms)')
ylabel('Frequency of occurance')
%  legend('transmitted (laser)','received (master log)')
title(fileprefix)