class gbxserialacfr::Serial;

// test connectivity to a [serial] device at a [baudrate];
// Assumes that you can figure out a [challenge] (command ...) to
// which the device will answer with a unique [ack] in [timeOutMsec] milliseconds.
// If [successThresh] or more [challenge]es are answered by an [ack]
//
// Limitations: amount of data expected before timeOutMsec should be
// reasonably small
//
// returns 0 for Success; -1 for failure
bool testConnectivity(
        std::string &challenge,
        std::string &ack,
        gbxserialacfr::Serial* serial,
        int timeOutMsec,
        int numTry,
        int successThresh,
        int baudrate);
