/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _NMEA_H_
#define _NMEA_H_

/*

for further info:

http://www.kh-gps.de/nmea-faq.htm
http://vancouver-webpages.com/peter/nmeafaq.txt

NMEA-0183 sentence

- up to 82 characters including the "$" and CR/LF.

$aaccc,c--c*hh<CR><LF>
||    ||   || |
||    ||   || \________ <CR><LF> - End of sentence (0xOD 0xOA)
||    ||   |\__________ hh    - Checksum field hexadecimal
||    ||   \___________ *     - Checksum delimiter (0x2A)
||    |\_______________ c--c  - Data sentence block
||    \________________ ,     - Field delimiter (0x2c)
|\_____________________ aaccc - Address field/Command
\______________________ $     - Start of sentence

        The optional checksum field consists of a "*" and two hex digits
        representing the exclusive OR of all characters between, but not
        including, the "$" and "*".  A checksum is required on some
        sentences.

*/

namespace orcagpsutil{
	extern const char StartOfSentence;
	extern const unsigned int CommandLen;
	extern const char FieldDelim;
	extern const char ChecksumDelim;
	extern const unsigned int ChecksumFieldLen;
	extern const char *EndofSentence;
	extern const unsigned int MaxSentenceLen;
	extern const unsigned int MaxDataLen;
	extern const int MaxNumFields;

    class NmeaMessage{
        friend class NmeaParser;
    public:
        NmeaMessage();
	NmeaMessage(const char *sentence);
	NmeaMessage(const char *command, const char *data);
	NmeaMessage(const char *command, int fields, ...);
	~NmeaMessage();

        //! Do we only have the raw string ?
	bool haveSentence(){return haveSentence_;};
        //! Set up the internal data for a sentence
	bool setSentence(const char *data);
        //! Is it partially parsed ?
	bool haveData(){return haveData_;};
        //! Have we parsed fields ?
	bool haveFields(){return haveFields_;};
	//! have we a checksum ?
	bool haveChecksum(){return haveChecksum_;};
        //! Return the checksum
	int checksum();
        //! set the checksum
	void setChecksum(int checksum){checksum_=checksum;};
	//! calculate the checksum from sentence
        int calcChecksum();
        //! Return the raw sentence string
	const char *sentence(){return sentence_;};
        //! Return the command string
	const char *command(){return command_;};
        //! Return the string forming data in the sentence
	const char *data(){return data_;};
        //! Return the length of data
	int dataLen(){return dataLen_;};
        //! Return a single data field string
	const char *dataField(int i);
        //! Return the number of fields
	int numDataFields(){return numDataFields_;};
    private:
        void init();
        //! Do we only have the raw string ?
	bool haveSentence_;
        //! Is it partially parsed ?
	bool haveData_;
        //! Have we parsed fields ?
	bool haveFields_;
	//! Have we a checksum ?
        bool haveChecksum_;
        //! The raw sentence
	char *sentence_;
        //! The command as a string
	char *command_;
        //! The raw data string
	char *data_;
        //! The length of the data string
	int dataLen_;
	//! The fields
	char **dataFieldsPtr_;
        char *dataFields_;
        //! The number of fields
	int numDataFields_;
        //! The checksum
	int checksum_;
    };

    class NmeaParser{
    public:
	NmeaParser();
	~NmeaParser();
	/*! parse a message but leave data intact
	    returns -1 if Checksum fails or otherwise fscked up
         */
	int parseMessage(NmeaMessage &message);
        //! parse data contained within an already parsed message
	int parseData(NmeaMessage &message);

    protected:
    private:

    };
}


#endif
