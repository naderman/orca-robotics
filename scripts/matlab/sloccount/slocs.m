% keep track of orca project size using sloccount
clear all

%%%%%%%%%% DATA %%%%%%%%%%%%%%%%%%%%

% include/* except: include/{Freeze IceSSL IcePatch2}
% src/{Ice Slice IceGrid icecpp IceStorm slice2cpp IceUtil IceBox IceXML}
%  Sice212 = [
%  (7842-1000) + (22093+18419+11499+10568+4392+4200+4252+567+316)
%  ];
%  Sice301 = [
%  (8164-1000) + (23018+19384+19169+10596+4479+4343+4308+622+319)
%  ];
% same as above, but now also add Java:
% src/{IceInternal Ice IceUtil IceBox}
%  Sice310 = [
%  (8810-1200) + (24275+20494+22689+10587+4569+4877+4770+667+319) + (13190+6915+1262+493)
%  ];
%  Sice311 = [
%  (8950-1275) + (24511+20505+22898+10590+4600+4884+4782+683+323) + (13266+6929+1262+504)
%  ];

% it's much easier to count the whole of Ice (we use most of it anyway)
% sloccount include/ src/
% (this does not account for .ice files in [ice]/slice/* but it's a small fraction of total)
Sice212 = 133523;
Sice301 = 144513;
Sice310 = 153243;
Sice311 = 154145;
Sice320 = 168475;
Sice321 = 169725;

% indeces
iyy=1;
imm=2;
idd=3;
iver=4;
isub=5;
ibase=6;
icomp=7;
iutil=8;
iad=9;

Sorca = [ ...
% YY    MM  DD  ?   SUB     BASE        COMP           UTILS    A&D VERSION
% 0.8.6: a single distro, divide in halves arbitrarily
2003     4  14  1   0       18930/2     18930/2             0   0   % 0.8.6 (last change in doc/ChangeLog
% BASE: orca-base
% COMP: orca-components
2004     9  13  1   0       42543       21726               0   0   % 0.11.0
2004    11  16  1   0       8963        27310               0   0   % 0.12.0
2004    12  23  0   0       10596       31845               0   0   % 0.12.1
2005     3  16  1   0       13767       43576               0   0   % 0.13.0
2005     3  21  0   0       13620       45874               0   0   % 0.13.1
2005     4   4  0   0       15630       45944               0   0   % 0.13.2
2005     4  16  0   0       12283       46666               0   0   % 0.13.3
2005     5   9  1   0       14639       46837               0   0   % 0.14.0
2005     7   2  1   0       19751       49854               0   0   % 0.15.0
2005    10  11  1   0       20818       52408               0   0   % 1.0.0
% BASE: just orcaice w/out test + interfaces/slice
% COMP: comp + util - orcaice 
2005    11  20  1   Sice212 2320+530    9226+8781-2320      0   0   % 2.0.0-rc1
2006     1  22  0   Sice301 2511+596    9801+9695-2511      0   0   % 2.0.0-rc2
% UTIL: sloccount components/log* 
2006     2  20  0   Sice301 3369+813    9867+14734-3369-1372 1372    0  % 2.0.0-rc3
% UTIL: sloccount components/orcaview/ components/log* utils/orcalog* utils/orcagui/ utils/orcaobj/
2006     3  29  0   Sice301 4022+1052   14625+18131-4022-9307 9307   0  % 2.0.0-rc4
2006     6  11  0   Sice310 3439+1116   19999+24867-3439-12670 12670 0  % 2.0.0-rc5  
2006     8   9  1   Sice310 3482+1116   19979+21711-3482-13111 13111 0  % 2.0.0
2006     9  18  0   Sice310 3647+1094   19317+30847-3647-15704 15704 0  % 2.0.1
2006    10   5  0   Sice310 3644+1094   20384+31798-3644-16329 16329 0  % 2.0.2
% UTIL: sloccount components/orcaview* components/log* utils/orcalog* utils/orcagui* utils/orcaobj/
2006    11  23  1   Sice311 3831+1178   21408+35311-3831-18359 18359 0  % 2.1.0
% BASE: cd src; sloccount libs/orcaice/ 
%          MINUS sloccount libs/orcaice/test 
%          PLUS sloccount interfaces/slice
% COMP: cd src; sloccount components/ libs/ 
%          MINUS sloccount libs/orcaice* libs/orcaq* libs/orcalog* libs/orcaprobe* libs/orcawall* libs/orcaobj libs/orcaobjutil/
% UTIL: cd src; sloccount utils libs/orcaq* libs/orcalog* libs/orcaprobe* libs/orcawall* libs/orcaobj libs/orcaice*
%          MINUS sloccount libs/orcaice/
% to count lines in *.ice files, rename them to *.cpp like this:
% $cd src/interfaces/slice/orca; for file in *; do mv $file `echo $file | sed s/.ice/.cpp/`; done
2006    12  17  0   Sice311 5615-1784+1240  60241-28024     32398-5615  0   % 2.1.1
2007     2   2  1   Sice311 6225-2011+1671  70376-31989     37297-6225  0   % 2.2.0
2007     4  10  1   Sice320 6232-2011+1671  72704-34165     39894-6232  0   % 2.3.0
2007     6   3  1   Sice320 6837-2056+1682  75506-34316     40074-6837  0   % 2.4.0
2007     8  13  1   Sice320 7247-2226+1772  78939-36926     42700-7247  0    % 2.5.0
2007     9  18  1   Sice321 7919-2572+1787  83838-38743     44694-7919  0    % 2.6.0
% A&D: Hydro: sloccount src
2007    10  28  1   Sice321 5481-1528+1787  72748-36062     42043-5481  12251    % 2.7.0
2007    12  10  0   Sice321 5348-1558+1787  64068-36370     44108-5348  23711    % 2.7.0+
];
Vorca ={'0.8.6', '0.11.0', '0.12.0', '0.12.1', '0.13.0', '0.13.1', '0.13.2', '0.13.3', '0.14.0', '0.15.0', '1.0.0', ...
    '2.0.0-rc1', '2.0.0-rc2', '2.0.0-rc3', '2.0.0-rc4', '2.0.0-rc5', '2.0.0', '2.0.1', '2.0.2', '2.1.0', '2.1.1', ...
    '2.2.0', '2.3.0', '2.4.0', '2.5.0', '2.6.0', '2.7.0', '2.7.0+' }';

% add a dummy release just for display
Sorca(end+1,:) = Sorca(end,:);
Sorca(end,imm) = Sorca(end,imm)+1;
Sorca(end,iver) = 0;
Vorca{end+1} = 'dummy';

% convert dates
Torca = datenum(Sorca(:,iyy),Sorca(:,imm),Sorca(:,idd));

% 2nd digit releases
i2 = (Sorca(:,iver)==1);
Torca2 = Torca(i2);
Vorca2 = Vorca(i2);

% what can go to Hydra?
% cd libs; sloccount bros1 orcacolourtext orcadynamicload orcageom2d orcagpsutil orcaiceutil orcaimage orcaimagegrabber orcalocalnav orcalockfile orcamapload orcamisc orcanavutil orcaogfusion orcaogmap orcapathplan orcaping orcaportability orcaserial orcawifi
% cd components; sloccount gps/garmin/ insgps/novatel insgps/novatelspan/ laser2d/laser2dutil/ laser2d/sickacfr/ laser2d/sickcarmen/ localnav/vfhdriver/ segwayrmp/rmpdriver/ 
% 15,102 + 10,322

% for comparison, ASN
%  Sasn = 90433;

Sgenom = [
% YY    MM  DD  ? SUB   BASE    COMP    UTIL    VERSION
% SUB : cd pocolibs-2.3; sloccount include src
% BASE: cd genom-2.0; sloccount include src
% COMP: 0
% UTIL: 0 
2007    8   31  1  9786    11368   0   0       % 2.0
];

Splayer = [ 
% YY    MM  DD  ?   SUB     BASE                COMP       UTIL  VERSION
% BASE: (server-drivers)+client_libs
% COMP: drivers + utils
2002    12 04   1  0    (17331-12037)+9188      12037+2685 0    % 1.3.1
2003    7  15   1  0    (30452-24334)+12458     24334+3064 0    % 1.4-rc1
2003    12 07   1  0    (37378-30947)+13110     30947+9633 0    % 1.4-rc2
2004    5  31   1  0    (49225-42643)+13664     42643+3550 0    % 1.5
2004    11 12   1  0    (50758-43190)+13545     43190+4479 0    % 1.6
2005    1  31   0  0    (52069-44517)+13576     44517+4662 0    % 1.6.2
2005    7  28   0  0    (53348-45686)+14300     45686+5188 0    % 1.6.5
% BASE: (libplayercore-./bindings) + client_libs + (libplayertcp-./bindings)
% note: libplayerxdr is autogenerated
% COMP: server + utils
2005    10 25   0  0    (4866-0)+7560+(1391-0)  46474+5725  0   % 2.0pre7
% SUB : 0
% BASE: sloccount libplayercore client_libs libplayertcp 
%           MINUS sloccount libplayercore/bindings/ libplayertcp/bindings
% note: libplayerxdr is autogenerated
% COMP: sloccount server 
% UTIL: sloccount utils PLUS stage/src
2006    2  28   1  0    63992-47749-1291        54704       0     % 2.0.0
2006    3  25   1  0    65606-48912-1291        53033       0     % 2.0.1
2006    6   9   1  0    69083-51546-1319        61421       0     % 2.0.2
2006    9  26   1  0    69388-51546-1319        64088       13606       % 2.0.3
2007    5   2   1  0    69602-51686-1359        58396       9589+13606  % 2.0.4
2007   10  26   1  0    23526-109               78141       11404+13606 % 2.0.4+
];
Tplayer = datenum(Splayer(:,1),Splayer(:,2),Splayer(:,3));

Saria = [
% YY    MM  DD  ? SUB   BASE    COMP    UTIL    VERSION
% SUB : 0
% BASE: sloccount ArNetworking/include/ ArNetworking/src/
% COMP: sloccount include src
% UTIL: 0 
2007    2   20  1  0    15854   45965   0       % 2.5.1
];

Scarmen = [
% YY    MM  DD  ?   SUB     BASE    COMP                    UTIL   VERSION
% SUB : cd src; sloccount ipc
% BASE: sloccount carmenpp global java python
% COMP: sloccount src MINUS all other
% UTIL: sloccount logger logtools mapeditor param_daemon robotgui simulator 
2007    8   15  1  24965    12032   107745-24965-12032-19629  19629   % 0.6.7
];

Sorocos = [
% YY    MM  DD  ?   SUB     BASE    COMP                        UTIL   VERSION
% SUB : cd ACE_wrappers; sloccount ace PLUS sloccount TAO/tao/
% BASE: ?
% LIBS: sloccount orocos-bfl-[VERSION]-src/src PLUS sloccount kdl-[VERSION]/kdl
% COMP: sloccount orocos-rtt-[VERSION]/src PLUS cd orocos-ocl-[VERSION]; sloccount hardware motion_control
% UTIL: cd orocos-ocl-[VERSION]; sloccount taskbrowser signals deployment reporting viewer ocl 
2007    8   15  1  160300+237011   0   (7767+12865)+(48971+20068)    5104   % ace-5.5, tao-1.5, rtt-1.2.1, bfl-0.6.1, kdl-0.4, ocl-0.4.1
];

Syarp = [
% YY    MM  DD  ?   SUB  BASE     COMP       UTIL   VERSION
% SUB : 0
% BASE: sloccount libYARP_OS libYARP_OS_compat
% COMP: sloccount yarpdev libYARP_dev libYARP_math libYARP_sig libYARP_sig_compat 
% UTIL: sloccount yarp-imageviewer modules
2007    7   26  1  0     21633   23528    1613   % 2.1.6
];

% YY    MM  DD  ?   SUB  BASE     COMP       UTIL
% SUB : 0
% BASE: 0
% COMP: sloccount . (matlab files will not be counted)
% UTIL: 0
Sopenslam = [
2007    8   16  1   0    0        23719      0
];

%%%%%%%%%% PLOTTING %%%%%%%%%%%%%%%%%%%%

Smax = 180;
d0 = [Torca Torca+1]'; d0=d0(:); d0(1)=[];
d1 = Smax-Sorca(:,isub)-Sorca(:,ibase); d1 = [d1 d1]'; d1=d1(:); d1(end)=[];
d2 = Sorca(:,isub); d2 = [d2 d2]'; d2=d2(:); d2(end)=[];
d3 = Sorca(:,ibase); d3 = [d3 d3]'; d3=d3(:); d3(end)=[];
d4 = Sorca(:,icomp); d4 = [d4 d4]'; d4=d4(:); d4(end)=[];
d5 = Sorca(:,iutil); d5 = [d5 d5]'; d5=d5(:); d5(end)=[];
d6 = Sorca(:,iad); d6 = [d6 d6]'; d6=d6(:); d6(end)=[];

figure(1), clf
ha=area( d0, [d1 d2 d3 d5 d4 d6]/1e3, -Smax ); hold on
plot( Torca, 0*Torca, 'k-o' )
h2=plot( Torca2, 0*Torca2, 'ko' );
set(h2,'markerfacecolor','k')
ylabel('Source Code Size (kSLOC)')

% colors
white = 1*[1 1 1];
gray = .85*[1 1 1];
red = min(255,[255 0 0]+60)/255;
orange = min(255,[255 153 0]+60)/255;
green = min(255,[60 255 109]+60)/255;
blue = min(255,[51 102 255]+60)/255;

set(ha(1),'facecolor', white);
set(ha(2),'facecolor', gray);
set(ha(3),'facecolor', red);
set(ha(4),'facecolor', green);
set(ha(5),'facecolor', orange);
set(ha(6),'facecolor', blue);
ha(1)=[];
legend(fliplr(ha),'Hydro','Orca components+libs','Orca utilities','Orca infrastructure','Ice middleware', 'Location','NorthWest')
%  xlim(datenum([2004;2005],[7;7],[1;1]) )
ylim([-Smax 100])
ax=axis;
xlim( [ax(1) datenum([2006 12 31])] )
ax=axis;
% label releases
mark('x', Torca2 )
i2odd = (1:2:length(Torca2));
ht = text(Torca2(i2odd)+5,.04*ax(4)*ones(length(i2odd),1),Vorca2(i2odd));
set(ht,'fontsize',8)
i2even = (2:2:length(Torca2));
ht = text(Torca2(i2even)+5,.08*ax(4)*ones(length(i2even),1),Vorca2(i2even));
set(ht,'fontsize',8)
datetick( 'x', 12 )
hp = plot( Tplayer, -Splayer(:,ibase)/1e3, 'k--o', Tplayer, Splayer(:,icomp)/1e3, 'k--o' );
set(hp, 'color', .5*[1 1 1] )
hpt = text( Tplayer(2), -Splayer(2,ibase)/1e3-8, 'Player infrastructure');
set(hpt, 'color', .5*[1 1 1] )
hpt = text( Tplayer(2), (Splayer(2,icomp)+Splayer(2,iutil))/1e3-5, 'Player "components+utils"');
set(hpt, 'color', .5*[1 1 1] )
%mark('y', Dasn(1,1)/1e3 );
%text( datenum([2003 5 1]), Dasn(1,1)/1e3, 'ASN')
xlabel('Releases')
title('http://orca-robotics.sf.net')

ht=text(datenum(2003,7,1),-Smax+15, '*   generated using David A. Wheeler`s SLOCCount' );
set(ht,'fontsize',8)
ht=text(datenum(2003,7,1),-Smax+5, '** see data in [ORCA2]/scripts/matlab/sloccount/slocs.m' );
set(ht,'fontsize',8)
