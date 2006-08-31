function mark(ax,v, c,s)
%MARK draws horizontal or vertical lines on existing plot
%	MARK(AX,V)
%	MARK(AX,V,COLOR,STYLE)
%
%	Alex Makarenko, 6/00, v.2

% v.1 6/2/00
% v.2 7/5/00 added color and style.

v = v(:).';

if nargin<3 | isempty(c), c = [.33 .33 .33]; end
if nargin<4 | isempty(s), s = ':'; end

switch ax(1)
case 'x'
   yl = ylim';
   h = line([1;1]*v, yl*ones(size(v)));
   
case 'y'
   xl = xlim';
   h = line(xl*ones(size(v)), [1;1]*v);
   
end

set(h, 'color',c, 'linestyle',s)