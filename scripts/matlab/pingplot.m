function h = pingplot( varargin )
%   PINGPLOT( D1,C1, D2,C2, D3,C3, ...)

% assert even number of inputs
if ( mod(nargin,2) ), error('must have even number of inputs'), end

N = nargin/2;

hold on

for i=1:N
    D = varargin{2*i-1};
    %sort by object size
    [jnk,ind]=sort(D(:,1));
    D = D(ind,:);

    ctype = varargin{2*i};

    n = D(:,1);
    tmin = D(:,2);
    tavg = D(:,3);
    tmax = D(:,4);
    tstd = D(:,5);

    
    for j=(1:length(n))
        hh(j) = plot(n(j)/1024*[1 1], [tmin(j) tmax(j)], 'k-');
        %hh(j) = plot(n(j)/1024*[1 1], [tmin(j) tmax(j)] / (n(j)/1024), 'k-');    
    end
    set(hh, 'color', 0.75*[1 1 1] )

    % if rrt values are provided
    if size(D,2)>5
        tmed = median( D(:,5:end), 2 );
        h(i) = plot( n/1024, tmed, ctype );
    else
        h(i) = plot( n/1024, tavg, ctype );
    end
    %h(i) = plot( n/1024, tavg ./ (n/1024), ctype );
end
