/* LzmaSDK.h -- amalgamated ver. 9.22
   Igor Pavlov
   Public domain */

#ifndef __LZMA_SDK_H
#define __LZMA_SDK_H

#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Types.h

#ifndef EXTERN_C_BEGIN
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
#endif

EXTERN_C_BEGIN

#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11
#define SZ_ERROR_THREAD 12

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17

typedef int SRes;

#ifdef _WIN32
typedef DWORD WRes;
#else
typedef int WRes;
#endif

#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif

typedef unsigned char Byte;
typedef short Int16;
typedef unsigned short UInt16;

#ifdef _LZMA_UINT32_IS_ULONG
typedef long Int32;
typedef unsigned long UInt32;
#else
typedef int Int32;
typedef unsigned int UInt32;
#endif

#ifdef _SZ_NO_INT_64

/* define _SZ_NO_INT_64, if your compiler doesn't support 64-bit integers.
 NOTES: Some code will work incorrectly in that case! */

typedef long Int64;
typedef unsigned long UInt64;

#else

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#define UINT64_CONST(n) n
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#define UINT64_CONST(n) n ## ULL
#endif

#endif

#ifdef _LZMA_NO_SYSTEM_SIZE_T
typedef UInt32 SizeT;
#else
typedef size_t SizeT;
#endif

typedef int Bool;
#define True 1
#define False 0


#ifdef _WIN32
#define MY_STD_CALL __stdcall
#else
#define MY_STD_CALL
#endif

#ifdef _MSC_VER

#if _MSC_VER >= 1300
#define MY_NO_INLINE __declspec(noinline)
#else
#define MY_NO_INLINE
#endif

#define MY_CDECL __cdecl
#define MY_FAST_CALL __fastcall

#else

#define MY_CDECL
#define MY_FAST_CALL

#endif

/* The following interfaces use first parameter as pointer to structure */

typedef struct
{
    Byte (*Read)(void *p); /* reads one byte, returns 0 in case of EOF or error */
} IByteIn;

typedef struct
{
    void (*Write)(void *p, Byte b);
} IByteOut;

typedef struct
{
    SRes (*Read)(void *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
     (output(*size) < input(*size)) is allowed */
} ISeqInStream;

/* it can return SZ_ERROR_INPUT_EOF */
SRes SeqInStream_Read(ISeqInStream *stream, void *buf, size_t size);
SRes SeqInStream_Read2(ISeqInStream *stream, void *buf, size_t size, SRes errorType);
SRes SeqInStream_ReadByte(ISeqInStream *stream, Byte *buf);

typedef struct
{
    size_t (*Write)(void *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
     (result < size) means error */
} ISeqOutStream;

typedef enum
{
    SZ_SEEK_SET = 0,
    SZ_SEEK_CUR = 1,
    SZ_SEEK_END = 2
} ESzSeek;

typedef struct
{
    SRes (*Read)(void *p, void *buf, size_t *size);  /* same as ISeqInStream::Read */
    SRes (*Seek)(void *p, Int64 *pos, ESzSeek origin);
} ISeekInStream;

typedef struct
{
    SRes (*Look)(void *p, const void **buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
     (output(*size) > input(*size)) is not allowed
     (output(*size) < input(*size)) is allowed */
    SRes (*Skip)(void *p, size_t offset);
    /* offset must be <= output(*size) of Look */
    
    SRes (*Read)(void *p, void *buf, size_t *size);
    /* reads directly (without buffer). It's same as ISeqInStream::Read */
    SRes (*Seek)(void *p, Int64 *pos, ESzSeek origin);
} ILookInStream;

SRes LookInStream_LookRead(ILookInStream *stream, void *buf, size_t *size);
SRes LookInStream_SeekTo(ILookInStream *stream, UInt64 offset);

/* reads via ILookInStream::Read */
SRes LookInStream_Read2(ILookInStream *stream, void *buf, size_t size, SRes errorType);
SRes LookInStream_Read(ILookInStream *stream, void *buf, size_t size);

#define LookToRead_BUF_SIZE (1 << 14)

typedef struct
{
    ILookInStream s;
    ISeekInStream *realStream;
    size_t pos;
    size_t size;
    Byte buf[LookToRead_BUF_SIZE];
} CLookToRead;

void LookToRead_CreateVTable(CLookToRead *p, int lookahead);
void LookToRead_Init(CLookToRead *p);

typedef struct
{
    ISeqInStream s;
    ILookInStream *realStream;
} CSecToLook;

void SecToLook_CreateVTable(CSecToLook *p);

typedef struct
{
    ISeqInStream s;
    ILookInStream *realStream;
} CSecToRead;

void SecToRead_CreateVTable(CSecToRead *p);

typedef struct
{
    SRes (*Progress)(void *p, UInt64 inSize, UInt64 outSize);
    /* Returns: result. (result != SZ_OK) means break.
     Value (UInt64)(Int64)-1 for size means unknown value. */
} ICompressProgress;

typedef struct
{
    void *(*Alloc)(void *p, size_t size);
    void (*Free)(void *p, void *address); /* address can be 0 */
} ISzAlloc;

#define IAlloc_Alloc(p, size) (p)->Alloc((p), size)
#define IAlloc_Free(p, a) (p)->Free((p), a)

#ifdef _WIN32

#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define STRING_PATH_SEPARATOR "\\"
#define WSTRING_PATH_SEPARATOR L"\\"

#else

#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define STRING_PATH_SEPARATOR "/"
#define WSTRING_PATH_SEPARATOR L"/"

#endif

#define MY_STDAPI int MY_STD_CALL

#define LZMA_PROPS_SIZE 5

// LzmaEnc.h

typedef struct _CLzmaEncProps
{
    int level;       /*  0 <= level <= 9 */
    UInt32 dictSize; /* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version
                      (1 << 12) <= dictSize <= (1 << 30) for 64-bit version
                      default = (1 << 24) */
    UInt32 reduceSize; /* estimated size of data that will be compressed. default = 0xFFFFFFFF.
                        Encoder uses this value to reduce dictionary size */
    int lc;          /* 0 <= lc <= 8, default = 3 */
    int lp;          /* 0 <= lp <= 4, default = 0 */
    int pb;          /* 0 <= pb <= 4, default = 2 */
    int algo;        /* 0 - fast, 1 - normal, default = 1 */
    int fb;          /* 5 <= fb <= 273, default = 32 */
    int btMode;      /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
    int numHashBytes; /* 2, 3 or 4, default = 4 */
    UInt32 mc;        /* 1 <= mc <= (1 << 30), default = 32 */
    unsigned writeEndMark;  /* 0 - do not write EOPM, 1 - write EOPM, default = 0 */
    int numThreads;  /* 1 or 2, default = 2 */
} CLzmaEncProps;

void LzmaEncProps_Init(CLzmaEncProps *p);
void LzmaEncProps_Normalize(CLzmaEncProps *p);
UInt32 LzmaEncProps_GetDictSize(const CLzmaEncProps *props2);


/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc_* functions can return the following exit codes:
 Returns:
 SZ_OK           - OK
 SZ_ERROR_MEM    - Memory allocation error
 SZ_ERROR_PARAM  - Incorrect paramater in props
 SZ_ERROR_WRITE  - Write callback error.
 SZ_ERROR_PROGRESS - some break from progress callback
 SZ_ERROR_THREAD - errors in multithreading functions (only for Mt version)
 */

typedef void * CLzmaEncHandle;

CLzmaEncHandle LzmaEnc_Create(ISzAlloc *alloc);
void LzmaEnc_Destroy(CLzmaEncHandle p, ISzAlloc *alloc, ISzAlloc *allocBig);
SRes LzmaEnc_SetProps(CLzmaEncHandle p, const CLzmaEncProps *props);
SRes LzmaEnc_WriteProperties(CLzmaEncHandle p, Byte *properties, SizeT *size);
SRes LzmaEnc_Encode(CLzmaEncHandle p, ISeqOutStream *outStream, ISeqInStream *inStream,
                    ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);
SRes LzmaEnc_MemEncode(CLzmaEncHandle p, Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
                       int writeEndMark, ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);

/* ---------- One Call Interface ---------- */

/* LzmaEncode
 Return code:
 SZ_OK               - OK
 SZ_ERROR_MEM        - Memory allocation error
 SZ_ERROR_PARAM      - Incorrect paramater
 SZ_ERROR_OUTPUT_EOF - output buffer overflow
 SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
 */

SRes LzmaEncode(Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
                const CLzmaEncProps *props, Byte *propsEncoded, SizeT *propsSize, int writeEndMark,
                ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);

// LzFind.h
typedef UInt32 CLzRef;

typedef struct _CMatchFinder
{
    Byte *buffer;
    UInt32 pos;
    UInt32 posLimit;
    UInt32 streamPos;
    UInt32 lenLimit;
    
    UInt32 cyclicBufferPos;
    UInt32 cyclicBufferSize; /* it must be = (historySize + 1) */
    
    UInt32 matchMaxLen;
    CLzRef *hash;
    CLzRef *son;
    UInt32 hashMask;
    UInt32 cutValue;
    
    Byte *bufferBase;
    ISeqInStream *stream;
    int streamEndWasReached;
    
    UInt32 blockSize;
    UInt32 keepSizeBefore;
    UInt32 keepSizeAfter;
    
    UInt32 numHashBytes;
    int directInput;
    size_t directInputRem;
    int btMode;
    int bigHash;
    UInt32 historySize;
    UInt32 fixedHashSize;
    UInt32 hashSizeSum;
    UInt32 numSons;
    SRes result;
    UInt32 crc[256];
} CMatchFinder;

#define Inline_MatchFinder_GetPointerToCurrentPos(p) ((p)->buffer)
#define Inline_MatchFinder_GetIndexByte(p, index) ((p)->buffer[(Int32)(index)])

#define Inline_MatchFinder_GetNumAvailableBytes(p) ((p)->streamPos - (p)->pos)

int MatchFinder_NeedMove(CMatchFinder *p);
Byte *MatchFinder_GetPointerToCurrentPos(CMatchFinder *p);
void MatchFinder_MoveBlock(CMatchFinder *p);
void MatchFinder_ReadIfRequired(CMatchFinder *p);

void MatchFinder_Construct(CMatchFinder *p);

/* Conditions:
 historySize <= 3 GB
 keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
 */
int MatchFinder_Create(CMatchFinder *p, UInt32 historySize,
                       UInt32 keepAddBufferBefore, UInt32 matchMaxLen, UInt32 keepAddBufferAfter,
                       ISzAlloc *alloc);
void MatchFinder_Free(CMatchFinder *p, ISzAlloc *alloc);
void MatchFinder_Normalize3(UInt32 subValue, CLzRef *items, UInt32 numItems);
void MatchFinder_ReduceOffsets(CMatchFinder *p, UInt32 subValue);

UInt32 * GetMatchesSpec1(UInt32 lenLimit, UInt32 curMatch, UInt32 pos, const Byte *buffer, CLzRef *son,
                         UInt32 _cyclicBufferPos, UInt32 _cyclicBufferSize, UInt32 _cutValue,
                         UInt32 *distances, UInt32 maxLen);

/*
 Conditions:
 Mf_GetNumAvailableBytes_Func must be called before each Mf_GetMatchLen_Func.
 Mf_GetPointerToCurrentPos_Func's result must be used only before any other function
 */

typedef void (*Mf_Init_Func)(void *object);
typedef Byte (*Mf_GetIndexByte_Func)(void *object, Int32 index);
typedef UInt32 (*Mf_GetNumAvailableBytes_Func)(void *object);
typedef const Byte * (*Mf_GetPointerToCurrentPos_Func)(void *object);
typedef UInt32 (*Mf_GetMatches_Func)(void *object, UInt32 *distances);
typedef void (*Mf_Skip_Func)(void *object, UInt32);

typedef struct _IMatchFinder
{
    Mf_Init_Func Init;
    Mf_GetIndexByte_Func GetIndexByte;
    Mf_GetNumAvailableBytes_Func GetNumAvailableBytes;
    Mf_GetPointerToCurrentPos_Func GetPointerToCurrentPos;
    Mf_GetMatches_Func GetMatches;
    Mf_Skip_Func Skip;
} IMatchFinder;

void MatchFinder_CreateVTable(CMatchFinder *p, IMatchFinder *vTable);

void MatchFinder_Init(CMatchFinder *p);
UInt32 Bt3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);
UInt32 Hc3Zip_MatchFinder_GetMatches(CMatchFinder *p, UInt32 *distances);
void Bt3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);
void Hc3Zip_MatchFinder_Skip(CMatchFinder *p, UInt32 num);

// LzHash.h
#define kHash2Size (1 << 10)
#define kHash3Size (1 << 16)
#define kHash4Size (1 << 20)

#define kFix3HashSize (kHash2Size)
#define kFix4HashSize (kHash2Size + kHash3Size)
#define kFix5HashSize (kHash2Size + kHash3Size + kHash4Size)

#define HASH2_CALC hashValue = cur[0] | ((UInt32)cur[1] << 8);

#define HASH3_CALC { \
UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
hash2Value = temp & (kHash2Size - 1); \
hashValue = (temp ^ ((UInt32)cur[2] << 8)) & p->hashMask; }

#define HASH4_CALC { \
UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
hash2Value = temp & (kHash2Size - 1); \
hash3Value = (temp ^ ((UInt32)cur[2] << 8)) & (kHash3Size - 1); \
hashValue = (temp ^ ((UInt32)cur[2] << 8) ^ (p->crc[cur[3]] << 5)) & p->hashMask; }

#define HASH5_CALC { \
UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
hash2Value = temp & (kHash2Size - 1); \
hash3Value = (temp ^ ((UInt32)cur[2] << 8)) & (kHash3Size - 1); \
hash4Value = (temp ^ ((UInt32)cur[2] << 8) ^ (p->crc[cur[3]] << 5)); \
hashValue = (hash4Value ^ (p->crc[cur[4]] << 3)) & p->hashMask; \
hash4Value &= (kHash4Size - 1); }

/* #define HASH_ZIP_CALC hashValue = ((cur[0] | ((UInt32)cur[1] << 8)) ^ p->crc[cur[2]]) & 0xFFFF; */
#define HASH_ZIP_CALC hashValue = ((cur[2] | ((UInt32)cur[0] << 8)) ^ p->crc[cur[1]]) & 0xFFFF;


#define MT_HASH2_CALC \
hash2Value = (p->crc[cur[0]] ^ cur[1]) & (kHash2Size - 1);

#define MT_HASH3_CALC { \
UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
hash2Value = temp & (kHash2Size - 1); \
hash3Value = (temp ^ ((UInt32)cur[2] << 8)) & (kHash3Size - 1); }

#define MT_HASH4_CALC { \
UInt32 temp = p->crc[cur[0]] ^ cur[1]; \
hash2Value = temp & (kHash2Size - 1); \
hash3Value = (temp ^ ((UInt32)cur[2] << 8)) & (kHash3Size - 1); \
hash4Value = (temp ^ ((UInt32)cur[2] << 8) ^ (p->crc[cur[3]] << 5)) & (kHash4Size - 1); }

#endif

// LzmaDec.h

/* #define _LZMA_PROB32 */
/* _LZMA_PROB32 can increase the speed on some CPUs,
 but memory usage for CLzmaDec::probs will be doubled in that case */

#ifdef _LZMA_PROB32
#define CLzmaProb UInt32
#else
#define CLzmaProb UInt16
#endif


/* ---------- LZMA Properties ---------- */

#define LZMA_PROPS_SIZE 5

typedef struct _CLzmaProps
{
    unsigned lc, lp, pb;
    UInt32 dicSize;
} CLzmaProps;

/* LzmaProps_Decode - decodes properties
 Returns:
 SZ_OK
 SZ_ERROR_UNSUPPORTED - Unsupported properties
 */

SRes LzmaProps_Decode(CLzmaProps *p, const Byte *data, unsigned size);


/* ---------- LZMA Decoder state ---------- */

/* LZMA_REQUIRED_INPUT_MAX = number of required input bytes for worst case.
 Num bits = log2((2^11 / 31) ^ 22) + 26 < 134 + 26 = 160; */

#define LZMA_REQUIRED_INPUT_MAX 20

typedef struct
{
    CLzmaProps prop;
    CLzmaProb *probs;
    Byte *dic;
    const Byte *buf;
    UInt32 range, code;
    SizeT dicPos;
    SizeT dicBufSize;
    UInt32 processedPos;
    UInt32 checkDicSize;
    unsigned state;
    UInt32 reps[4];
    unsigned remainLen;
    int needFlush;
    int needInitState;
    UInt32 numProbs;
    unsigned tempBufSize;
    Byte tempBuf[LZMA_REQUIRED_INPUT_MAX];
} CLzmaDec;

#define LzmaDec_Construct(p) { (p)->dic = 0; (p)->probs = 0; }

void LzmaDec_Init(CLzmaDec *p);

/* There are two types of LZMA streams:
 0) Stream with end mark. That end mark adds about 6 bytes to compressed size.
 1) Stream without end mark. You must know exact uncompressed size to decompress such stream. */

typedef enum
{
    LZMA_FINISH_ANY,   /* finish at any point */
    LZMA_FINISH_END    /* block must be finished at the end */
} ELzmaFinishMode;

/* ELzmaFinishMode has meaning only if the decoding reaches output limit !!!
 
 You must use LZMA_FINISH_END, when you know that current output buffer
 covers last bytes of block. In other cases you must use LZMA_FINISH_ANY.
 
 If LZMA decoder sees end marker before reaching output limit, it returns SZ_OK,
 and output value of destLen will be less than output buffer size limit.
 You can check status result also.
 
 You can use multiple checks to test data integrity after full decompression:
 1) Check Result and "status" variable.
 2) Check that output(destLen) = uncompressedSize, if you know real uncompressedSize.
 3) Check that output(srcLen) = compressedSize, if you know real compressedSize.
 You must use correct finish mode in that case. */

typedef enum
{
    LZMA_STATUS_NOT_SPECIFIED,               /* use main error code instead */
    LZMA_STATUS_FINISHED_WITH_MARK,          /* stream was finished with end mark. */
    LZMA_STATUS_NOT_FINISHED,                /* stream was not finished */
    LZMA_STATUS_NEEDS_MORE_INPUT,            /* you must provide more input bytes */
    LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK  /* there is probability that stream was finished without end mark */
} ELzmaStatus;

/* ELzmaStatus is used only as output value for function call */


/* ---------- Interfaces ---------- */

/* There are 3 levels of interfaces:
 1) Dictionary Interface
 2) Buffer Interface
 3) One Call Interface
 You can select any of these interfaces, but don't mix functions from different
 groups for same object. */


/* There are two variants to allocate state for Dictionary Interface:
 1) LzmaDec_Allocate / LzmaDec_Free
 2) LzmaDec_AllocateProbs / LzmaDec_FreeProbs
 You can use variant 2, if you set dictionary buffer manually.
 For Buffer Interface you must always use variant 1.
 
 LzmaDec_Allocate* can return:
 SZ_OK
 SZ_ERROR_MEM         - Memory allocation error
 SZ_ERROR_UNSUPPORTED - Unsupported properties
 */

SRes LzmaDec_AllocateProbs(CLzmaDec *p, const Byte *props, unsigned propsSize, ISzAlloc *alloc);
void LzmaDec_FreeProbs(CLzmaDec *p, ISzAlloc *alloc);

SRes LzmaDec_Allocate(CLzmaDec *state, const Byte *prop, unsigned propsSize, ISzAlloc *alloc);
void LzmaDec_Free(CLzmaDec *state, ISzAlloc *alloc);

/* ---------- Dictionary Interface ---------- */

/* You can use it, if you want to eliminate the overhead for data copying from
 dictionary to some other external buffer.
 You must work with CLzmaDec variables directly in this interface.
 
 STEPS:
 LzmaDec_Constr()
 LzmaDec_Allocate()
 for (each new stream)
 {
 LzmaDec_Init()
 while (it needs more decompression)
 {
 LzmaDec_DecodeToDic()
 use data from CLzmaDec::dic and update CLzmaDec::dicPos
 }
 }
 LzmaDec_Free()
 */

/* LzmaDec_DecodeToDic
 
 The decoding to internal dictionary buffer (CLzmaDec::dic).
 You must manually update CLzmaDec::dicPos, if it reaches CLzmaDec::dicBufSize !!!
 
 finishMode:
 It has meaning only if the decoding reaches output limit (dicLimit).
 LZMA_FINISH_ANY - Decode just dicLimit bytes.
 LZMA_FINISH_END - Stream must be finished after dicLimit.
 
 Returns:
 SZ_OK
 status:
 LZMA_STATUS_FINISHED_WITH_MARK
 LZMA_STATUS_NOT_FINISHED
 LZMA_STATUS_NEEDS_MORE_INPUT
 LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
 SZ_ERROR_DATA - Data error
 */

SRes LzmaDec_DecodeToDic(CLzmaDec *p, SizeT dicLimit,
                         const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);


/* ---------- Buffer Interface ---------- */

/* It's zlib-like interface.
 See LzmaDec_DecodeToDic description for information about STEPS and return results,
 but you must use LzmaDec_DecodeToBuf instead of LzmaDec_DecodeToDic and you don't need
 to work with CLzmaDec variables manually.
 
 finishMode:
 It has meaning only if the decoding reaches output limit (*destLen).
 LZMA_FINISH_ANY - Decode just destLen bytes.
 LZMA_FINISH_END - Stream must be finished after (*destLen).
 */

SRes LzmaDec_DecodeToBuf(CLzmaDec *p, Byte *dest, SizeT *destLen,
                         const Byte *src, SizeT *srcLen, ELzmaFinishMode finishMode, ELzmaStatus *status);


/* ---------- One Call Interface ---------- */

/* LzmaDecode
 
 finishMode:
 It has meaning only if the decoding reaches output limit (*destLen).
 LZMA_FINISH_ANY - Decode just destLen bytes.
 LZMA_FINISH_END - Stream must be finished after (*destLen).
 
 Returns:
 SZ_OK
 status:
 LZMA_STATUS_FINISHED_WITH_MARK
 LZMA_STATUS_NOT_FINISHED
 LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK
 SZ_ERROR_DATA - Data error
 SZ_ERROR_MEM  - Memory allocation error
 SZ_ERROR_UNSUPPORTED - Unsupported properties
 SZ_ERROR_INPUT_EOF - It needs more bytes in input buffer (src).
 */

SRes LzmaDecode(Byte *dest, SizeT *destLen, const Byte *src, SizeT *srcLen,
                const Byte *propData, unsigned propSize, ELzmaFinishMode finishMode,
                ELzmaStatus *status, ISzAlloc *alloc);

// Alloc.h

void *MyAlloc(size_t size);
void MyFree(void *address);

#ifdef _WIN32

void SetLargePageSize();

void *MidAlloc(size_t size);
void MidFree(void *address);
void *BigAlloc(size_t size);
void BigFree(void *address);

#else

#define MidAlloc(size) MyAlloc(size)
#define MidFree(address) MyFree(address)
#define BigAlloc(size) MyAlloc(size)
#define BigFree(address) MyFree(address)

#endif

// LzmaLib.h

/*
RAM requirements for LZMA:
  for compression:   (dictSize * 11.5 + 6 MB) + state_size
  for decompression: dictSize + state_size
    state_size = (4 + (1.5 << (lc + lp))) KB
    by default (lc=3, lp=0), state_size = 16 KB.

LZMA properties (5 bytes) format
    Offset Size  Description
      0     1    lc, lp and pb in encoded form.
      1     4    dictSize (little endian).
*/

/*
LzmaCompress
------------

outPropsSize -
     In:  the pointer to the size of outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.
     Out: the pointer to the size of written properties in outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.

  LZMA Encoder will use defult values for any parameter, if it is
  -1  for any from: level, loc, lp, pb, fb, numThreads
   0  for dictSize
  
level - compression level: 0 <= level <= 9;

  level dictSize algo  fb
    0:    16 KB   0    32
    1:    64 KB   0    32
    2:   256 KB   0    32
    3:     1 MB   0    32
    4:     4 MB   0    32
    5:    16 MB   1    32
    6:    32 MB   1    32
    7+:   64 MB   1    64
 
  The default value for "level" is 5.

  algo = 0 means fast method
  algo = 1 means normal method

dictSize - The dictionary size in bytes. The maximum value is
        128 MB = (1 << 27) bytes for 32-bit version
          1 GB = (1 << 30) bytes for 64-bit version
     The default value is 16 MB = (1 << 24) bytes.
     It's recommended to use the dictionary that is larger than 4 KB and
     that can be calculated as (1 << N) or (3 << N) sizes.

lc - The number of literal context bits (high bits of previous literal).
     It can be in the range from 0 to 8. The default value is 3.
     Sometimes lc=4 gives the gain for big files.

lp - The number of literal pos bits (low bits of current position for literals).
     It can be in the range from 0 to 4. The default value is 0.
     The lp switch is intended for periodical data when the period is equal to 2^lp.
     For example, for 32-bit (4 bytes) periodical data you can use lp=2. Often it's
     better to set lc=0, if you change lp switch.

pb - The number of pos bits (low bits of current position).
     It can be in the range from 0 to 4. The default value is 2.
     The pb switch is intended for periodical data when the period is equal 2^pb.

fb - Word size (the number of fast bytes).
     It can be in the range from 5 to 273. The default value is 32.
     Usually, a big number gives a little bit better compression ratio and
     slower compression process.

numThreads - The number of thereads. 1 or 2. The default value is 2.
     Fast mode (algo = 0) can use only 1 thread.

Out:
  destLen  - processed output size
Returns:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

MY_STDAPI LzmaCompress(unsigned char *dest, size_t *destLen, const unsigned char *src, size_t srcLen,
  unsigned char *outProps, size_t *outPropsSize, /* *outPropsSize must be = 5 */
  int level,      /* 0 <= level <= 9, default = 5 */
  unsigned dictSize,  /* default = (1 << 24) */
  int lc,        /* 0 <= lc <= 8, default = 3  */
  int lp,        /* 0 <= lp <= 4, default = 0  */
  int pb,        /* 0 <= pb <= 4, default = 2  */
  int fb,        /* 5 <= fb <= 273, default = 32 */
  int numThreads /* 1 or 2, default = 2 */
  );

/*
LzmaUncompress
--------------
In:
  dest     - output data
  destLen  - output data size
  src      - input data
  srcLen   - input data size
Out:
  destLen  - processed output size
  srcLen   - processed input size
Returns:
  SZ_OK                - OK
  SZ_ERROR_DATA        - Data error
  SZ_ERROR_MEM         - Memory allocation arror
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

MY_STDAPI LzmaUncompress(unsigned char *dest, size_t *destLen, const unsigned char *src, SizeT *srcLen,
  const unsigned char *props, size_t propsSize);

EXTERN_C_END
