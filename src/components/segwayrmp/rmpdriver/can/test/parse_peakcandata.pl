#!/usr/bin/perl 


# A rough and ready perl script used to parse the peak CAN data from the segway
# It is expecting data in format that the command 'cat /dev/pcan40 | parse_peakcandata.pl'
# will produce.

use warnings;
use strict;

my ($TimeStamp,$CommandSpeed,@canData,$ReceivedSpeed,$ActualSpeed,$UiVoltage,$CUAVoltage,$CUBVoltage);
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

    elsif($canData[2] eq "0x00000406"){
        my $CuVoltage = ((getnum($canData[10]) << 8)  + getnum($canData[11])) / 4;
        print "CU_Voltage $CuVoltage\n";
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
        my $UiVoltage = 0x03FF & (getnum($canData[8]) << 8)  + getnum($canData[9]);
        $UiVoltage = 1.4 + ($UiVoltage * 0.0125);
        my $UiBatFlags = getnum($canData[8]) & 0xC0;
        print "New 688: UiVoltage $UiVoltage, UiFlags $UiBatFlags, $Last688Msg";
    }elsif($canData[2] eq "0x00000680"){
        $Last680Msg = $line;
        print "New 680 $Last680Msg";
        parse680 (@canData);
    }elsif($canData[2] eq "0x00000412"){
        $Last412Msg = $line;
        print "New 412 $Last412Msg";
    }else{
        print "\n";
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



#**********************************************************************************
#Function to parse the status flags in the #0x680 message
sub parse680{

    my @data = shift;

    my $Byte2 = getnum($canData[6]);
    my $Byte3 = getnum($canData[7]);
    my $Word1 = ($Byte2 << 8) + $Byte3;
    
    my $Byte4 = getnum($canData[8]);
    my $Byte5 = getnum($canData[9]);
    my $Word2 = ($Byte4 << 8) + $Byte5;


    print "Msg 0x680 Flags set:- ";
    if($Word1 & 0x0001){print "SafteyShutdown1 ";}
    if($Word1 & 0x0002){print "NotUsed1 ";}
    if($Word1 & 0x0004){print "SafteyShutdown2 ";}
    if($Word1 & 0x0008){print "DisableResponse ";}
    if($Word1 & 0x0010){print "SpeedLimitTo4MPH ";}
    if($Word1 & 0x0020){print "SpeedLimitTo0MPH ";}
    if($Word1 & 0x0040){print "SystemIntegrityFault ";}
    if($Word1 & 0x0080){print "MotorsEnabled ";}
    if($Word1 & 0x0100){print "BalanceMode ";}
    if($Word1 & 0x0200){print "YawTransientFlag ";}
    if($Word1 & 0x0400){print "EmptyBatteryHazard ";}
    if($Word1 & 0x0800){print "NotUsed2 ";}
    if($Word1 & 0x1000){print "WheelSlipFlag ";}
    if($Word1 & 0x2000){print "MotorHazardFault ";}
    if($Word1 & 0x4000){print "LowBatteryFault ";}
    if($Word1 & 0x8000){print "NotUsed3 ";}
    print "\n\n";
        
}
