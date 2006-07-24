// import Ice.*;
// import orca.*;

public class PureIceJ {

    public static void
    main(String[] args)
    {
        int status = 0;
        Ice.Communicator ic = null;
        try {
            // Create a communicator
            //
            ic = Ice.Util.initialize(args);
    
            // Create a proxy for the laser
            //
            // we'll use indirect proxy here to connect to the sicklaser component
            // setup according to the Quick Start tutorial of Orca2
            // see: orca-robotics.sf.net/orca_doc_quickstart.html
    //         Ice.ObjectPrx base = ic.stringToProxy( "laser@"+getHostname()+"/sicklaser" );
            System.out.println("*****BIG HACK: hostname is hardwired!");
            Ice.ObjectPrx base = ic.stringToProxy( "laser@"+"agave"+"/sicklaser" );
            System.out.println("Base proxy created...");
    
            // Down-cast the proxy to a Laser proxy
            //
            orca.LaserPrx laser = orca.LaserPrxHelper.checkedCast(base);
            if (laser == null) {
                throw new RuntimeException("Invalid proxy");
            }
            System.out.println("Derived proxy created...");
    
            // get one laser scan and print it out
            //
            orca.RangeScannerData data = new orca.LaserData();
            data = laser.getData();
            System.out.println("Received laser scan:");
            System.out.println(" RangeScannerData: ["+data.ranges.length+" elements]: ");
            System.out.println("    startAngle     : "+data.startAngle*180.0/3.14159265358979+"deg");
            System.out.println("    angleIncrement : "+data.angleIncrement*180.0/3.14159265358979+"deg");
        } 
        catch (Ice.LocalException e) {
            e.printStackTrace();
            status = 1;
        } 
        catch (Exception e) {
            System.err.println(e.getMessage());
            status = 1;
        }

        if (ic != null) {
            // Clean up
            //
            try {
            ic.destroy();
            } catch (Exception e) {
            System.err.println(e.getMessage());
            status = 1;
            }
        }
        System.exit(status);
    }
}
