#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbGifBitmap.h"
#include "QbGifBuilder/include/QbGifFrame.h"
#include "QbGifBuilder/include/QbLzwDictionaryEntry.h"
#include <fstream>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE( "QbGifBitmap - constructors", "[QbGifBitmap]" ) {

  // CTOR: QbGifBitmap (int width, int height)

  QbGifBitmap bitmap1(220, 150);
  CHECK( bitmap1.left == 0);
  CHECK( bitmap1.top == 0);
  CHECK( bitmap1.width == 220);
  CHECK( bitmap1.height == 150);
  CHECK( bitmap1.globalWidth == 220);
  CHECK( bitmap1.globalHeight == 150);
  CHECK( bitmap1.colorTableSize == 256);
  int pos = 0;
  for (int i = 0; i < bitmap1.colorTableSize*3; i++) {
    CHECK(bitmap1.colorTable[i] == 0x00);
  }
  for (int i = 0; i < bitmap1.width * bitmap1.height; i++) {
    CHECK(bitmap1.bitmap[i] == 0);
  }

  // CTOR: QbGifBitmap (void * frame);

    // NOTE :
  // ../data/wiki_example.gif is a GIF animation built by
  // ../dataBuilder/dataBuilder.x
  // Its content is described in this page:
  // https://en.wikipedia.org/wiki/GIF#Example_GIF_file
  //
  const char *inputFilename = "../data/wiki_example.gif";
  ifstream inputStream(inputFilename, ifstream::binary);
  vector<QbGifFrame *> test = QbGifFrame::createGifFrames(inputStream);
  REQUIRE(test.size() == 1);
  QbGifFrame *frame = test[0];
  QbGifBitmap bitmap2(frame);

  CHECK( bitmap2.left == 0);
  CHECK( bitmap2.top == 0);
  CHECK( bitmap2.width == 3);
  CHECK( bitmap2.height == 5);
  CHECK( bitmap2.globalWidth == 3);
  CHECK( bitmap2.globalHeight == 5);
  CHECK( bitmap2.colorTableSize == 256);
  pos = 0;
  for (int i = 0; i < bitmap2.colorTableSize; i++) {
    if (i == 0x28) {
      CHECK(bitmap2.colorTable[pos++] == 0x00);
      CHECK(bitmap2.colorTable[pos++] == 0x00);
      CHECK(bitmap2.colorTable[pos++] == 0x00);
    } else {
      CHECK(bitmap2.colorTable[pos++] == i);
      CHECK(bitmap2.colorTable[pos++] == i);
      CHECK(bitmap2.colorTable[pos++] == i);
    }
  }
  __u_char bitmap_ref[] = {
    0x28,0xFF,0xFF,
    0xFF,0x28,0xFF,
    0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF
  };
  for (int i = 0; i < bitmap2.width * bitmap2.height; i++) {
    CHECK(bitmap2.bitmap[i] == bitmap_ref[i]);
  }

  // CTOR: QbGifBitmap (const QbGifBitmap &other)

  QbGifBitmap bitmap3(bitmap2);
  CHECK( bitmap3.left == 0);
  CHECK( bitmap3.top == 0);
  CHECK( bitmap3.width == 3);
  CHECK( bitmap3.height == 5);
  CHECK( bitmap3.globalWidth == 3);
  CHECK( bitmap3.globalHeight == 5);
  CHECK( bitmap3.colorTableSize == 256);
  pos = 0;
  for (int i = 0; i < bitmap3.colorTableSize; i++) {
    if (i == 0x28) {
      CHECK(bitmap3.colorTable[pos++] == 0x00);
      CHECK(bitmap3.colorTable[pos++] == 0x00);
      CHECK(bitmap3.colorTable[pos++] == 0x00);
    } else {
      CHECK(bitmap3.colorTable[pos++] == i);
      CHECK(bitmap3.colorTable[pos++] == i);
      CHECK(bitmap3.colorTable[pos++] == i);
    }
  }
  for (int i = 0; i < bitmap3.width * bitmap3.height; i++) {
    CHECK(bitmap3.bitmap[i] == bitmap_ref[i]);
  }
}

TEST_CASE( "Ctor QbGifFrame (const QbGifBitmap *bitmap)", "[QbGifBitmap]" ) {
  // NOTE:
  // Ctor: QbGifFrame (const QbGifBitmap *bitmap)
  // Due to circular references, this constructor
  // is tested in tdd-QbGifBitmap.cpp

  // Bitmap is the gif described here:
  // https://en.wikipedia.org/wiki/GIF#Example_GIF_file
  //
  // +-+-+-+
  // |X| | |
  // +-+-+-+
  // | |X| |
  // +-+-+-+
  // | | | |
  // +-+-+-+
  // | | | |
  // +-+-+-+
  // | | | |
  // +-+-+-+

  QbGifBitmap bitmap(3, 5);
  // build color table
  for (int color = 0; color < 256; color++) {
    int pos = color * 3;
    if (color == 0x28) {
      bitmap.colorTable[pos++] = 0x00;
      bitmap.colorTable[pos++] = 0x00;
      bitmap.colorTable[pos++] = 0x00;
    } else {
      bitmap.colorTable[pos++] = color;
      bitmap.colorTable[pos++] = color;
      bitmap.colorTable[pos++] = color;
    }
  }
  // build map
  for (int i = 0; i < bitmap.width * bitmap.height; i++) {
    bitmap.bitmap[i] = 0xFF;
  }
  bitmap.bitmap[0] = 0x28;
  bitmap.bitmap[4] = 0x28;

  // Buid the frame and check it
  QbGifFrame frame(&bitmap);

  CHECK(frame.globalData.width == 3);
  CHECK(frame.globalData.height == 5);
  CHECK(frame.globalData.backgroundColor == 0);
  CHECK(frame.globalData.aspectRatioAsByte == 0);
  // GCT (Global Color Table)
  CHECK(frame.globalData.gctFlag == 0);
  CHECK(frame.globalData.gctInitalColorResolution == 0);
  CHECK(frame.globalData.gctSorted == false);
  CHECK(frame.globalData.gctSize == 0);
  CHECK(frame.globalData.colorTable == NULL);
  // -- Local data
  CHECK(frame.localData.left == 0);
  CHECK(frame.localData.top == 0);
  CHECK(frame.localData.width == 3);
  CHECK(frame.localData.height == 5);
  // LCT (Local Color Table)
  CHECK(frame.localData.lctFlag == true);
  CHECK(frame.localData.lctSorted == 0);
  CHECK(frame.localData.lctSize == 256);
  CHECK(frame.localData.colorTable != NULL);
  // Check local color table
  for (int color = 0; color < 256; color++) {
    int pos = color * 3;
    if (color == 0x28) {
      CHECK( (int)frame.localData.colorTable[pos++] == 0x00);
      CHECK( (int)frame.localData.colorTable[pos++] == 0x00);
      CHECK( (int)frame.localData.colorTable[pos++] == 0x00);
    } else {
      CHECK( (int)frame.localData.colorTable[pos++] == color);
      CHECK( (int)frame.localData.colorTable[pos++] == color);
      CHECK( (int)frame.localData.colorTable[pos++] == color);
    }
  }
  // Extra data
  CHECK(frame.localData.disposalMethod == 1);
  CHECK(frame.localData.userInput == 0);
  CHECK(frame.localData.transparentFlag == 0);
  CHECK(frame.localData.transparentColor == 0xFF);
  CHECK(frame.localData.delayInMs == -1);
  // Payload
  CHECK(frame.localData.isInterlaced == 0);
  CHECK(frame.localData.lzwData == 0x08); // Minimum LZW code size for Image Data
  CHECK(frame.localData.payloadSize == 13);
  CHECK(frame.localData.payload != NULL);
  // Check payload
  // Note: payload contents payloadSize(0x0B) + 0x00 + payload 
  string payload_as_string = "";
  for( int i = 0; i < frame.localData.payloadSize; i++) {
    payload_as_string += 
         QbLzwDictionaryEntry::int_to_hex(frame.localData.payload[i], 2);
    payload_as_string += ",";
  }
  CHECK(payload_as_string == "0x0b,0x00,0x51,0xfc,0x1b,0x28,0x70,0xa0,0xc1,0x83,0x01,0x01,0x00,");
}

TEST_CASE( "Factory (createQbGifBitmaps)", "[QbGifBitmap]" ) {
    // NOTE :
  // ../data/red_green_blue.gif is a GIF animation built by
  // ../dataBuilder/dataBuilder.x
  // It contains 3 frames (220x150)displayed 1 second:
  // - frame 0 = red  : all pixels with color index = 0 (RGB = 255,0,0)
  // - frame 1 = green: all pixels with color index = 0 (RGB = 0,255,0)
  // - frame 2 = blue : all pixels with color index = 0 (RGB = 0,0,255)

  const char *inputFilename = "../data/red_green_blue.gif";
  ifstream inputStream(inputFilename, ifstream::binary);
  vector<QbGifBitmap> bitmapList = QbGifBitmap::createQbGifBitmaps(inputStream);
  inputStream.close();

  REQUIRE(bitmapList.size() == 3);
  // Check all bitmap size
  for (auto bitmap: bitmapList) {
    CHECK(bitmap.width == 220);
    CHECK(bitmap.height == 150);
  }
  // Check color tables for all frames
  for (auto bitmap: bitmapList) {
    // Red
    CHECK(bitmap.colorTable[0] == 0xFF);
    CHECK(bitmap.colorTable[1] == 0x00);
    CHECK(bitmap.colorTable[2] == 0x00);
    // Green
    CHECK(bitmap.colorTable[3] == 0x00);
    CHECK(bitmap.colorTable[4] == 0xFF);
    CHECK(bitmap.colorTable[5] == 0x00);
    // Blue
    CHECK(bitmap.colorTable[6] == 0x00);
    CHECK(bitmap.colorTable[7] == 0x00);
    CHECK(bitmap.colorTable[8] == 0xFF);
  }
  // Check bitmap
  for (int i = 0; i < bitmapList[0].width*bitmapList[0].height; i++) {
    // Red
    CHECK(bitmapList[0].bitmap[i] == 0x00);
    // Green
    CHECK(bitmapList[1].bitmap[i] == 0x01);
    // Blue
    CHECK(bitmapList[2].bitmap[i] == 0x02);
  }
}

TEST_CASE( "setColor / setPixel", "[QbGifBitmap]" ) {

  QbGifBitmap bitmap(220, 150);

  // setColor
  int colorIndex = 10;
  __u_char red = 0x10;
  __u_char green = 0x11;
  __u_char blue = 0x12;
  bitmap.setColor( colorIndex, red, green, blue);
  // Check color change
  int pos = colorIndex * 3;
  CHECK(bitmap.colorTable[pos++] == red);
  CHECK(bitmap.colorTable[pos++] == green);
  CHECK(bitmap.colorTable[pos++] == blue);

  // setColor
  int x = 100;
  int y = 50;
  bitmap.setPixel(x, y, colorIndex);
  // Check pixel change
  pos = x + y * bitmap.width;
  CHECK(bitmap.bitmap[pos] == colorIndex);
}
