/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007  Alex Brooks, Alexei Makarenko, Tobias Kaupp, Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <vector>
#include <string>

#ifndef _NMEA_H_
#define _NMEA_H_



/*

for further info:

http://www.kh-gps.de/nmea-faq.htm
http://vancouver-webpages.com/peter/nmeafaq.txt

NMEA-0183 sentence

$aaccc,c--c*hh<CR><LF>
||    ||   || |
||    ||   || \________ <CR><LF> - End of sentence (0xOD 0xOA)
||    ||   |\__________ hh    - Checksum field hexadecimal [optional]
||    ||   \___________ *     - Checksum delimiter (0x2A) [optional]
||    |\_______________ c--c  - Data sentence block
||    \________________ ,     - Field delimiter (0x2c)
|\_____________________ aaccc - Address field/Command
\______________________ $     - Start of sentence

        The optional checksum field consists of a "*" and two hex digits
        representing the exclusive OR of all characters between, but not
        including, the "$" and "*".  A checksum is required on some
        sentences.

*/

using namespace std;

namespace orcagpsutil{


class NmeaException : public std::exception
{
public:

    NmeaException(const char *message)
        : message_(message) {}
    NmeaException(const std::string &message)
        : message_(message) {}
    virtual ~NmeaException() throw() {}
    virtual const char* what() const throw() { return message_.c_str(); }

protected:
    std::string  message_;
};


#define MAX_SENTENCE_LEN 256

//! When using class to send data, need to add checksum, when reciving data need to test checksum
//! Checksums are usually optional 
    enum{TestChecksum, AddChecksum, DontTestOrAddChecksum};

    class NmeaMessage{
    public:
        NmeaMessage();
        NmeaMessage(const char *sentence, int testCheckSum = DontTestOrAddChecksum);
        ~NmeaMessage();

        //! Do we only have the raw string ?
        bool haveSentence(){return haveSentence_;};
        //! Set up the internal data for a sentence
        void setSentence(const char *data, int testCheckSum = DontTestOrAddChecksum);
         //! Have we parsed fields ?
        bool haveTokens(){return haveTokens_;};
        //! have we a valid checksum ?
        bool haveValidChecksum(){return checkSumOK_;};    
        //! have we checked the checksum?
        bool haveTestedChecksum(){return haveCheckSum_;};    
        //! calculate the checksum from sentence
        //! Note that this function may throw NMEA_Exception...
        bool testChecksumOk();
        //! Return the raw sentence string
        const char * sentence(){return sentence_;};
        //! Return a single data token as a string
        string& getDataToken(int i){return dataTokens_[i];};

        //! Return the number of fields
        int numDataTokens(){return dataTokens_.size();};
        //Tokenise the string that we received
        void parseTokens();

    private:
        void init();
        void addCheckSum();
        //! Do we only have the raw string ?
        bool haveSentence_;
        //! Have we parsed data into tokens ?
        bool haveTokens_;
        //! Have we a checksum and is it valid?
        bool haveCheckSum_;
        bool checkSumOK_;
        //! The raw sentence, allow for terminator
        char sentence_[MAX_SENTENCE_LEN+1];
        //! The tokenised data
        vector<string> dataTokens_;
        
    };

}

#endif
