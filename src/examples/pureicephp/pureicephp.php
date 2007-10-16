<?php

//Author Duncan Mercer.
//This script is designed to test that the connection to a laser
//scanner component can be made. Follow the quickstart examples
//from the orca website to start a laserscanner and lasermon
//component.

//Edit the proxy string to suit your location
$ProxyStr = "laserscanner2d@tapti/laser2d";
?>

<HTML><HEAD>
<META HTTP-EQUIV="Refresh" CONTENT="5;">
<TITLE>Test</TITLE>
</HEAD>

<BODY>

<H1>Laser2d test PHP script</H1>
A script that connects to a laser2d component at Tapti<br>
It should refresh the <B>scan data</B> every 5 secs<br>&nbsp<br>

<?php

global $ICE;
Ice_loadProfile();

try
{
//Setup the description of the object that we are going to connect to.
    $base = $ICE->stringToProxy($ProxyStr);
    echo "Connect to: ";
    print $ICE->stringToProxy($ProxyStr);
    
    
//Connect to the object, and get some basic data
    $laser = $base->ice_checkedCast("::orca::LaserScanner2d");
    $data = $laser -> getDescription();
    $TimeStamp = $data->timeStamp;
    echo "<H2>Description Data</H2>";   
    echo "Time object initialised: " . $TimeStamp -> seconds ." <br>\n";
    echo "MaxRange(m): " . $data->maxRange . "<br>\n";
    echo "FieldOfView(Rad): " . $data->fieldOfView . "<br>\n";
    echo "ScanStart(Rad): " . $data->startAngle . "<br>\n";
    echo "Num of Samples: " . $data->numberOfSamples . "\n";

//Get a laser scan and print some info.
    $data = $laser -> getData();
    $TimeStamp = $data->timeStamp;
    $TimeStampArray = localtime($TimeStamp -> seconds, false);
    
    echo "<H2>Scan Data</H2>";   
    printf ("Time of scan, %02d:%02d:%02d<br>&nbsp<br>\n",$TimeStampArray[2],$TimeStampArray[1],$TimeStampArray[0]);

    $ranges = $data->ranges;
    foreach($ranges as $range){
        echo "$range, ";
    }
}   


catch(Ice_LocalExeception $ex)
{
    print_r($ex);
}

?>

</BODY>
</HTML>