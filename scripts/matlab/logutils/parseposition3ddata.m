function [time, position3d] = parseposition3ddata( file )
% parse file created by orca logger (ASCII)
% output args
%    time = each row is a separate individual timestamp
%    posistion3d = each row is a pose for each timestamp

[day, month, year, hour, min, sec, posX, posY, posZ, roll, pitch, yaw,...
    velX, velY, velZ, velAngularX, velAngularY, velAngularZ]...
     =textread(file, '%2d%*1s%2d%*1s%2d %2d%*1s%2d%*1s%f \n %f %f %f %f %f %f \n %f %f %f %f %f %f', 'headerlines',1);

time = [day month year hour min sec];
position3d = [posX posY posZ roll pitch yaw velX velY velZ velAngularX velAngularY velAngularZ];
