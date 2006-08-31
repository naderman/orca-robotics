%function dispping( filename )
% load results of pings (unix and orca) and plot them
clear all
IS_PRINT = 0;

% configs
xmech_lab = {'orca', 'ping', 'iceping' };
%  route_lab = { 'coffee->coffee', 'answer->question', 'coffee->thistle',  'coffee->question (wireless)' };
xscale = [1e-2 1e3];
yscale = [9e-3 3e2]; % [8e-3 8e3];

figure(1),clf, orient tall, hold on

% ICEBOX

D0o= load(  'pingdata/icebox_lin.dat' );  % RC4
%D0p= load(  '212/coffee_icebox_ice_101_1.dat' );  % ice ping

%figure(10),clf
subplot(131)
h=pingplot( D0o, 'b-s' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
mark('y', 0.03671 );
ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, xmech_lab{:}, 'location', 'northwest');
title('IceBox') %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])


% LOCAL HOST

D1p= load(  'pingdata/samehost_ping.dat' );  % ping
D1oLL4= load( '~/sys/pings/200rc4/agave_agave_orca_101_025.dat' );  % orca2 Linux-Linux
D1oLL= load( 'pingdata/samehost_lin.dat' );  % orca2 Linux-Linux
D1oWW= load( 'pingdata/samehost_win.dat' );  % orca2 Win-Win

% clean up data (updates the means)
%  D1s = pingclean(D1s);
%  D1c = pingclean(D1c);

%  figure(11),clf
subplot(132)
h=pingplot( D1oLL,'b-s', D1p,'k--o' );
%  h=pingplot( D1oLL,'b-s', D1oWW,'g-^', D1p,'k--o' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
mark('y', 0.21488 );
ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, 'orca linux', 'ping', 'location', 'northwest');
%  legend( h, 'orca linux', 'orca win', 'ping', 'location', 'northwest');
title('Intra-Host' ) %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])

% WIRED TO ANOTHER HOST

D2p= load( 'pingdata/twohosts_ping.dat' );  % struct
D2oLL= load( 'pingdata/twohosts_linlin.dat' );  % RC4
D2oLW= load( 'pingdata/twohosts_linwin.dat' );  % orca2
D2oWW= load( 'pingdata/twohosts_winwin.dat' );  % orca2


%  figure(12),clf
subplot(133)
h=pingplot( D2oLL,'b-s', D2p,'k--o' );
%  h=pingplot( D2oLL,'b-s', D2oWW,'g-^', D2oLW,'m-d', D2p,'k--o' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
mark('y', 0.43911 );
ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, 'orca linux', 'ping', 'location', 'northwest');
%  legend( h, 'orca linux', 'orca win', 'orca mix', 'ping', 'location', 'northwest');
title('Host-to-Host, wired') %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])

%if IS_PRINT, print -dpng 'rtt_all', end
