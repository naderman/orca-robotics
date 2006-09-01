function [time, position2d] = parseposition2ddata( file )
% parse file created by orca logger (ASCII)
% output args
%    time = each row is a separate individual timestamp
%    posistion2d = each row is a pose for each timestamp

[day, month, year, hour, min, sec, posX, posY, orientation, velX, velY, velOrientation]...
     =textread(file, '%2d%*1s%2d%*1s%2d %2d%*1s%2d%*1s%f \n %f %f %f %f %f %f', 'headerlines',1);

time = [day month year hour min sec];
position2d = [posX posY orientation velX velY velOrientation];
