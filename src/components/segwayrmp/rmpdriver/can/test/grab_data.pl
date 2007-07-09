#!/usr/bin/perl 

use warnings;
use strict;

my ($TimeStamp,$CommandSpeed,@canData,$ReceivedSpeed,$ActualSpeed);
my $LastT413 = 0;
my $LastT407 = 0;
my $LastT402 = 0;
my $checkSpeeds = 0;
my $LastCmdSpeed = -1;
my $TimeOfLastErr= -1;
my $Last688Msg = '';
my $Last412Msg = '';
my $Last680Msg = '';
my $LastMsgId = '';
my $LastMsgTimestamp=0;
my %MessageList;
my $Max413Timeout=0;
my $linecount = 0;



$ReceivedSpeed = $CommandSpeed  =0;

while(<>){


    my $line = $_;
    @canData = split;

    $TimeStamp = $canData[12];
    
    print "linenum " . $linecount++ . " ";

    # These are the speed command messages...
    if($canData[2] eq "0x00000413"){
        
        print "LastMsg $LastMsgId; " . ($TimeStamp - $LastMsgTimestamp) . "ms ago\n";

        my $TimeDiff = ($TimeStamp - $LastT413);
        print "MaxT413 $Max413Timeout, Delta Time 0x413 " . $TimeDiff ." millisec. ";
        $CommandSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        print "Cmd speed $CommandSpeed ";
        print $line;

        if($LastCmdSpeed != $CommandSpeed){
            if($LastCmdSpeed != -1){
                print "WARNING:- Last CMD speed ($LastCmdSpeed) and current ($CommandSpeed) CMD speed different!\n"
            }
        }

        $checkSpeeds = 1;
        $LastCmdSpeed = $CommandSpeed;

        if(($TimeDiff > $Max413Timeout)&($LastT413 != 0)){
            $Max413Timeout = $TimeDiff;
        }

        $LastT413 = $TimeStamp;

    }


    # These are the speed messages echoed back by the RMP basae
    elsif($canData[2] eq "0x00000407"){
        print "Delta Time 0x407 " .($TimeStamp - $LastT407) ." millisec. ";
        $ReceivedSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        print "Req speed $ReceivedSpeed ";
        $LastT407 = $TimeStamp;
        print $line;
        $checkSpeeds = 1;
    }

    #These are the 'actual' wheel velocities
    elsif($canData[2] eq "0x00000402"){
        print "Delta Time 0x402 " .($TimeStamp - $LastT402) ." millisec. ";
        $ActualSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        $ActualSpeed += (getnum($canData[6]) << 8)  + getnum($canData[7]);
        $ActualSpeed /= 2;
        print "Act speed $ActualSpeed ";
        $LastT402 = $TimeStamp;
        print $line;
    }elsif($canData[2] eq "0x00000688"){
        $Last688Msg = $line;
        print "New 688 $Last688Msg";
    }elsif($canData[2] eq "0x00000680"){
        $Last680Msg = $line;
        print "New 680 $Last680Msg";
    }elsif($canData[2] eq "0x00000412"){
        $Last412Msg = $line;
        print "New 412 $Last412Msg";
    }

    if(($ReceivedSpeed != $CommandSpeed) && $checkSpeeds){
        print  "Err Rx ($ReceivedSpeed) and Cmd";
        print  "($CommandSpeed) speeds differ. Now ($TimeStamp ms). Time since last CmdMessage " .($TimeStamp - $LastT413). " ms \n";
        print STDERR "Err Rx ($ReceivedSpeed) and Cmd";
        print STDERR "($CommandSpeed) speeds differ. Now ($TimeStamp ms). Time since last CmdMessage " .($TimeStamp - $LastT413). " ms \n";
        $checkSpeeds = 0;

        if($TimeOfLastErr != -1){
            print  "Time since last error. ". ($TimeStamp - $TimeOfLastErr) ."ms \n";
            print STDERR "Time since last error. ". ($TimeStamp - $TimeOfLastErr) ."ms \n";
        }
        $TimeOfLastErr = $TimeStamp;


        if($Last412Msg ne ''){print STDERR "Last 412 $Last412Msg";}
        if($Last680Msg ne ''){print STDERR "Last 680 $Last680Msg";}
        if($Last688Msg ne ''){print STDERR "Last 688 $Last688Msg";}
        print STDERR "\n";
 
        if($Last412Msg ne ''){print  "Last 412 $Last412Msg";}
        if($Last680Msg ne ''){print  "Last 680 $Last680Msg";}
        if($Last688Msg ne ''){print  "Last 688 $Last688Msg";}
        print "\n";
    }

    $MessageList{$canData[2]}++;
    $LastMsgId = $canData[2];
    $LastMsgTimestamp = $TimeStamp;
    
}

my $key;

foreach $key (sort keys (%MessageList)){
    print "Msg $key => $MessageList{$key}\n";
}

#**************************************************************************************

sub getnum {
    use POSIX qw(strtod);
    my $str = shift;
    $str =~ s/^\s+//;
    $str =~ s/\s+$//;
    $! = 0;
    my($num, $unparsed) = strtod($str);
    if (($str eq '') || ($unparsed != 0) || $!) {
        return;
    } else {
        return $num;
    } 
} 
