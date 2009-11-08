function parse_histogram2d( fileprefix )

fileprefix = 'tango_dynamic_2d_histo_test/histogram_'

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

numCellsPerDim = (length(edges)-1);
if ( numCellsPerDim^2 ~= length(bins) ), error('Size of edges and bins not as expected'); end

Xh = edges(1:end-1) + diff(edges)/2;

rec = repmat(Xh,numCellsPerDim,1);
send=[];
for i=1:length(Xh)
    send = [send; ones(numCellsPerDim,1)*Xh(i)];
end
zeroInd = find(bins==0);
bins(zeroInd)=[];
send(zeroInd)=[];
rec(zeroInd)=[];

figure;
scatter(send,rec,bins,'ro','LineWidth',2)
grid
xlabel('send delay in ms')
ylabel('receive delay in ms')
axis equal


