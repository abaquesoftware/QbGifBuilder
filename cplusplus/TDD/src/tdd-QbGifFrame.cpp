#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbGifFrame.h"
#include "QbGifBuilder/include/QbLzwDictionaryEntry.h"
#include "QbGifBuilder/include/QbStreamTools.h"
#include <fstream>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE( "QbGifFrame - constructors", "[QbGifFrame]" ) {

  // Ctor: QbGifFrame () 
  QbGifFrame frame1;
  CHECK(frame1.globalData.width == 0);
  CHECK(frame1.globalData.height == 0);
  CHECK(frame1.globalData.backgroundColor == 0);
  CHECK(frame1.globalData.aspectRatioAsByte == 0);
  // GCT (Global Color Table)
  CHECK(frame1.globalData.gctFlag == 0);
  CHECK(frame1.globalData.gctInitalColorResolution == 0);
  CHECK(frame1.globalData.gctSorted == 0);
  CHECK(frame1.globalData.gctSize == 0);
  CHECK(frame1.globalData.colorTable == NULL);
  // initialize local data
  CHECK(frame1.localData.left == 0);
  CHECK(frame1.localData.top == 0);
  CHECK(frame1.localData.width == 0);
  CHECK(frame1.localData.height == 0);
  // LCT (Local Color Table)
  CHECK(frame1.localData.lctFlag == 0);
  CHECK(frame1.localData.lctSorted == 0);
  CHECK(frame1.localData.lctSize == 0);
  CHECK(frame1.localData.colorTable == NULL);
  // Extra data
  CHECK(frame1.localData.disposalMethod == 1);
  CHECK(frame1.localData.userInput == 0);
  CHECK(frame1.localData.transparentFlag == 0);
  CHECK(frame1.localData.transparentColor == 0);
  CHECK(frame1.localData.delayInMs == -1);
  // Payload
  CHECK(frame1.localData.isInterlaced == 0);
  CHECK(frame1.localData.lzwData == 0x08); // Minimum LZW code size for Image Data
  CHECK(frame1.localData.payloadSize == 0);
  CHECK(frame1.localData.payload == NULL);

  // Ctor: QbGifFrame (GlobalData *globalData);
  QbGifFrame::GlobalData global_data;
  global_data.width=100;
  global_data.height=200;
  global_data.backgroundColor = 3;
  global_data.aspectRatioAsByte = 1;
  // GCT (Global Color Table)
  global_data.gctFlag = 1;
  global_data.gctInitalColorResolution = 2;
  global_data.gctSorted = 1;
  global_data.gctSize = 4;
  global_data.colorTable = (__u_char *)malloc(4*3);
  for(int i = 0; i < 4*3; i++) {
    global_data.colorTable[i] = i;
  }

  QbGifFrame frame2(&global_data);
  CHECK(frame2.globalData.width == 100);
  CHECK(frame2.globalData.height == 200);
  CHECK(frame2.globalData.backgroundColor == 3);
  CHECK(frame2.globalData.aspectRatioAsByte == 1);
  // GCT (Global Color Table)
  CHECK(frame2.globalData.gctFlag == 1);
  CHECK(frame2.globalData.gctInitalColorResolution == 2);
  CHECK(frame2.globalData.gctSorted == 1);
  CHECK(frame2.globalData.gctSize == 4);
  CHECK(frame2.globalData.colorTable != NULL);
  for (int i = 0 ; i < 4 * 3; i++) {
    CHECK( (int)frame2.globalData.colorTable[i] == i);
  }
  // -- Local data
  CHECK(frame2.localData.left == 0);
  CHECK(frame2.localData.top == 0);
  CHECK(frame2.localData.width == 0);
  CHECK(frame2.localData.height == 0);
  // LCT (Local Color Table)
  CHECK(frame2.localData.lctFlag == 0);
  CHECK(frame2.localData.lctSorted == 0);
  CHECK(frame2.localData.lctSize == 0);
  CHECK(frame2.localData.colorTable == NULL);
  // Extra data
  CHECK(frame2.localData.disposalMethod == 1);
  CHECK(frame2.localData.userInput == 0);
  CHECK(frame2.localData.transparentFlag == 0);
  CHECK(frame2.localData.transparentColor == 0);
  CHECK(frame2.localData.delayInMs == -1);
  // Payload
  CHECK(frame2.localData.isInterlaced == 0);
  CHECK(frame2.localData.lzwData == 0x08); // Minimum LZW code size for Image Data
  CHECK(frame2.localData.payloadSize == 0);
  CHECK(frame2.localData.payload == NULL);

  // NOTE:
  // Ctor: QbGifFrame (const QbGifBitmap *bitmap)
  // Due to circular references, this constructor
  // is tested in tdd-QbGifBitmap.cpp
}

TEST_CASE( "factory from GifFile - test with a single frame GIF file (wiki_example.gif)", "[QbGifFrame]" ) {
  // NOTE :
  // ../data/wiki_example.gif is a GIF animation built by
  // ../dataBuilder/dataBuilder.x
  // Its content is described in this page:
  // https://en.wikipedia.org/wiki/GIF#Example_GIF_file
  //

  const char *inputFilename = "../data/wiki_example.gif";
  ifstream inputStream(inputFilename, ifstream::binary);
  vector<QbGifFrame *> frameList = QbGifFrame::createGifFrames(inputStream);
    // close stream
  inputStream.close();

  REQUIRE(frameList.size() == 1);
  QbGifFrame *frame = frameList[0];
  // -- Global data
  CHECK(frame->globalData.width == 3);
  CHECK(frame->globalData.height == 5);
  CHECK(frame->localData.width == 3);
  CHECK(frame->localData.height == 5);
  // GCT (Global Color Table)
  CHECK(frame->globalData.gctFlag == 1);
  CHECK(frame->globalData.gctInitalColorResolution == 7);
  CHECK(frame->globalData.gctSorted == 0);
  CHECK(frame->globalData.gctSize == 256);
  CHECK(frame->globalData.colorTable != NULL);
  // Check GCT content
  // Black
  int pos = 0x28 * 3;
  CHECK((int)frame->globalData.colorTable[pos++] == 0x00);
  CHECK((int)frame->globalData.colorTable[pos++] == 0x00);
  CHECK((int)frame->globalData.colorTable[pos++] == 0x00);
  // Green
  pos = 0xFF * 3;
  CHECK((int)frame->globalData.colorTable[pos++] == 0xFF);
  CHECK((int)frame->globalData.colorTable[pos++] == 0xFF);
  CHECK((int)frame->globalData.colorTable[pos++] == 0xFF);
  // -- Local data
  CHECK(frame->localData.left == 0);
  CHECK(frame->localData.top == 0);
  CHECK(frame->localData.width == 3);
  CHECK(frame->localData.height == 5);
  // LCT (Local Color Table)
  CHECK(frame->localData.lctFlag == 0);
  CHECK(frame->localData.lctSorted == 0);
  CHECK(frame->localData.lctSize == 0);
  CHECK(frame->localData.colorTable != NULL); // should be NULL ?
  // Extra data
  CHECK(frame->localData.disposalMethod == 0);
  CHECK(frame->localData.userInput == 0);
  CHECK(frame->localData.transparentFlag == true);
  CHECK(frame->localData.transparentColor == 16);
  CHECK(frame->localData.delayInMs == 100);
  // Payload
  CHECK(frame->localData.isInterlaced == 0);
  CHECK(frame->localData.lzwData == 0x08); // Minimum LZW code size for Image Data
  CHECK(frame->localData.payloadSize == 13);
  CHECK(frame->localData.payload != NULL);
  // Check payload
  // Note: payload contents payloadSize(0x0B) + 0x00 + payload 
  string payload_as_string = "";
  for( int i = 0; i < frame->localData.payloadSize; i++) {
    payload_as_string += 
         QbLzwDictionaryEntry::int_to_hex(frame->localData.payload[i], 2);
    payload_as_string += ",";
  }
  CHECK(payload_as_string == "0x0b,0x00,0x51,0xfc,0x1b,0x28,0x70,0xa0,0xc1,0x83,0x01,0x01,0x00,");

  for (auto frame: frameList) {
    delete frame;
  }

}

TEST_CASE( "factory from GifFile - test with a multi frame GIF file (red_green_blue.gif)", "[QbGifFrame]" ) {

  // NOTE :
  // ../data/red_green_blue.gif is a GIF animation built by
  // ../dataBuilder/dataBuilder.x
  // It contains 3 frames (220x150)displayed 1 second:
  // - frame 0 = red  : all pixels with color index = 0 (RGB = 255,0,0)
  // - frame 1 = green: all pixels with color index = 0 (RGB = 0,255,0)
  // - frame 2 = blue : all pixels with color index = 0 (RGB = 0,0,255)

  const char *inputFilename = "../data/red_green_blue.gif";
  ifstream inputStream(inputFilename, ifstream::binary);
  vector<QbGifFrame *> frameList = QbGifFrame::createGifFrames(inputStream);
  // close stream
  inputStream.close();

  REQUIRE(frameList.size() == 3);
  // Check all frame size
  for (auto frame: frameList) {
    CHECK(frame->globalData.width == 220);
    CHECK(frame->globalData.height == 150);
    CHECK(frame->localData.width == 220);
    CHECK(frame->localData.height == 150);
  }
  // Check LCT (local color table) for all frames
  for (auto frame: frameList) {
    // Red
    CHECK(frame->localData.colorTable[0] == 0xFF);
    CHECK(frame->localData.colorTable[1] == 0x00);
    CHECK(frame->localData.colorTable[2] == 0x00);
    // Green
    CHECK(frame->localData.colorTable[3] == 0x00);
    CHECK(frame->localData.colorTable[4] == 0xFF);
    CHECK(frame->localData.colorTable[5] == 0x00);
    // Blue
    CHECK(frame->localData.colorTable[6] == 0x00);
    CHECK(frame->localData.colorTable[7] == 0x00);
    CHECK(frame->localData.colorTable[8] == 0xFF);
  }
  
  for (auto frame: frameList) {
    delete frame;
  }
}

TEST_CASE( "Accessors", "[QbGifFrame]" ) {
  const char *inputFilename = "../data/red_green_blue.gif";
  ifstream inputStream(inputFilename, ifstream::binary);
  vector<QbGifFrame *> frameList = QbGifFrame::createGifFrames(inputStream);
  // close stream
  inputStream.close();

  REQUIRE(frameList.size() == 3);
  QbGifFrame *frame = frameList[0];
  // Delay
  CHECK(frame->getDelayInMs() == 1000);
  frame->setDelayInMs(3000);
  CHECK(frame->getDelayInMs() == 3000);
  // Global size
  CHECK(frame->getGlobalWidth() == 220);
  CHECK(frame->getGlobalHeight() == 150);

  for (auto frame: frameList) {
    delete frame;
  }
}

TEST_CASE( "writeGlobalData", "[QbGifFrame]" ) {
  stringstream outputStream;
  QbGifFrame::writeGlobalData(outputStream, 200, 100);

  // Checl GCT header
  __u_char global_data_header[] = {
    0x47,0x49,0x46,0x38,0x39,0x61, // Header (GIF89a)
    // GCT
    0xc8,0x00, // new width = 200
    0x64,0x00, // new height = 100
    0x00, // no color table
    0x00, // background color
    0x00, // aspect ratio
  };
  REQUIRE(outputStream.tellp() == 13);
  outputStream.seekg(0);
  for (int i = 0 ; i < 13; i++ ) {
    __u_char byte = QbStreamTools::readByte(outputStream);
    CHECK( (int)byte == (int)global_data_header[i]);
  }
}

TEST_CASE( "writeNetscape20Extension", "[QbGifFrame]" ) {
  stringstream outputStream;
  QbGifFrame::writeNetscape20Extension(outputStream, 100);

  __u_char netscape20Extension_ref[] = {
    0x21,0xff,0x0b,
    0x4E,0x45,0x54,0x53,0x43,0x41,0x50,0x45,0x32,0x2e,0x30, // NETSCAPE2.0
    0x03,0x01,
    0x64,0x00, // nb rep. = 100
    0x00
  };
  REQUIRE(outputStream.tellp() == 19);
  outputStream.seekg(0);
  for (int i = 0 ; i < 19; i++ ) {
    __u_char byte = QbStreamTools::readByte(outputStream);
    CHECK( (int)byte == (int)netscape20Extension_ref[i]);
  }
}

TEST_CASE( "writeLocalData", "[QbGifFrame]" ) {
  QbGifFrame frame;

  // initialize local data
  frame.localData.left = 20;
  frame.localData.top = 10;
  frame.localData.width = 200;
  frame.localData.height = 100;
  // LCT (Local Color Table)
  frame.localData.lctFlag = 1;
  frame.localData.lctSorted = 1;
  frame.localData.lctSize = 4;
  __u_char lct[] = { 
    0x00, 0x00, 0x00,
    0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff,
  };
  frame.localData.colorTable = (__u_char*)malloc(frame.localData.lctSize * 3);
  //memcpy()
  for (int i = 0; i < frame.localData.lctSize * 3; i++) {
    frame.localData.colorTable[i] = lct[i];
  }
  // Extra data
  frame.localData.disposalMethod == 1;
  frame.localData.userInput == 0;
  frame.localData.transparentFlag == 1;
  frame.localData.transparentColor == 0x04;
  frame.localData.delayInMs = 1000;
  // Payload
  frame.localData.isInterlaced = 1;
  frame.localData.lzwData = 0x08; // Minimum LZW code size for Image Data
  frame.localData.payloadSize = 11;
    // payload size + payload
  frame.localData.payload = (__u_char *)malloc(frame.localData.payloadSize + 1);
  frame.localData.payload[0] = frame.localData.payloadSize - 1;
  for (int i = 0 ; i < frame.localData.payloadSize; i++ ) {
    frame.localData.payload[i+1] = i;
  }

  stringstream outputStream;
  frame.writeLocalDataAndPayload(outputStream);

  __u_char localData_ref[] = {
    // -- Graphics Control Extension
    0x21, // Introducer
    0xf9, // Graphic Control Label
    0x04, // Bloc size
    0x04, // Packed
    0x64, 0x00, // delay ( = ms / 10 )
    0x00, // transparent color
    0x00, // Terminator
    // -- Local Image Descriptor
    0x2c, // separator
    0x14, 0x00, // left
    0x0a, 0x00, // top
    0xc8, 0x00, // width
    0x64, 0x00, // height
    0xe1, // packed
    // -- Color table
    0x00, 0x00, 0x00,
    0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff,
    //
    0x08, // Lzw data
    // payload size + payload
    0x0a,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
  };
  REQUIRE(outputStream.tellp() == 42);
  outputStream.seekg(0);
  for (int i = 0 ; i < sizeof(localData_ref); i++ ) {
    __u_char byte = QbStreamTools::readByte(outputStream);
    CHECK( (int)byte == (int)localData_ref[i]);
  }
}

/*  
private:
  void parseGlobalData(istream &stream);
  void readImageDescriptorAndPayload(istream &stream);
  void readGraphicControlExtension(istream &stream);
  static void skipExtension(istream &stream);
*/
