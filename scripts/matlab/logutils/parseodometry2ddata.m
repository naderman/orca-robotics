function [time, position2d] = parseodometry2ddata( file )
% parse file created by orca logger (ASCII)
% output args
%    time = each row is a Unix timestamp
%    odometry2d = each row is a data record for each timestamp: posX, posY, orientation, velX, velY, velOrientation

error('AlexB: The Odometry ascii log file format has changed, and I have not updated this function: needs to be updated.');

[sec, usec, posX, posY, orientation, velX, velY, velOrientation]...
     =textread(file, '%f %f \n %f %f %f %f %f %f', 'headerlines',1);

time = [sec usec];
position2d = [posX posY orientation velX velY velOrientation];
