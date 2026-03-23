#pragma once

//  Macros to convert 4 bytes to a Riff-style ID DWORD.
//  Todo: make these endian independent !!!

// Main Riff-Wave header.
struct FRiffWaveHeader
{ 
	DWORD	rID;			// Contains 'RIFF'
	DWORD	ChunkLen;		// Remaining length of the entire riff chunk (= file).
	DWORD	wID;			// Form type. Contains 'WAVE' for .wav files.
};

// General chunk header format.
struct FRiffChunkOld
{
	DWORD	ChunkID;		  // General data chunk ID like 'data', or 'fmt ' 
	DWORD	ChunkLen;		  // Length of the rest of this chunk in bytes.
};

// ChunkID: 'fmt ' ("WaveFormatEx" structure ) 
struct FFormatChunk
{
	WORD   wFormatTag;        // Format type: 1 = PCM
	WORD   nChannels;         // Number of channels (i.e. mono, stereo...).
	DWORD   nSamplesPerSec;    // Sample rate. 44100 or 22050 or 11025  Hz.
	DWORD   nAvgBytesPerSec;   // For buffer estimation  = sample rate * BlockAlign.
	WORD   nBlockAlign;       // Block size of data = Channels times BYTES per sample.
	WORD   wBitsPerSample;    // Number of bits per sample of mono data.
	WORD   cbSize;            // The count in bytes of the size of extra information (after cbSize).
};

// ChunkID: 'smpl'
struct FSampleChunk
{
	DWORD   dwManufacturer;
	DWORD   dwProduct;
	DWORD   dwSamplePeriod;
	DWORD   dwMIDIUnityNote;
	DWORD   dwMIDIPitchFraction;
	DWORD	dwSMPTEFormat;		
	DWORD   dwSMPTEOffset;		//
	DWORD   cSampleLoops;		// Number of tSampleLoop structures following this chunk
	DWORD   cbSamplerData;		// 
};

struct FSampleLoop				// Immediately following cbSamplerData in the SMPL chunk.
{
	DWORD	dwIdentifier;		//
	DWORD	dwType;				//
	DWORD	dwStart;			// Startpoint of the loop in samples
	DWORD	dwEnd;				// Endpoint of the loop in samples
	DWORD	dwFraction;			// Fractional sample adjustment
	DWORD	dwPlayCount;		// Play count
};

class CWaveModInfo
{
public:
	FFormatChunk mFormat;
	Stream*		 mStream;
	size_t		 mDataPos;
	size_t		 mDataSize;
	bool		 mOk;

	CWaveModInfo(Stream* pStream);

	bool readData(void* pData,size_t size);

};
