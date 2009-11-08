clear all
format compact

directory = 'tango_laser_cs_cf';
directory = 'tango_static_cs_cf';
directory = 'tango_dynamic_cs_cf';

%  directory = 'tango_laser_gen_cf_50k';
%  directory = 'tango_static_gen_cf_50k';
%  directory = 'tango_dynamic_gen_cf_50k';
directory = 'tango_static_gen_cf_50k_10s';
directory = 'tango_dynamic_gen_cf_50k_10s';
directory = 'tango_static_gen_cf_20k_10s_quiet';
%  directory = 'tango_dynamic_gen_cf_20k_10s_quiet';

directory = 'tango_laser_cs_cf_20k_5s';
directory = 'tango_static_cs_cf_20k_5s';
directory = 'tango_dynamic_cs_cf_20k_5s';

directory = 'tango_laser_cs_cf_20k_5s_quiet';
%  directory = 'tango_static_gen_cf_20k_10s_quiet';
%  directory = 'tango_dynamic_gen_cf_20k_10s_quiet';
directory = 'tango_dynamic_cs_cf_480k_5s_quiet';

fileprefix = [directory '/histogram_'];
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
Xh([min(find(bins)) max(find(bins))] )
display('min/max deviation from 106ms')
Xh([min(find(bins)) max(find(bins))] ) - 106

figure(2),clf
semilogy(Xh,bins,'b-*')
xlabel('Interval between messages (ms)')
ylabel('Frequency of occurance')
%  legend('transmitted (laser)','received (master log)')
