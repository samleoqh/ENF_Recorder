#ifndef EAFF_H
#define EAFF_H

#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <CTime>
#include <time.h>

using namespace std;

class WaveFile;
class AudioENF;
int fileSize(FILE& inf);
void crtENF( string inpFile,struct tm *ctm,
             string NID, string RID, int sR, int nBit);

uint32_t FourCC ( const char *CName );
void CName(uint32_t ID, char* cID);

enum OpenMode
{
   UNKNOWN,      // undefined type (can use to mean "N/A" or "not open")
   WRITE,        // open for write
   READ          // open for read
};

struct Location{
	uint32_t NationID; //indicates National ID of an audio like "NORW"
	uint32_t RegionID; //indicates Regional ID of an audio like "OSLO"
};

struct DateTime{
	uint16_t sec;	//seconds of minutes from 0 to 59
	uint16_t min;	//minutes of hour from 0 to 59
	uint16_t hour;	//hours of day from 0 to 24
	uint16_t day;	//day of month from 1 to 31
	uint16_t mon;	//month of year from 1 to 12
	uint32_t year;	//year since 1900
};	

typedef struct ENF {
	uint32_t formatID;//indicates audio format here is "ENF"
	Location locationID; //indicates record location
	DateTime creationTime; //indicates recording time
	uint32_t sampleRate; // the sample rate of audio in HZ
	uint16_t bitsperSample; //number of bits per sample: 8/16 or 32.
	uint32_t dataSize; //audio data size in bytes
} ENF_Header;

class AudioENF
{
	FILE *fENF;
	ENF_Header enfHeader;
	
  public:
	  AudioENF();
	  ~AudioENF();
	  void Open(const char* filename, OpenMode OM);
	  void setFormat(string formt="ENF");
      void setLocation();						//set by enter location
      void setLocation(string sNat, string sReg);//set loc directly
      void setTime();							//set by enter datetime
      void setLocalTime();
	  void setTime(struct tm *ctm);				//set datetime
      void setRate(uint32_t rate=8000);
      void setBits(uint16_t bits=16);
	  void setRateBits();						//set by enter rate and bits
	  void setRateBits(uint32_t rate, uint16_t bits);
	  void setSize(uint32_t newSize){enfHeader.dataSize = newSize; }
      void copyHeader(WaveFile& wav);
      void copyHeader(ENF_Header& eh);
      void setDefaultHeader();
      void Close();
	  
	  void WriteHeader();
	  void WriteHeader(ENF_Header& eh); // write header by copy exist header
	  void WriteHeader(WaveFile& wav);	// write header from wavfile
	  void WriteData(const int16_t* data, uint32_t num);
	  void getHeader(ENF_Header& eh);
	  uint32_t getDataSize(){return enfHeader.dataSize;}
	  uint32_t getfileSize(){return fileSize(*fENF);}
	  
	  void PrintHeader(); // print out header info for debug
	  friend int fileSize(FILE& inf);
};

//define WAVE file structure and Class
//big endian = BE, little endian = LE File-offset-bytes = FOB  
struct Riff_CK_Header
{
	uint8_t ckID[4]; 	//BE 4-byte FOB:0   = "RIFF"
	uint32_t cksize; 	//LE 4-byte FOB:4
	uint8_t format[4];	//BE 4-byte FOB:8	= "WAVE" 
};

struct Fmt_CK_Header
{
	uint8_t  ckid[4];		//BE 4-byte FOB:12 = "fmt"
	uint32_t cksize;		//LE 4-byte FOB:16
	uint16_t audioformat;	//LE 2-byte FOB:20 1=PCM, 6=mulaw, 7=alaw
	uint16_t numchannels;	//LE 2-byte FOB:22 1=Mono, 2=Sterio
	uint32_t samplerate;	//LE 4-byte FOB:24 sample frequency in HZ
	uint32_t byterate;		//LE 4-byte FOB:28 bytes per second
	uint16_t blockalign;	//LE 2-byte FOB:32 2=16-bit mono, 4=16-bit stereo
	uint16_t bitspersample;	//LE 2-byte FOB:34
};

struct Data_CK_Header
{
	uint8_t ckid[4];	//BE 4-byte FOB:36 	= "data" 
	uint32_t cksize;	//LE 4-byte FOB:40 sampled data length
};

typedef struct WAV_HEADER 
{
	Riff_CK_Header  riff;
	Fmt_CK_Header   fmt;
	Data_CK_Header  data;
} Wav_Header;


class WaveFile
{
	friend class AudioENF;
  private:	
	FILE *fWav;
	Wav_Header wavheader;
	
  public:
	WaveFile();
	~WaveFile();
	
	void Open(const char *filename);
	bool isValidwav(void);
	int ReadSamples(int num, int16_t *sarray[2]);
	uint16_t nChan(void){return wavheader.fmt.numchannels; }		
	uint16_t nBits(void){return wavheader.fmt.bitspersample; }					
	uint32_t sRate(void){return wavheader.fmt.samplerate; }	
	uint32_t dSize(void){return wavheader.data.cksize; }	
	void printheader();
};

#endif // EAFF_H
