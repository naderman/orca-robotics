#!/usr/bin/perl 

use warnings;
use strict;

my ($TimeStamp,$CommandSpeed,@canData,$ReceivedSpeed,$ActualSpeed);
my $LastT413 = 0;
my $LastT407 = 0;
my $LastT402 = 0;
my $checkSpeeds = 0;
$ReceivedSpeed = $CommandSpeed  =0;

while(<>){


    my $line = $_;
    @canData = split;
    
    # These are the speed command messages...
    if($canData[2] eq "0x00000413"){
        $TimeStamp = $canData[12];
        print "Delta Time 0x413 " .($TimeStamp - $LastT413) ." millisec. ";
        $LastT413 = $TimeStamp;
        $CommandSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        print "Cmd speed $CommandSpeed ";
        print $line;
        $checkSpeeds = 1;
    }


    # These are the speed messages echoed back by the RMP basae
    elsif($canData[2] eq "0x00000407"){
        $TimeStamp = $canData[12];
        print "Delta Time 0x407 " .($TimeStamp - $LastT407) ." millisec. ";
        $ReceivedSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        print "Req speed $ReceivedSpeed ";
        $LastT407 = $TimeStamp;
        print $line;
        $checkSpeeds = 1;
    }

    #These are the 'actual' wheel velocities
    elsif($canData[2] eq "0x00000402"){
        $TimeStamp = $canData[12];
        print "Delta Time 0x402 " .($TimeStamp - $LastT402) ." millisec. ";
        $ActualSpeed = (getnum($canData[4]) << 8)  + getnum($canData[5]);
        $ActualSpeed += (getnum($canData[6]) << 8)  + getnum($canData[7]);
        $ActualSpeed /= 2;
        print "Act speed $ActualSpeed ";
        $LastT402 = $TimeStamp;
        print $line;
    }

    if(($ReceivedSpeed != $CommandSpeed) && $checkSpeeds){
        print STDERR "Error received ($ReceivedSpeed) and command";
        print STDERR "($CommandSpeed) speeds are different. Time since last CmdMessage " .($TimeStamp - $LastT413). " ms \n";
        $checkSpeeds = 0;
    }

    
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
