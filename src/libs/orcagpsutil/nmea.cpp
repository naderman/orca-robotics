/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <assert.h>
#include <orcamisc/tokenise.h>

// Make sure strnlen is defined.
#include <orcaportability/strnlen.h>

#include "nmea.h"

using namespace std;
using namespace orcagpsutil;

const char NMEAStartOfSentence  = '$';
const char NMEAChecksumDelim    = '*';


//*************************************************************
//The blank constructor
NmeaMessage::NmeaMessage()
{
    init();
}

//**************************************************************
void NmeaMessage::init()
{
    haveSentence_ = false;
    haveTokens_   = false;
    haveCheckSum_ = false;
    checkSumOK_   = false;

    //! Now clear the internal data store
    sentence_[0]   = 0;
    dataTokens_.clear();
}


//****************************************************************
NmeaMessage::NmeaMessage(const char *sentence, int testCheckSum)
{
    init();
    setSentence(sentence,testCheckSum);
}


//****************************************************************************

NmeaMessage::~NmeaMessage()
{    
}


//**********************************************************************
//Load the data as requested and test the checksum if we are asked to.
void NmeaMessage::setSentence(const char *data, int AddOrTestCheckSum)
{
    init();
    
    strncpy(sentence_,data, MAX_SENTENCE_LEN);

    //terminate just in case, Note that we have a buffer which is
    //MAX_SENTENCE_LEN + 1 long!
    
    sentence_[MAX_SENTENCE_LEN] = '\0';
    haveSentence_             = true;

    switch(AddOrTestCheckSum)
    {
    case TestChecksum:  //This is for Rx'd data that we need to test for correct reception
        testChecksumOk(); break;
    case AddChecksum:   //This is for Tx data that needs to checksummed before sending
        addCheckSum(); checkSumOK_ = true; break;
    case DontTestOrAddChecksum: 
        break;
    default:
        assert(true);
    }

}


//*****************************************************************
bool NmeaMessage::testChecksumOk()
{
    haveCheckSum_ = true;
    checkSumOK_   = false;

    //First save the checksum chars from the existing message
    char* ptr;
    char  chksum_HIB,chksum_LOB;
   
    //First save the existing two checksum chars from the message
    //These are straight after the '*' character
    ptr = strchr(sentence_, NMEAChecksumDelim);
    if(!ptr){return false;}
   
    //save the high and low bytes of the checksum
    //Make sure they are in upper case!
    chksum_HIB = toupper(*(++ptr));  
    chksum_LOB = toupper(*(ptr + 1));
   

    //invalidate the existing checksum
    *ptr = *(ptr+1) = 'x';
        
    //****NOTE** We leave the ptr pointing at the first chksum byte
       
    //Re-calculate our own copy of the checksum
    addCheckSum();

    //Now compare our saved version with our new ones
    if((chksum_HIB == *ptr) && (chksum_LOB == *(ptr+1))){
        //all looked good!
        checkSumOK_ = true;
        return true;
    }
   
    //failed the checksum!
    return false;
        
}


//*****************************************************
// Add the checksum chars to an existing message
// NOTE: this assumes that there is allready space in the message for
// the checksum, and that the checksum delimiter is there
 
void NmeaMessage::addCheckSum(){   
      
    assert( haveSentence_ );
    
    haveCheckSum_ = true;

    //check that we have the '$' at the start
    if(sentence_[0]!= NMEAStartOfSentence)
       {return;}

    unsigned char chkRunning = 0;
    
    int loopCount;
    unsigned char nextChar;
    for( loopCount =1; loopCount < MAX_SENTENCE_LEN; loopCount++){
    
        nextChar = static_cast<unsigned char>(sentence_[loopCount]);
    
        // no delimiter uh oh
        if((nextChar=='\r')||(nextChar=='\n')||(nextChar=='\0')){
            throw NmeaException("nmea: cannot calculate checksum, missing '*'\n");
            return;
        }
		
            // goodie we found it
        if(nextChar==NMEAChecksumDelim)
          {break;}
    
        //Keep the running total going
        chkRunning ^= nextChar;
    }
        
    //Put the byte values as upper case HEX back into the message
    sprintf(sentence_ + loopCount + 1,"%02X",chkRunning);

}


//**********************************************************************
// Parse the data fields of our message...
void NmeaMessage::parseTokens(){

    //We should not attempt to be parsing a message twice...
    assert (numDataTokens() == 0);

    //Split the message at the commas
    //TODO cope with missing fields
    orcamisc::tokenise(sentence_, dataTokens_, ",");
    
    //Now discard the $ and the * from the first and last tokens...
    //TODO : - dataTokens_[0] = 

    //keep track of what we have done.
    haveTokens_ = true;

}
