#include "QbGifFrame.h"

#include <math.h>

#include "QbStreamTools.h"
#include "QbLzwEncoder.h"
// Debug
#include "QbLzwDictionaryEntry.h"

#define GIF87A "GIF87a"
#define GIF89A "GIF89a"

// CTOR
QbGifFrame::QbGifFrame () {
  // initialize global data
  this->globalData.width = 0;
  this->globalData.height = 0;
  this->globalData.backgroundColor = 0;
  this->globalData.aspectRatioAsByte = 0;
  // GCT (Global Color Table)
  this->globalData.gctFlag = 0;
  this->globalData.gctInitalColorResolution = 0;
  this->globalData.gctSorted = 0;
  this->globalData.gctSize = 0;
  this->globalData.colorTable = NULL;

  // initialize local data
  this->localData.left = 0;
  this->localData.top = 0;
  this->localData.width = 0;
  this->localData.height = 0;
    // LCT (Local Color Table)
  this->localData.lctFlag = 0;
  this->localData.lctSorted = 0;
  this->localData.lctSize = 0;
  this->localData.colorTable = NULL;      
  // Extra data
  this->localData.disposalMethod = 1;
  this->localData.userInput = 0;
  this->localData.transparentFlag = 0;
  this->localData.transparentColor = 0;
  this->localData.delayInMs = -1;
  // Payload
  this->localData.isInterlaced = 0;
  this->localData.lzwData = 0x08; // Minimum LZW code size for Image Data
  this->localData.payloadSize = 0;
  this->localData.payload = NULL;
}

// CTOR
QbGifFrame::QbGifFrame (GlobalData *globalData) {
  int debug = 0;
  this->globalData.width = globalData->width;
  this->globalData.height = globalData->height;
  this->globalData.backgroundColor = globalData->backgroundColor;
  this->globalData.aspectRatioAsByte = globalData->aspectRatioAsByte;
  // GCT (Global Color Table)
  this->globalData.gctFlag = globalData->gctFlag;
  this->globalData.gctInitalColorResolution = globalData->gctInitalColorResolution;
  this->globalData.gctSorted = globalData->gctSorted;
  this->globalData.gctSize = globalData->gctSize;
  this->globalData.colorTable = NULL;
  if (globalData->colorTable) {
    this->globalData.colorTable = (__u_char *)malloc(globalData->gctSize * 3);
    for ( int i = 0; i < globalData->gctSize * 3; i++) {
      this->globalData.colorTable[i] = globalData->colorTable[i];
    }
    debug += globalData->gctSize * 3;
  }

  // initialize local data
  this->localData.left = 0;
  this->localData.top = 0;
  this->localData.width = 0;
  this->localData.height = 0;
    // LCT (Local Color Table)
  this->localData.lctFlag = 0;
  this->localData.lctSorted = 0;
  this->localData.lctSize = 0;
  this->localData.colorTable = NULL;      
  // Extra data
  this->localData.disposalMethod = 1;
  this->localData.userInput = 0;
  this->localData.transparentFlag = 0;
  this->localData.transparentColor = 0;
  this->localData.delayInMs = -1;
  // Payload
  this->localData.isInterlaced = 0;
  this->localData.lzwData = 0x08; // Minimum LZW code size for Image Data
  this->localData.payloadSize = 0;
  this->localData.payload = NULL;
}

QbGifFrame::QbGifFrame (const QbGifBitmap *bitmap) {
  // - - - - - - - - - - - - - -
  // initialize global data
  // - - - - - - - - - - - - - -
  this->globalData.width = bitmap->width;
  this->globalData.height = bitmap->height;
  this->globalData.backgroundColor = 0;
  this->globalData.aspectRatioAsByte = 0;
  // GCT (Global Color Table)
  this->globalData.gctFlag = 0;
  this->globalData.gctInitalColorResolution = 0;
  this->globalData.gctSorted = 0;
  this->globalData.gctSize = 0;
  this->globalData.colorTable = NULL;
  
  // - - - - - - - - - - - - - -
  // initialize local data
  // - - - - - - - - - - - - - -
  this->localData.left = 0;
  this->localData.top = 0;
  this->localData.width = bitmap->width;
  this->localData.height = bitmap->height;
    // LCT (Local Color Table)
  this->localData.lctFlag = 1;
  this->localData.lctSorted = 0;
  this->localData.lctSize = 256;
  this->localData.colorTable = (__u_char *)malloc(this->localData.lctSize * 3);
  for (int i = 0; i < this->localData.lctSize * 3; i++) {
    this->localData.colorTable[i] = bitmap->colorTable[i];
  }
  // Extra data
  this->localData.disposalMethod = 1;
  this->localData.userInput = 0;
  this->localData.transparentFlag = 0;
  this->localData.transparentColor = 0xFF;
  this->localData.delayInMs = -1;
  // Payload
  this->localData.isInterlaced = 0;
  this->localData.lzwData = 0x08; // Minimum LZW code size for Image Data

  // Generate payload from bitmap definition
  int decodedPayloadSize = bitmap->width * bitmap->height;
  vector<__u_char> rawEncodedPayload = QbLzwEncoder::encode(8, bitmap->bitmap, decodedPayloadSize);
  const int blocSize = 254;
  int nbBlocs = rawEncodedPayload.size() / blocSize;
  int lastBlocSize = rawEncodedPayload.size() - (nbBlocs * blocSize);
  int payloadSize = ( blocSize + 1 ) * nbBlocs;
  if( lastBlocSize > 0) {
    payloadSize += lastBlocSize + 1;
  }
  payloadSize++;
  this->localData.payloadSize = payloadSize;
  this->localData.payload = (__u_char*) malloc(payloadSize);
  int posIn = 0;
  int posOut = 0;
  for( int bloc = 0; bloc < nbBlocs; bloc++) {
    this->localData.payload[posOut++] = blocSize;
    for( int i = 0 ; i < blocSize ; i++) {
      this->localData.payload[posOut++] = rawEncodedPayload[posIn++];
    }
  }
  if( lastBlocSize > 0) {
    this->localData.payload[posOut++] = lastBlocSize;
    for( int i = 0 ; i < lastBlocSize ; i++) {
      this->localData.payload[posOut++] = rawEncodedPayload[posIn++];
    }
  }
  this->localData.payload[posOut] = 0x00; // terminator
  int debug = this->localData.lctSize * 3 + payloadSize;
}

QbGifFrame::~QbGifFrame () {
  if (this->globalData.colorTable) free(this->globalData.colorTable);
  if (this->localData.colorTable ) free(this->localData.colorTable );
  if (this->localData.payload) free(this->localData.payload);

  int debug = 0;
  if (this->globalData.colorTable) debug += this->globalData.gctSize * 3;
  if (this->localData.colorTable ) debug += this->localData.lctSize * 3;
  if (this->localData.payload) debug += this->localData.payloadSize;
}

void QbGifFrame::setDelayInMs (int delayInMs) {
  this->localData.delayInMs = delayInMs;
  if (delayInMs <= 0) {
    this->localData.delayInMs = 100;
  }
}

int QbGifFrame::getDelayInMs () {
  return this->localData.delayInMs;
}

int QbGifFrame::getGlobalWidth () {
  return this->globalData.width;
}
  
int QbGifFrame::getGlobalHeight () {
  return this->globalData.height;
}

// Factory
vector<QbGifFrame *> QbGifFrame::createGifFrames (istream &gifStream) {
  vector<QbGifFrame *> result;
  // Read signature + version (must be "GIF87a" or "GIF89a" )
  int len = 6; // length of "GIF87a" or "GIF89a"
  string signatureAndVersion = "";
  for( int i = 0 ; i < len ; i++) {
    int b = QbStreamTools::readByte(gifStream);
    signatureAndVersion += (char)b;
  }
  if ( signatureAndVersion.compare(GIF87A) && signatureAndVersion.compare(GIF89A)) {
      return result;
  }
  QbGifFrame * newFrame = new QbGifFrame();
  newFrame->parseGlobalData(gifStream);
  // newFrame->dumpGlobalData();

  // main loop
  bool exit = false;
  while (!exit) {
    bool bOk = false;
    int firstByte = QbStreamTools::readByte(gifStream);
    // cout << "first byte : " << QbLzwDictionaryEntry::int_to_hex(firstByte,2) << "\n";
    if (firstByte == 0x2c) {
      // Parse frame
      newFrame->readImageDescriptorAndPayload(gifStream);
      // newFrame->dumpLocalData();
      result.push_back(newFrame);
      GlobalData *newGlobalData = &(newFrame->globalData);
      newFrame = new QbGifFrame(newGlobalData);
      bOk = true;
    }
    if (firstByte == 0x21) {
      int secondByte = QbStreamTools::readByte(gifStream);
      if( secondByte == 0xf9) {
        newFrame->readGraphicControlExtension(gifStream);
      } else {
        QbGifFrame::skipExtension(gifStream);
      }
      bOk = true;
    }
    if (firstByte == 0x3b || firstByte == -1 ) {
      exit = true;
      bOk = true;
    }
    if (!bOk) {
        cout << "Parsing error. Unknown descriptor (" + std::to_string(firstByte) + ").\n";
        int nextByte = QbStreamTools::readByte(gifStream);
        cout << "next byte = " + QbLzwDictionaryEntry::int_to_hex(firstByte, 2) + "\n";

        return result;
    }
  }
  if( newFrame ) {
    delete(newFrame);
  }
  return result;
}

void QbGifFrame::writeGlobalData (ostream &stream, int width, int height) {
  __u_char globalPacked = 0x00; // No GCT
  __u_char backgroundColor = 0x00;
  __u_char aspectRatioAsByte = 0x00;
  QbStreamTools::writeString(stream, "GIF89a", 6); // 6 = strlen("Gif89a")
  QbStreamTools::writeShort(stream, width);
  QbStreamTools::writeShort(stream, height);
  QbStreamTools::writeByte(stream, globalPacked);
  QbStreamTools::writeByte(stream, backgroundColor);
  QbStreamTools::writeByte(stream, aspectRatioAsByte);
}

void QbGifFrame::writeNetscape20Extension (ostream &stream, int nbRepetitions) {
  // Application extension
  QbStreamTools::writeByte(stream, 0x21);
  QbStreamTools::writeByte(stream, 0xFF);
  QbStreamTools::writeByte(stream, 0x0B);
  QbStreamTools::writeString(stream, "NETSCAPE2.0", 11); // 11 = strlen("NETSCAPE2.0")
  QbStreamTools::writeByte(stream, 0x03);
  QbStreamTools::writeByte(stream, 0x01);
  QbStreamTools::writeShort(stream, nbRepetitions);
  QbStreamTools::writeByte(stream, 0x00);
}

void QbGifFrame::writeLocalDataAndPayload (ostream &stream) {

  //   .............................
  //   Graphics Control Extension
  //   .............................
  //   1 byte  - Introducer (0x21)
  //   1 byte  - Label = Graphic Control Label (0xF9) 
  //   1 byte  - BlockSize (0x04) 
  //   1 byte  - Packed (Method of graphics disposal)
  //   2 bytes - Delay ( = ms / 10 )
  //   1 byte  - Transparent Color
  //   1 byte  - Terminator (0x00) 
  //
  //   packed
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   --- reserved ---    | Disposal method       |UserInp|Tr.col.|
  //   +-------+-------+-------+-------+-------+-----------------------+
  
  // Build packet
  int packed = (this->localData.disposalMethod & 0x0F) << 2 
             | (this->localData.userInput?1:0) << 1
             | (this->localData.transparentFlag?1:0); 
  QbStreamTools::writeByte(stream, 0x21);
  QbStreamTools::writeByte(stream, 0xF9);
  QbStreamTools::writeByte(stream, 0x04);
  QbStreamTools::writeByte(stream, packed);
  int delay = this->localData.delayInMs / 10;
  if (delay < 2 ) delay = 2;
  if (delay > 65535) delay = 65535;
  QbStreamTools::writeShort(stream, delay);
  QbStreamTools::writeByte(stream, this->localData.transparentColor);
  QbStreamTools::writeByte(stream, 0x00);

  //   ................
  //   Local Image Descriptor
  //   ................
  //   (already read) -> 1 byte  - separator (0x2c)
  //   2 bytes - left (short)
  //   2 bytes - top (short)
  //   2 bytes - width (short)
  //   2 bytes - heigth (short)
  //   1 byte  - packed = image and color LOCAL table information
  //
  //   packed byte:
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   | LCT ? |Interla| Sort  | -- Reserved - |n=tableSizeIndicator(*)|
  //   +-------+-------+-------+-------+-------+-----------------------+
  //
  QbStreamTools::writeByte(stream, 0x2C);
  QbStreamTools::writeShort(stream, this->localData.left);
  QbStreamTools::writeShort(stream, this->localData.top);
  QbStreamTools::writeShort(stream, this->localData.width);
  QbStreamTools::writeShort(stream, this->localData.height);
  // Build packed byte
  bool flag = this->localData.lctFlag;
  __u_char * colorTable = this->localData.colorTable;
  int colorTableSize = this->localData.lctSize;
  bool sorted = this->localData.lctSorted;      
  if (!flag) {
    flag = this->globalData.gctFlag;
    colorTable = this->globalData.colorTable;
    colorTableSize = this->globalData.gctSize;
    sorted = this->globalData.gctSorted;
  }
  packed =  ( (flag?1:0) << 7 )
          | ( (this->localData.isInterlaced?1:0) << 6)
          | ( (sorted?1:0) << 5)
          | ( (int)(log2(colorTableSize)-1) & 0x07);
  QbStreamTools::writeByte(stream, packed);

  // Color table
  for (int i = 0; i < colorTableSize * 3; i++) {
    QbStreamTools::writeByte(stream, colorTable[i] );
  }

  // lzw data
  QbStreamTools::writeByte(stream, this->localData.lzwData);

  // payload
  for (int i = 0; i < this->localData.payloadSize; i++) {
    QbStreamTools::writeByte(stream, this->localData.payload[i] );
  }
}

// ********************** private *******************

void QbGifFrame::parseGlobalData( istream &stream) {
  globalData = this->globalData;
  //   ................      
  //   Logical Screen Descriptor
  //   ................      
  //   2 bytes - width (short)
  //   2 bytes - height (short)
  //   1 byte  - Packed = screen and color GLOBAL table informations
  //   1 byte  - background color
  //   1 byte  - a = aspectRatioIndicator(*)
  //   (*) Aspect ratio = ( a + 15 ) / 64

  globalData.width = QbStreamTools::readShort(stream);
  globalData.height = QbStreamTools::readShort(stream);
  int packed = QbStreamTools::readByte(stream);
  globalData.backgroundColor = QbStreamTools::readByte(stream);
  globalData.aspectRatioAsByte = QbStreamTools::readByte(stream);

  //   ................
  //   packed (GLOBAL)
  //   ................
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   | GCT ? |   Color resolution    |Sorted |n=tableSizeIndicator(*)|
  //   +-------+-------+-------+-------+-------+-----------------------+
  //   (*) Table size = 2 ^ (n+1)   (= nb of colors in the table)
  //
  globalData.gctFlag = ( (packed & 0x80) != 0 );
  globalData.gctInitalColorResolution = (packed & 0x70) >> 4;
  if (globalData.gctFlag) {
    globalData.gctSorted = ( ( (packed & 0x08) >> 3 ) != 0 );
    globalData.gctSize =  1 << ( ( packed & 0x07 ) + 1);
  } else {
    globalData.gctSorted = false;
    globalData.gctSize = 0;
  }

  //   .....................
  //   Global Color Table
  //   .....................
  //   for each color:
  //      1 byte - red
  //      1 byte - green
  //      1 byte - blue
  //
  if (globalData.colorTable) {
    free(globalData.colorTable);
  }
  globalData.colorTable = (__u_char *)malloc(3*globalData.gctSize);
  for (int i = 0; i < globalData.gctSize * 3; i++) {
    globalData.colorTable[i] = QbStreamTools::readByte(stream);
  }
}

void QbGifFrame::readImageDescriptorAndPayload(istream &stream) {
  //   ................
  //   Local Image Descriptor
  //   ................
  //   (already read) -> 1 byte  - separator ( 0x2c )
  //   2 bytes - left (short)
  //   2 bytes - top (short)
  //   2 bytes - width (short)
  //   2 bytes - heigth (short)
  //   1 byte  - packed = image and color LOCAL table information
  //
  this->localData.left = QbStreamTools::readShort(stream);
  this->localData.top = QbStreamTools::readShort(stream);
  // int w = QbStreamTools::readShort(stream);
  // cout << "w = " << w << "\n";
  this->localData.width = QbStreamTools::readShort(stream);
  this->localData.height = QbStreamTools::readShort(stream);
  int packed = QbStreamTools::readByte(stream);

  //   ................
  //   packed (LOCAL)
  //   ................
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   | LCT ? |Interla| Sort  | -- Reserved - |n=tableSizeIndicator(*)|
  //   +-------+-------+-------+-------+-------+-----------------------+
  //

  this->localData.lctFlag = ( (packed & 0x80) != 0 );
  this->localData.isInterlaced = ( ( (packed & 0x40) >> 6) != 0);
  if (this->localData.lctFlag) {
    this->localData.lctSorted = ( ( (packed & 0x20) >> 5 ) != 0 );
    this->localData.lctSize =  1 << ( ( packed & 0x07 ) + 1);
  } else {
    this->localData.lctSorted = false;
    this->localData.lctSize =  0;
  }

  //   .....................
  //   Local Color Table
  //   .....................
  //   for each color:
  //      1 byte - red
  //      1 byte - green
  //      1 byte - blue
  //
  if (this->localData.colorTable) {
    free(this->localData.colorTable);
  }
  this->localData.colorTable = (__u_char *)malloc(3*this->localData.lctSize);
  for (int i = 0; i < this->localData.lctSize * 3; i++) {
    this->localData.colorTable[i] = QbStreamTools::readByte(stream);
  }

  this->localData.lzwData = QbStreamTools::readByte(stream);

  // payload 
  long payloadPos = stream.tellg();
  this->localData.payloadSize = 0;

  // calculate payload size (including lzwData)
  int blocSize = 1;
  while (blocSize > 0) {
    blocSize = QbStreamTools::readByte(stream);
    this->localData.payloadSize++;
    for (int i = 0; i < blocSize; i++) {
      QbStreamTools::readByte(stream);
      this->localData.payloadSize++;
    }
  }

  // read payload
  stream.seekg(payloadPos, stream.beg);
  if (this->localData.payload) {
    free(this->localData.payload);
  }
  this->localData.payload = (__u_char *)malloc(this->localData.payloadSize);
  for (int i = 0; i < this->localData.payloadSize; i++) {
    this->localData.payload[i] = QbStreamTools::readByte(stream);
  }
}

void QbGifFrame::readGraphicControlExtension(istream &stream) {
  //   .....................
  //   Extension
  //   .....................
  //   (already read) -> 1 byte  - separator ( 0x21 )
  //   (already read) -> 1 byte  - extension type (0xf9)
  //   1 byte  - bloc size (0x04)
  //   1 byte  - packed
  //   2 bytes - frame delay (in hundreds of second)
  //   1 byte  - transparent color
  //   1 byte  - terminator (0x00)
  //
  //   Packed:
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
  //   +-------+-------+-------+-------+-------+-------+-------+-------+
  //   |   --- reserved ---    | Disposal method       |UserInp|Tr.col.|
  //   +-------+-------+-------+-------+-------+-----------------------+
  
  QbStreamTools::readByte(stream); // bloc size (0x04)
  int packed = QbStreamTools::readByte(stream);;
  this->localData.delayInMs = QbStreamTools::readShort(stream) * 10;
  if (this->localData.delayInMs <= 0) {
    this->localData.delayInMs = 100;
  }
  this->localData.transparentColor = QbStreamTools::readByte(stream);
  QbStreamTools::readByte(stream); // terminator (0x00)

  // Parse packed byte
  this->localData.disposalMethod = ( (packed >> 2) & 0x0F );
  this->localData.userInput = (packed & 0x02) != 0;
  this->localData.transparentFlag = (packed & 0x01) != 0;
}

void QbGifFrame::skipExtension(istream &stream) {
  //   .....................
  //   Extension
  //   .....................
  //   (already read) -> 1 byte  - separator ( 0x21 )
  //   (already read) -> 1 byte  - extension type
  //   1 byte  - bloc size
  //   ... bloc
  //   1 byte  - bloc size
  //   ... bloc
  //       ...
  //   1 byte  - terminator (0x00)
  //
  int blocSize = 1;
  while (blocSize > 0) {
    blocSize = QbStreamTools::readByte(stream);
    streampos pos = stream.tellg();
    stream.seekg((int)pos + blocSize, stream.beg);
  }
}

// ********* Debug  ****************

void QbGifFrame::dumpGlobalData () {
  //cout << "globalData.width = " << QbLzwDictionaryEntry::int_to_hex(this->globalData.width,4) << "\n";
  cout << "====================== globalData ========================================\n";
  cout << "globalData.width = " << to_string(this->globalData.width) << "\n";
  cout << "globalData.height = " << to_string(this->globalData.height) << "\n";
  cout << "globalData.backgroundColor = " << to_string(this->globalData.backgroundColor) << "\n";
  cout << "globalData.aspectRatioAsByte = " << to_string(this->globalData.aspectRatioAsByte) << "\n";
    // GCT (Global Color Table)
  cout << "globalData.gctFlag = " << to_string(this->globalData.gctFlag) << "\n";
  cout << "globalData.gctInitalColorResolution = " << to_string(this->globalData.gctInitalColorResolution) << "\n";
  cout << "globalData.gctSorted = " << to_string(this->globalData.gctSorted) << "\n";
  cout << "globalData.gctSize = " << to_string(this->globalData.gctSize) << "\n";
  for ( int i = 0; i < this->globalData.gctSize;i++) {
  cout << "this->globalData.colorTable[" << to_string(i) << "] = " 
        << QbLzwDictionaryEntry::int_to_hex(this->globalData.colorTable[3*i+0],2) << " , "
        << QbLzwDictionaryEntry::int_to_hex(this->globalData.colorTable[3*i+1],2) << " , "
        << QbLzwDictionaryEntry::int_to_hex(this->globalData.colorTable[3*i+2],2) << "\n";
      }
  cout << "====================== globalData ========================================\n";
}

void QbGifFrame::dumpLocalData () {
  cout << "====================== localData ========================================\n";
  cout << "localData.left = " << to_string(this->localData.left) << "\n";
  cout << "localData.top = " << to_string(this->localData.top) << "\n";
  cout << "localData.width = " << to_string(this->localData.width) << "\n";
  cout << "localData.height = " << to_string(this->localData.height) << "\n";
    // GCT (Global Color Table)
  cout << "localData.lctFlag = " << to_string(this->localData.lctFlag) << "\n";
  cout << "localData.lctSorted = " << to_string(this->localData.lctSorted) << "\n";
  cout << "localData.lctSize = " << to_string(this->localData.lctSize) << "\n";
  for ( int i = 0; i < this->localData.lctSize;i++) {
  cout << "localData.colorTable[" << to_string(i) << "] = " 
        << QbLzwDictionaryEntry::int_to_hex(this->localData.colorTable[3*i+0],2) << " , "
        << QbLzwDictionaryEntry::int_to_hex(this->localData.colorTable[3*i+1],2) << " , "
        << QbLzwDictionaryEntry::int_to_hex(this->localData.colorTable[3*i+2],2) << "\n";
      }
  // Extra data
  cout << "localData.disposalMethod = " << to_string(this->localData.disposalMethod) << "\n";
  cout << "localData.userInput = " << to_string(this->localData.userInput) << "\n";
  cout << "localData.transparentFlag = " << to_string(this->localData.transparentFlag) << "\n";
  cout << "localData.transparentColor = " << to_string(this->localData.transparentColor) << "\n";
  cout << "localData.delayInMs = " << to_string(this->localData.delayInMs) << "\n";
  // Payload
  cout << "localData.isInterlaced = " << to_string(this->localData.isInterlaced) << "\n";
  cout << "localData.lzwData = " << to_string(this->localData.lzwData) << "\n";
  cout << "localData.payloadSize = " << to_string(this->localData.payloadSize) << "\n";
  cout << "====================== localData ========================================\n";
}
