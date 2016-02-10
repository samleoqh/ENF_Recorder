#include "EAFF.h"

uint32_t FourCC ( const char *CName )
{
   uint32_t retbuf = 0x20202020;   // four spaces (padding)
   char *p = ((char *)&retbuf);
   // Remember, this is Intel format!
   // The first character goes in the LSB
   for ( int i=0; i<4 && CName[i]; i++ )
   {
	  *p++ = CName[i];
   }
   return retbuf;
}

//convert uint32 into char[4] for printout
void CName(uint32_t ID, char* cID)
{
	char *p = ((char *)&ID);
	for ( int i=0; i<4 ; i++ )
	{
		cID[i]=*p++;
	}
}

AudioENF::AudioENF() 
{
	fENF = NULL;
}
AudioENF::~AudioENF()
{ 
	if(fENF != NULL) fclose(fENF); 
}

void AudioENF::Open(const char* filename, OpenMode OM)
{
    switch ( OM )
    {
        case READ:
            fENF = fopen(filename, "rb"); //must use "rb"
            if (fENF!=0) {
                fread(&enfHeader, 1, sizeof (ENF_Header), fENF);
            }
            else{
                cout<<"read AudioENF file failure...."<<endl;
            }
            break;
        case WRITE:
            fENF = fopen(filename, "wb"); //must use "wb"
            if (fENF == 0)
                cout<<"read AudioENF file failure...."<<endl;
            break;
        default:
            break;
    }
}

void AudioENF::setTime(struct tm *ctm)
{
    enfHeader.creationTime.sec = ctm->tm_sec;
    enfHeader.creationTime.min = ctm->tm_min;
    enfHeader.creationTime.hour = ctm->tm_hour;
    enfHeader.creationTime.day = ctm->tm_mday;
    enfHeader.creationTime.mon = ctm->tm_mon + 1;
    enfHeader.creationTime.year = ctm->tm_year + 1900;
}

void AudioENF::setLocalTime()
{
	time_t rawTm;
	struct tm *ctm;
	time(&rawTm);
	ctm = localtime(&rawTm);

	setTime(ctm);
}


void AudioENF::setDefaultHeader()
{
	enfHeader.formatID = FourCC("ENF");
	enfHeader.locationID.NationID = FourCC("NORW");
	enfHeader.locationID.RegionID = FourCC("OSLO");
    setLocalTime();
	enfHeader.sampleRate = 1000;
	enfHeader.bitsperSample = 16;
	enfHeader.dataSize = 0;
}


void AudioENF::setFormat(string formt)
{
	enfHeader.formatID = FourCC(formt.c_str());
}

void AudioENF::setLocation()
{
	string input;
	cout<<"Enter recording National ID (upto4 chars):	";
	getline(cin, input);
	enfHeader.locationID.NationID = FourCC(input.c_str());
	cout<<"Enter recording Regional ID (upto4 chars):	";
	getline(cin, input);
	enfHeader.locationID.RegionID = FourCC(input.c_str());
}
void AudioENF::setLocation(string sNat, string sReg)
{
	enfHeader.locationID.NationID = FourCC(sNat.c_str());
	enfHeader.locationID.RegionID = FourCC(sReg.c_str());
}

void AudioENF::setTime()
{
	int hh,mi,ss,dd,mm,yyyy;
	cout<<"Enter recording time (hh:mi:ss):  	";
	scanf("%u:%u:%u", &hh, &mi, &ss);
	enfHeader.creationTime.sec = ss;
	enfHeader.creationTime.min = mi;
	enfHeader.creationTime.hour = hh;
	cout<<"Enter recording date (dd.mm.yyyy):	";
	scanf("%u.%u.%u",&dd, &mm, &yyyy);			
	enfHeader.creationTime.day = dd;
	enfHeader.creationTime.mon = mm;
	enfHeader.creationTime.year = yyyy;	
}

void AudioENF::setRateBits()
{
	int rate, bits;
	cout<<"Enter sample-rate: 	";
	scanf("%u", &rate);
	cout<<"Enter bits-per-sample:	";
	scanf("%u", &bits);
	enfHeader.sampleRate = rate;
	enfHeader.bitsperSample = bits;
}
void AudioENF::setRate(uint32_t rate)
{
	enfHeader.sampleRate = rate;
}

void AudioENF::setBits(uint16_t bits)
{
	enfHeader.bitsperSample = bits;
}

void AudioENF::setRateBits(uint32_t rate, uint16_t bits)
{
	enfHeader.sampleRate = rate;
	enfHeader.bitsperSample = bits;
}

void AudioENF::copyHeader(WaveFile& wav)
{
    enfHeader.dataSize = wav.dSize()/wav.nChan();
    enfHeader.bitsperSample = wav.nBits();
    enfHeader.sampleRate = wav.sRate();
}

void AudioENF::copyHeader(ENF_Header& eh)
{
	enfHeader.formatID = eh.formatID; 
	enfHeader.locationID = eh.locationID; 
	enfHeader.creationTime = eh.creationTime; 
	enfHeader.sampleRate = eh.sampleRate; 
	enfHeader.bitsperSample = eh.bitsperSample; 
	enfHeader.dataSize = eh.dataSize; 
}


void AudioENF::getHeader(ENF_Header& eh)
{
	eh.formatID = enfHeader.formatID;
	eh.locationID = enfHeader.locationID; 
	eh.creationTime = enfHeader.creationTime;
	eh.sampleRate = enfHeader.sampleRate;
	eh.bitsperSample = enfHeader.bitsperSample;
	eh.dataSize = enfHeader.dataSize; 
}

void AudioENF::WriteHeader()
{
	fwrite(&enfHeader, 1, sizeof (ENF_Header), fENF);
}

void AudioENF::WriteHeader(ENF_Header& eh)
{
	copyHeader(eh);
	WriteHeader();
}
void AudioENF::WriteHeader(WaveFile& wav)
{
	copyHeader(wav);
	setFormat();
	setLocation();
	setTime();
	WriteHeader();
}

void AudioENF::WriteData(const int16_t* data, uint32_t num)
{
	fwrite( data, sizeof(int16_t), num, fENF );
}

void AudioENF::Close()
{
    if(fENF != NULL) fclose(fENF);
}

void AudioENF::PrintHeader()
{
    char cID[4];
    cout<<"------------ENF Audio header inform-----------"<<endl;
    CName(enfHeader.formatID,cID);
    cout<<"formatID: 	" << cID[0] << cID[1]<< cID[2]<< cID[3]<< endl;
    CName(enfHeader.locationID.NationID,cID);
    cout<<"locationID: 	" << cID[0] << cID[1]<< cID[2]<< cID[3];
    CName(enfHeader.locationID.RegionID,cID);
    cout<<", " << cID[0] << cID[1]<< cID[2]<< cID[3]<< endl;
    cout<<"creationTime:	" << enfHeader.creationTime.day<<"."
                              << enfHeader.creationTime.mon<<"."
                              << enfHeader.creationTime.year<<" "
                              << enfHeader.creationTime.hour<<":"
                              << enfHeader.creationTime.min<<":"
                              << enfHeader.creationTime.sec<< endl;
    cout<<"sampleRate: 	" << enfHeader.sampleRate <<" HZ "<< endl;
    cout<<"bitsperSample:	" << enfHeader.bitsperSample << endl;
    cout<<"dataSize: 	" << enfHeader.dataSize <<" bytes " << endl;
}

WaveFile::WaveFile()
{ 
	fWav = NULL; 
}
WaveFile::~WaveFile()
{
	if (fWav != NULL) {
		fclose(fWav); 
	}
}

void WaveFile::Open(const char *filename)
{
    fWav = fopen(filename, "rb");
	if (fWav!=0)
	{
		fread(&wavheader, 1, sizeof (Wav_Header), fWav);
	}
	else{
		cout<<"read wave file failure...."<<endl;
	}		
}

bool WaveFile::isValidwav(void)
{ 
	uint32_t nChan, nBits, sRate;
	nChan = wavheader.fmt.numchannels;
	sRate = wavheader.fmt.samplerate;
	nBits = wavheader.fmt.bitspersample;
	
	return 	(wavheader.fmt.numchannels == 1 || 
			 wavheader.fmt.numchannels == 2    ) &&
			(wavheader.fmt.byterate == (nChan*sRate*nBits)/8) &&
			(wavheader.fmt.blockalign == (nChan*nBits)/8);				
} 
	
int WaveFile::ReadSamples(int num, int16_t *sarray[2])
{
	int Size;
	int nByte = (nBits()==16) ? 2 : 1;
	int i = 0;
	bool unend = 1;
	
	for ( i=0; i < num && unend; i++ )  
	{
		unend = (fread(&sarray[0][i],nByte,1,fWav) == 1);
		
		if((nChan() == 2) && unend){
			unend = (fread(&sarray[1][i],nByte,1,fWav) == 1);
		}						
	}
	Size = unend ? i : (i-1);
	return Size;
}

void WaveFile::printheader()
{
	if (fWav!=0)
	{
		cout<<"------------wav header information-----------"<<endl;
		cout<<"chunkID:  	"
		<<wavheader.riff.ckID[0]<<wavheader.riff.ckID[1]
		<<wavheader.riff.ckID[2]<<wavheader.riff.ckID[3]<<endl;
		cout<<"chunkSize:	"<<wavheader.riff.cksize <<endl;
		cout<<"format:		"
		<<wavheader.riff.format[0]<<wavheader.riff.format[1]
		<<wavheader.riff.format[2]<<wavheader.riff.format[3]<<endl;
		cout<<"chunkID: 	"
		<<wavheader.fmt.ckid[0]<<wavheader.fmt.ckid[1]
		<<wavheader.fmt.ckid[2]<<wavheader.fmt.ckid[3]<< endl;
		cout<<"chunkSize:	"<<wavheader.fmt.cksize<< endl;
		cout<<"audioFormat:	"<<wavheader.fmt.audioformat<< endl;
		cout<<"numChannels:	"<<wavheader.fmt.numchannels<< endl;
		cout<<"sampleRate:	"<<wavheader.fmt.samplerate<< endl;
		cout<<"byteRate:	"<<wavheader.fmt.byterate<< endl;
		cout<<"blockAlign:	"<<wavheader.fmt.blockalign<< endl;
		cout<<"bitsPerSample:	"<<wavheader.fmt.bitspersample<<endl;
		cout<<"chunkID:  	"	
		<<wavheader.data.ckid[0]<<wavheader.data.ckid[1]
		<<wavheader.data.ckid[2]<<wavheader.data.ckid[3]<<endl;
		cout<<"chunksize:	" << wavheader.data.cksize<<endl;
		cout<<"-------------------------------------------"<<endl;	
	}	
}

void crtENF(string inpFile, struct tm *ctm, string NID, string RID,
             int sRate, int nBits)
{
    FILE* in_fid;
    AudioENF enfAudio;
    string newENF;
    newENF = inpFile + ".enf";

    in_fid = fopen( inpFile.c_str(), "rb" );
    if (in_fid != 0)
    {
        enfAudio.Open(newENF.c_str(),WRITE);

        int dataSize = 0;
        if (inpFile.find(".wav")<inpFile.length()){
            Wav_Header wavheader;
            fread(&wavheader, 1, sizeof (Wav_Header), in_fid);
            dataSize = wavheader.data.cksize;
        }else{
            dataSize = fileSize(*in_fid);
        }
        enfAudio.setFormat();
        enfAudio.setLocation(NID,RID);
        enfAudio.setTime(ctm);
        enfAudio.setRate(sRate);
        enfAudio.setBits(nBits);
        enfAudio.setSize(dataSize);
        enfAudio.WriteHeader();

        int size, sums=0;
        int16_t input[80];
        do {
              size = fread( input, sizeof(int16_t), 80, in_fid );
              sums+=size;
              enfAudio.WriteData(input, size);
           } while ( size != 0 );
    }
    fclose( in_fid );
    enfAudio.Close();
}

int fileSize(FILE& inf)
{
	int size;
	fseek(&inf, 0, SEEK_END);
	size = ftell(&inf);
	fseek(&inf, 0, SEEK_SET);
	return size;
}


