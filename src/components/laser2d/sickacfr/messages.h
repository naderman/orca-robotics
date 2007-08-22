/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SICK_ACFR_DRIVER_MESSAGES_H
#define SICK_ACFR_DRIVER_MESSAGES_H

#include <sickacfr/sickdefines.h>
#include <string>
#include <vector>

namespace sickacfr {

    std::string toHexString( const uChar *buf, int bufLen );
    inline std::string toHexString( const std::vector<uChar> &buf )
    {return toHexString( &(buf[0]), buf.size() );}

    //////////////////////////////////////////////////////////////////////
    // Generic LMS Response classes:
    // 
    // - The SICK replies to commands with responses.
    // - Responses are of a standard format, but some responses contain
    //   extra response-type-specific data.
    // - This is handled with the abstract class 'LmsResponseData':
    //   - The LmsResponse has a pointer to LmsResponseData, which should
    //     be cast to the appropriate type depending on the response type.
    //
    // (Note that in continuous mode, the measurements continuously
    //  sent out by the SICK are 'responses', even though there's no command).
    // 
    //////////////////////////////////////////////////////////////////////

    // Abstract class for representing response-type-specific data.
    class LmsResponseData {
    public:
        virtual ~LmsResponseData() {}

        // human-readable string
        virtual std::string toString() const=0;

        virtual bool isError() const { return false; }

        // Returns a freshly allocated object of the same type
        virtual LmsResponseData *clone() const=0;
    };

    // This class represents responses which the SICK uses to reply to commands.
    // All response types have the information in this class.
    // Some responses also have extra data, which is stored in the 'data' member.
    class LmsResponse {
    public:
        LmsResponse()
            : status(0),
              data(NULL)
            {}
        LmsResponse( const LmsResponse &other );
        LmsResponse &operator=( const LmsResponse &other );
        ~LmsResponse()
            { if (data) delete data; }

        uChar type;
        uChar status;
        LmsResponseData *data;

        bool isError() const;
    };
    std::string toString( const LmsResponse &r );

    //////////////////////////////////////////////////////////////////////
    // Response-specific data classes
    //
    // - The set of classes below all inherit from the abstract
    //   'LmsResponseData' class.  They represent the data contained
    //   in specific response types.
    // - See 'parseResponse' in the .cpp file for details of which 
    //   classes go with which response codes.
    // 
    //////////////////////////////////////////////////////////////////////

    class LmsInitResponseData : public LmsResponseData {
    public:
        std::string description;

        std::string toString() const { return description; }
        LmsResponseData *clone() const { return new LmsInitResponseData(*this); }        
    };

    class LmsStatusResponseData : public LmsResponseData {
    public:
        std::string version;
        uChar       operatingMode;
        uChar       status;
        std::string manufacturer;
        uChar       variantType;
        uint16_t    pollution[POLLUTION_LENGTH];
        uint16_t    refPollution[REF_POLLUTION_LENGTH];
        uint16_t    calibPollution[CALIB_POLLUTION_LENGTH];
        uint16_t    calibRefPollution[CALIB_REF_POLLUTION_LENGTH];
        uint16_t    numMotorRevolutions;
        uint16_t    refScale1Dark100Pct;
        uint16_t    refScale2Dark100Pct;
        uint16_t    refScale1Dark66Pct;
        uint16_t    refScale2Dark66Pct;
        uint16_t    signalAmplitudePct;
        uint16_t    currentAngle;
        uint16_t    peakThreshold;
        uint16_t    angleOfMeasurement;
        uint16_t    calibSignalAmplitude;
        uint16_t    targetStopThreshold;
        uint16_t    targetPeakThreshold;
        uint16_t    actualStopThreshold;
        uint16_t    actualPeakThreshold;
        uChar       measuringMode;
        uint16_t    refSingleMeasuredValues;
        uint16_t    refMeanMeasuredValues;
        uint16_t    scanningAngle;
        uint16_t    angularResolution;
        uChar       restartMode;
        uChar       restartTime;
        uint16_t    baudRate;
        uChar       evaluationNumber;
        uChar       permanentBaudRate;
        uChar       lmsAddress;
        uChar       fieldSetNumber;
        uChar       currentMeasuredValueUnit;
        uChar       laserSwitchOff;
        std::string softwareVersion;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsStatusResponseData(*this); }
    };

    class LmsSwitchOperatingModeResponseData : public LmsResponseData {
    public:
        uChar success;

        bool isError() const { return success != OPERATING_MODE_RESPONSE_SUCCESS; }
        std::string toString() const { return modeSwitchSuccessToString(success); }
        LmsResponseData *clone() const { return new LmsSwitchOperatingModeResponseData(*this); }
    };

    class LmsConfigurationData : public LmsResponseData {
    public:

        // Default values for all these fuckers
        LmsConfigurationData();
        bool operator==( const LmsConfigurationData &o ) const;
        bool operator!=( const LmsConfigurationData &o ) const
            { return !(operator==(o)); }

        uint16_t blanking;
        uChar    sensitivity;
        uChar    availability;
        uChar    measuringMode;
        uChar    measuredValueUnit;
        uChar    transientFieldSet;
        uChar    subtractiveFields;   // 14
        uChar    multipleEvaluation;
        uChar    restart;
        uChar    restartTime;
        uChar    multipleEvaluationForSuppressed;
        uChar    contourARef;
        uChar    contourAPosToleranceBand; // 20
        uChar    contourANegToleranceBand;
        uChar    contourAStartAngle;
        uChar    contourAStopAngle;
        uChar    contourBRef;
        uChar    contourBPosToleranceBand;
        uChar    contourBNegToleranceBand;
        uChar    contourBStartAngle;
        uChar    contourBStopAngle;
        uChar    contourARef2;
        uChar    contourAPosToleranceBand2; // 30
        uChar    contourCNegToleranceBand;
        uChar    contourCStartAngle;
        uChar    contourCStopAngle;
        uChar    pixelOrientedEvaluation;
        uChar    singleMeasuredValueEvaluation;
        uint16_t restartTimeFields;
        uint16_t multipleEvaluationForDazzle;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsConfigurationData(*this); }
    };

    class LmsConfigurationResponseData : public LmsResponseData {
    public:
        LmsConfigurationData config;
        uChar                configSuccess;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsConfigurationResponseData(*this); }        
        bool isError() const { return configSuccess != CONFIGURATION_SUCCESS; }
    };

    class LmsSwitchVariantResponseData : public LmsResponseData {
    public:
        uChar            success;
        uint16_t         scanningAngle;
        uint16_t         angularResolution;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsSwitchVariantResponseData(*this); }        
        bool isError() const { return success != SWITCH_VARIANT_SUCCESS; }
    };

    class LmsMeasurementData : public LmsResponseData {
    public:
        
        // ranges in metres
        std::vector<float> ranges;
        std::vector<uChar> intensities;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsMeasurementData(*this); }        
    };

    class LmsErrorResponseData : public LmsResponseData {
    public:
        
        // ranges in metres
        std::vector<uChar> errorTypes;
        std::vector<uChar> errorCodes;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsErrorResponseData(*this); }        
        bool isError() const;
    };

    class LmsOperatingDataCounterData : public LmsResponseData {
    public:
        
        int hoursOfOperation;
        int numSwitchOns;

        std::string toString() const;
        LmsResponseData *clone() const { return new LmsOperatingDataCounterData(*this); }        
    };

    ////////////////////////////////////////////////////////////////////////////////

    // Returns true if a complete telegram was found
    bool parseBuffer( const uChar *buffer,
                      int bufferLength,
                      LmsResponse &response,
                      int &bytesParsed );

    void constructTelegram( std::vector<uChar>       &buffer,
                            const std::vector<uChar> &commandAndData );

    // SICK parameters can be changed in installation mode.
    void constructRequestInstallationMode( std::vector<uChar> &commandAndData );

    void constructRequestContinuousMode( std::vector<uChar> &commandAndData );
    void constructRequestMeasuredOnRequestMode( std::vector<uChar> &commandAndData );

    void constructInitAndReset( std::vector<uChar> &commandAndData );

    void constructStatusRequest( std::vector<uChar> &commandAndData );

    void constructConfigurationRequest( std::vector<uChar> &commandAndData );

    void constructConfigurationCommand( const LmsConfigurationData &c,
                                        std::vector<uChar> &commandAndData );

    void constructRequestErrorMessage( std::vector<uChar> &commandAndData );

    void constructSwitchVariant( uint16_t scanningAngle,
                                 uint16_t angularResolution,
                                 std::vector<uChar> &commandAndData );

    void constructRequestOperatingDataCounter( std::vector<uChar> &commandAndData );
        
    void constructRequestBaudRate( std::vector<uChar> &commandAndData, int baudRate );
}

#endif
