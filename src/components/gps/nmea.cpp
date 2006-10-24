/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <stdio.h>
#include <string.h>
#include <iostream>

// Make sure strnlen is defined.
#include <orcaportability/strnlen.h>

#include "nmea.h"

using namespace std;

namespace nmea{

    const char StartOfSentence = '$';
    const unsigned int CommandLen = 5;
    const char FieldDelim = ',';
    const char ChecksumDelim = '*';
    const unsigned int ChecksumFieldLen = 2;
    const char *EndofSentence = "\r\n";
    // wrong wrong wrong, should be 82!
    const unsigned int MaxSentenceLen = 256;
    const unsigned int MaxDataLen = MaxSentenceLen - 7;
    const int MaxNumFields = 128;


    NmeaMessage::NmeaMessage()
    {
	init();
    }

    void NmeaMessage::init()
    {
	haveSentence_=false;
	haveData_=false;
	haveFields_=false;
        haveChecksum_=false;
	checksum_=-1;

        //! The raw sentence
	sentence_ = new char[MaxSentenceLen+1];
        //! The command as a string
	command_ = new char[CommandLen+1];
        //! The raw data string
	data_ = new char[MaxDataLen+1];
	//! The fields
	dataFieldsPtr_ = new char*[MaxNumFields];
        dataFields_ = new char[MaxSentenceLen];


        sentence_[0]=0;
	command_[0]=0;
	data_[0]=0;
	dataLen_=0;
        dataFieldsPtr_[0]=0;
        dataFields_[0]=0;
        numDataFields_=0;
    }

    NmeaMessage::NmeaMessage(const char *sentence)
    {
	init();
	strncpy(sentence_,sentence,MaxSentenceLen);
        // terminate just in case
	sentence_[MaxSentenceLen]='\0';
        haveSentence_=true;
    }

    NmeaMessage::NmeaMessage(const char *command, const char *data)
    {
	init();
    }

    NmeaMessage::NmeaMessage(const char *command, int fields, ...)
    {
	init();
    }

    NmeaMessage::~NmeaMessage()
    {
    }

    const char *NmeaMessage::dataField(int i)
    {
	if(i>=numDataFields_||i<0||!haveFields_){
            return NULL;
	}else{
            return dataFieldsPtr_[i];
	}
    }

    bool NmeaMessage::setSentence(const char *data)
    {
	strncpy(sentence_,data,MaxSentenceLen);
        // terminate just in case
	sentence_[MaxSentenceLen]='\0';
	haveSentence_=true;
        // reset other fields
	haveData_=false;
	haveFields_=false;
        haveChecksum_=false;
	checksum_=-1;
        return true;
    }

    int NmeaMessage::calcChecksum()
    {
	if(haveSentence_){
            if(sentence_[0]!=StartOfSentence){
                return -1;
	    }
            unsigned char chk=0;

	    for(unsigned int i=1;i<MaxSentenceLen;i++){
                // no delimiter uh oh
		if((sentence_[i]=='\r')||(sentence_[i]=='\n')||(sentence_[i]=='\0')){
		    cerr << "nmea: cannot calculate checksum, missing *\n";
		    return -1;
		}
                // goodie we found it
		if(sentence_[i]==ChecksumDelim)
                    break;
		chk^=(unsigned char)sentence_[i];
	    }
            return chk;
	}
	else{
            return -1;
	}
    }

    //! Return the checksum
    int NmeaMessage::checksum(){
        if(haveChecksum_)
            return checksum_;
        else
            return -1;
    };


// The Parser

    NmeaParser::NmeaParser()
    {
    }

    NmeaParser::~NmeaParser()
    {
    }

    int NmeaParser::parseMessage(NmeaMessage &message)
    {
	if(!message.haveSentence_){
	    cerr << "nmea: No Sentence\n";
            return -1;
	}

	if(message.sentence_[0]!=StartOfSentence){
	    cerr << "nmea: Start of sentence($) not found\n";
	    return -1;
	}

	if(strnlen(message.sentence_,MaxSentenceLen+1)>=(MaxSentenceLen)){
	    cerr << "nmea: Sentence incorrect length\n";
            return -1;
	}

        //copy and check the command length
	strncpy(message.command_,&message.sentence_[1],CommandLen);
	if(message.sentence_[6]!=FieldDelim){
	    cerr << "Data delimiter not found\n";
            return -1;
	}
	message.command_[CommandLen]='\0';
	if(strnlen(message.command_,CommandLen)!=CommandLen){
	    cerr << "nmea: Command incorrect length\n";
            return -1;
	}

	char *ptr=&message.sentence_[7];
        unsigned int i=0;
	for(i=0;i<MaxDataLen;i++)
	{
	    if(*(ptr+i)=='\r')
		break;
	    if(*(ptr+i)=='\n'){
		cerr << "nmea: Sentence incorrectly terminated\n";
		cerr << "nmea:  encountered \\n before \\r\n";
		return -1;
                break;
	    }
	    if(*(ptr+i)==ChecksumDelim)
		break;
	    if(*(ptr+i)=='\0'){
		cerr << "nmea: Sentence incorrectly terminated\n";
		cerr << "nmea:  encountered \\0 before endof sentence\n";
		return -1;
		break;
	    }
            // copy each byte (of data)
	    message.data_[i]=*(ptr+i);

	}

	if(i==MaxDataLen){
	    cerr << "nmea: Sentence incorrectly terminated\n";
	    cerr << "nmea:  message exceeds maximum length\n";
	    return -1;
	}
        // terminate the copied data string
	message.data_[i]='\0';
        message.dataLen_=i;

        // optional Checksum
	if(*(ptr+i)==ChecksumDelim){
            i+=1;
            char *endptr=NULL;
	    message.checksum_=strtol(ptr+i,&endptr,16);
	    if(endptr!=NULL){
                // check if strtol worked
                if(endptr==ptr+i+2){
                    message.haveChecksum_=true;
		}
	    }
            i+=2;
	}

        //we are done
	if(strncmp(ptr+i,EndofSentence,2)==0){
	    message.haveData_=true;
	    return 0;
	}
	cerr << "nmea: Sentence incorrectly terminated\n";
	cerr << "nmea:  End of sentence not found\n";
	fprintf(stderr,"nmea:  found: %x%x\n",(int)(*(ptr+i)),(int)(*(ptr+i+1)));


        return -1;

    }

    int NmeaParser::parseData(NmeaMessage &message)
    {
        if(message.haveData_!=true)
	    return -1;

	int field_index=0;
        //reset stuff in message
	message.numDataFields_=0;
        message.haveFields_=false;
        char *field_start=&message.dataFields_[0];
	for( int i=0;i<=message.dataLen_;i++)
	{
            //copy the char
	    message.dataFields_[i]=message.data_[i];
	    if(message.dataFields_[i]==FieldDelim||message.data_[i]=='\0')
	    {
                //terminate current field
		message.dataFields_[i]='\0';
                //assign pointer to field
		message.dataFieldsPtr_[field_index]=field_start;
                // point to start of next field
		field_start=&message.dataFields_[i+1];
                //increment index
		field_index++;
		//increment counter
                message.numDataFields_++;
	    }
            //are we there yet ?
	    if(message.data_[i]=='\0')
	    {
                break;
	    }

	}
	//printf("found %d data fields\n",message.numDataFields_);
	if(message.numDataFields_>0)
	{
            message.haveFields_=true;
	}
        return 0;
    }



}
