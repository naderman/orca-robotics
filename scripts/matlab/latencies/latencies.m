%function dispping( filename )
% load results of pings (unix and orca) and plot them
clear all
IS_PRINT = 0;

% configs
xmech_lab = {'orca', 'ping', 'iceping' };
%  route_lab = { 'coffee->coffee', 'answer->question', 'coffee->thistle',  'coffee->question (wireless)' };
xscale = [1e-2 1e3];
yscale = [5e-3 6e1];
xtick = 10.^(-2:3);

figure(1),clf, orient tall, hold on

% ICEBOX

D0o= load(  'pingdata/icebox_lnx.dat' );
%  D0agav= load(  'pingdata/icebox_lnx_agave.dat' );
%  D0burr= load(  'pingdata/icebox_lnx_burrito.dat' );
%D0p= load(  '212/coffee_icebox_ice_101_1.dat' );  % ice ping

%figure(10),clf
subplot(131)
h=pingplot( D0o, 'b-s' ); %, D0burr, 'g-o', D0agav, 'g--x' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
%  mark('y', 0.03671 );
set(gca,'xtick',xtick)
ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, 'orca linux', 'location', 'northwest');
title('IceBox') %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])


% LOCAL HOST

D1p= load(  'pingdata/samehost_ping.dat' );  % ping
D1oLL= load( 'pingdata/samehost_lnx.dat' );  % orca2 Linux-Linux
D1oWW= load( 'pingdata/samehost_win.dat' );  % orca2 Win-Win

%  D1pburr= load(  'pingdata/samehost_ping_burrito.dat' );  % ping
%  D1pagav= load(  'pingdata/samehost_ping_agave.dat' );  % ping
%  D1burr = load( 'pingdata/burr_burr_lnxlnx.dat' );
%  D1agav = load( 'pingdata/agave_agave_lnxlnx.dat' );

% clean up data (updates the means)
%  D1s = pingclean(D1s);
%  D1c = pingclean(D1c);

%  figure(11),clf
subplot(132)
h=pingplot( D1oLL,'b-s', D1p,'k--o' ); %, D1pburr,'r-o', D1pagav,'r--x', D1burr, 'g-o', D1agav, 'g--x' );
%  h=pingplot( D1oLL,'b-s', D1oWW,'g-^', D1p,'k--o' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
%  mark('y', 0.21488 );
set(gca,'xtick',xtick)
%  ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, 'orca linux', 'ping', 'location', 'northwest');
%  legend( h, 'orca linux', 'orca win', 'ping', 'location', 'northwest');
title('Intra-Host' ) %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])

% WIRED TO ANOTHER HOST

D2pLQ= load( 'pingdata/twohosts_ping_lnxqnx.dat' );
%  D2pLW100= load( 'pingdata/twohosts_ping_lnxqnx_100mb.dat' );
%  D2pLW1000= load( 'pingdata/twohosts_ping_lnxqnx_1000mb.dat' );
%  D2oLL= load( 'pingdata/twohosts_linlin.dat' );  % RC4
%  D2oLW= load( 'pingdata/twohosts_linwin.dat' );  % orca2
%  D2oWW= load( 'pingdata/twohosts_winwin.dat' );  % orca2
D2oLQ= load( 'pingdata/twohosts_lnxqnx.dat' );  % RC4
%  D2oLQ100= load( 'pingdata/twohosts_lnxqnx_100mb.dat' );
%  D2oLQ1000= load( 'pingdata/twohosts_lnxqnx_1000mb.dat' );

%  figure(12),clf
subplot(133)
h=pingplot( D2oLQ,'b-s', D2pLQ,'k--o'); %, D2pLW100,'r--x', D2pLW1000,'r-o', D2oLQ100,'g-o', D2oLQ1000,'m-o' );
%  h=pingplot( D2oLL,'b-s', D2oWW,'g-^', D2oLW,'m-d', D2p,'k--o' );
set(gca,'xscale','log')
set(gca,'yscale','log')
xlim(xscale)
ylim(yscale)
%  mark('y', 0.43911 );
set(gca,'xtick',xtick)
%  ylabel( 'Round Trip Time (ms)' )
xlabel( 'Object Size (KB)' )
legend( h, 'orca linux-qnx', 'ping', 'location', 'northwest');
%  legend( h, 'orca linux', 'orca win', 'orca mix', 'ping', 'location', 'northwest');
title('Host-to-Host, wired 1000Mb/s') %, 101 objects, interval=0.25s, preload=1')
wp=get(gcf,'position'); set(gcf,'position', [wp(1:2) 750 600])

%if IS_PRINT, print -dpng 'rtt_all', end
