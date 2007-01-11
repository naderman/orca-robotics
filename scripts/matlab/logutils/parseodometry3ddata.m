function [time, position3d] = parseodometry3ddata( file )
% parse file created by orca logger (ASCII)
% output args
%    time = each row is a separate individual timestamp
%    posistion3d = each row is a pose for each timestamp

[sec, usec, posX, posY, posZ, roll, pitch, yaw,...
    velX, velY, velZ, velAngularX, velAngularY, velAngularZ]...
     =textread(file, '%f %f \n %f %f %f %f %f %f \n %f %f %f %f %f %f', 'headerlines',1);

time = [sec usec];
position3d = [posX posY posZ roll pitch yaw velX velY velZ velAngularX velAngularY velAngularZ];
