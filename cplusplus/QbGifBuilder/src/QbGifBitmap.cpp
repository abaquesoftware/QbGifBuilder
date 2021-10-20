#include "QbGifBitmap.h"

#include <stdlib.h>
#include <memory.h>
#include <iostream>

#include "QbGifBuilder.h"
#include "QbGifFrame.h"
#include "QbLzwDecoder.h"

#define NB_COLORS 256

using namespace std;

// CTOR
QbGifBitmap::QbGifBitmap (int width, int height) {
  this->left = 0;
  this->top = 0;
  this->width = width;
  this->height = height;
  this->globalWidth = width;
  this->globalHeight = height;
  this->colorTableSize = NB_COLORS;
  this->colorTable = (__u_char *)malloc(this->colorTableSize * 3);
  memset(this->colorTable, 0, this->colorTableSize * 3);
  this->bitmap = (__u_char *)malloc(width * height);
  memset(this->bitmap, 0, width * height);

  int debug = this->colorTableSize * 3 + width * height;
}

// CTOR
QbGifBitmap::QbGifBitmap (void *frame_) {
  QbGifFrame * frame = (QbGifFrame *)frame_;
  this->left = frame->localData.left;
  this->top = frame->localData.top;
  this->width = frame->localData.width;
  this->height = frame->localData.height;
  this->globalWidth = frame->globalData.width;
  this->globalHeight = frame->globalData.height;
  // Color Table
  bool flag = frame->localData.lctFlag;
  __u_char * colorTable = frame->localData.colorTable;
  int colorTableSize = frame->localData.lctSize;
  if (!flag) {
    flag = frame->globalData.gctFlag;
    colorTable = frame->globalData.colorTable;
    colorTableSize = frame->globalData.gctSize;
  }
  this->colorTableSize = colorTableSize;
  this->colorTable = (__u_char *)malloc(this->colorTableSize * 3);
  memcpy(this->colorTable, colorTable, this->colorTableSize * 3);
  // calculate payload size (including lzwData)
  int truePayloadSize = 0; // Payload size with no bloc descriptor
  int blocSize = 1;
  int inputPos = 0;
  while (blocSize > 0 && inputPos < frame->localData.payloadSize) {
    blocSize = frame->localData.payload[inputPos++];
    truePayloadSize += blocSize;
    inputPos +=blocSize;
  }
  // create payload byteArray (with no bloc descriptor)
  __u_char * payload = (__u_char *)malloc(truePayloadSize);
  blocSize = 1;
  inputPos = 0;
  int outputPos = 0;
  while (blocSize > 0 && inputPos < frame->localData.payloadSize) {
    blocSize = frame->localData.payload[inputPos++];
    memcpy(&payload[outputPos], &frame->localData.payload[inputPos], blocSize);
    inputPos += blocSize;
    outputPos += blocSize;
  }
  // Decode payload
  vector<__u_char> decodedPayload = QbLzwDecoder::decode(frame->localData.lzwData, payload, truePayloadSize);
  free(payload);

  this->bitmap = (__u_char *)malloc(decodedPayload.size());
  int bitmapPos = 0;
  for (auto colorIndex: decodedPayload) {
    this->bitmap[bitmapPos++] = colorIndex;
  }


  int debug = this->colorTableSize * 3 + decodedPayload.size();
}

// CTOR by copy to use vector<QbGifBitmap>
QbGifBitmap::QbGifBitmap (const QbGifBitmap &other) {
  this->left = other.left;
  this->top = other.top;
  this->width = other.width;
  this->height = other.height;
  this->globalWidth = other.globalWidth;
  this->globalHeight = other.globalHeight;
  this->colorTableSize = other.colorTableSize;
  this->colorTable = (__u_char *)malloc(this->colorTableSize * 3);
  memcpy(this->colorTable, other.colorTable, this->colorTableSize * 3);
  this->bitmap = (__u_char *)malloc(this->width * this->height);
  memcpy(this->bitmap, other.bitmap, this->width * this->height);

  int debug = this->colorTableSize * 3 + this->width * this->height;
}

// DTOR
QbGifBitmap::~QbGifBitmap () {
  free(this->colorTable);
  free(this->bitmap);  
  int debug = this->colorTableSize * 3 + this->width * this->height;
}

// Factory - Convert GIF stream -> QbGifBitmap 
vector<QbGifBitmap> QbGifBitmap::createQbGifBitmaps(istream &gifStream) {
  vector<QbGifBitmap> result;
  // Extract GifFrame from gifStream and convert them into QbGifMap
  vector<QbGifFrame *> frameList = QbGifFrame::createGifFrames(gifStream);
  for (auto frame: frameList) {
    QbGifBitmap bitmap = QbGifBitmap(frame);
    result.push_back( bitmap );
  }
  // Clean-up
  for (auto frame: frameList) {
    delete frame;
  }
  return result;
}


void QbGifBitmap::setColor( int index, __u_char red,  __u_char green,  __u_char blue) {
  if( index >= 0 && index < NB_COLORS) {
    int pos = index*3;
    this->colorTable[pos++] = red;
    this->colorTable[pos++] = green;
    this->colorTable[pos  ] = blue;
  }
}

void QbGifBitmap::setPixel( int x, int y, __u_char colorIndex) {
  int pos = y * this->width + x;
  if( pos >= 0 && pos < this->width * this->height ) {
    this->bitmap[pos] = colorIndex;
  }
}
