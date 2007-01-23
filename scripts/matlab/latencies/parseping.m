function parseping( filename )
%PARSEPING
%   PARSEPING( FILENAME )
%   Post-process results of multiping shell script
%
%   Example:
%   $ ./multiping coffee 101 5 16 0.25 pingresults.txt
%   >> parseping( 'pingresults.txt' )   

fid=fopen( filename, 'r' );

% test case numbe by packet size
i = 0;

while 1
    tline = fgetl(fid);
    if ~ischar(tline), break, end

    if ( length(tline)>4 & strcmp( tline(1:4), 'size' ) )
        i = i + 1; % new size
        D(1,i) = sscanf( tline(6:end), '%d', 1 );
    end

    if ( length(tline)>4 & strcmp( tline(1:3), 'rtt' ) )
        % these are the results for the current size
        D(2:5,i) = sscanf( strrep(tline(23:end-3),'/',' '), '%f', [1,4] );
    end

end
fclose(fid);

% save parsed results
fid = fopen( [filename(1:end-3) 'dat'], 'w' );

fprintf( fid,  '%d %6.3f %6.3f %6.3f %6.3f\n', D );

fclose(fid);
