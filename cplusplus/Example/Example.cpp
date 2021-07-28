#include <iostream>
#include <vector>
#include <fstream>


#include "QbGifBuilder.h"
#include "QbGifFrame.h"
#include "QbGifBitmap.h"

// Debug
#include "QbLzwDictionaryEntry.h"
#include "QbLzwDecoder.h"
#include "QbLzwEncoder.h"

using namespace std;

int main( int argc, char** argv) {
  std::cout << "================= Test\n";
 
  const int bitmapWidth = 220;
  const int bitmapHeight = 150;
  const char * morphingFileName = "./Example-files/morphing.gif";
  const char * outputFileName = "./animation.gif";

/*
  QbGifBuilder builder;
  ifstream testFile( "../../csharp/Sample/test.gif", ifstream::binary);
  builder.AddFrames(testFile);
  testFile.close();

  cout << "\n";
  cout << "\n";
  cout << "xxxxxxxxxxxxxxxxxxxx DECODE xxxxxxxxxxxxxxxxxxxxxxxx\n";
  cout << "\n";
  QbGifFrame * frame = builder.frameList[0];
  cout << "\n";
  for (int i = 0; i < frame->localData.payloadSize; i++) {
    cout << QbLzwDictionaryEntry::int_to_hex(frame->localData.payload[i],2) << ".";
  }
  cout << "\n";
  int iLen = frame->localData.payload[0];
  __u_char * payload = &(frame->localData.payload[1]);
  vector <__u_char> tmp = QbLzwDecoder::decode(5, payload, iLen);
  cout << "\n";
  cout << "\n";
  cout << "xxxxxxxxxxxxxxxxxxxx ENCODE xxxxxxxxxxxxxxxxxxxxxxxx\n";
  cout << "\n";
  __u_char * decodedArray = (__u_char *)malloc(tmp.size());
  for(int i = 0; i < tmp.size(); i++) {
    decodedArray[i] = tmp[i];
  }
  std::vector<u_char> res = QbLzwEncoder::encode(5, decodedArray, tmp.size());
*/

  // Build bitmap
  QbGifBitmap bitmap( bitmapWidth, bitmapHeight);
  for (int i = 0; i <= 0xFF; i++) {
    int colorIndex = i;
    int red = i;
    int green = 0xFF;
    int blue = i;
    bitmap.setColor( colorIndex , red , green, blue);
  }
  for (int x = 0; x < bitmapWidth; x++) {
    for (int y = 0; y < bitmapHeight; y++) {
      int colorIndex = x;
      if( x < 30 || y < 30 || x > 190 || y > 120 ) {
        colorIndex = bitmapWidth - x;
      }
      bitmap.setPixel(x, y, colorIndex);
    }
  }

/*
  QbGifBitmap bitmap( 3, 5);
  bitmap.setColor( 0x28 , 0 , 0, 0);
  bitmap.setColor( 0xFF , 255 , 255, 255);
  bitmap.setPixel(0, 0, 0x28);  bitmap.setPixel(1, 0, 0xFF);  bitmap.setPixel(2, 0, 0xFF); 
  bitmap.setPixel(0, 1, 0xFF);  bitmap.setPixel(1, 1, 0x28);  bitmap.setPixel(2, 1, 0xFF); 
  bitmap.setPixel(0, 2, 0xFF);  bitmap.setPixel(1, 2, 0xFF);  bitmap.setPixel(2, 2, 0xFF); 
  bitmap.setPixel(0, 3, 0xFF);  bitmap.setPixel(1, 3, 0xFF);  bitmap.setPixel(2, 3, 0xFF); 
  bitmap.setPixel(0, 4, 0xFF);  bitmap.setPixel(1, 4, 0xFF);  bitmap.setPixel(2, 4, 0xFF); 
*/

  // use QbGifBuilder
  QbGifBuilder builder;

  // Add bitmap
  builder.addFrame(&bitmap, 2000);

  // Add Gif animated file
  ifstream morphingFile( morphingFileName, ifstream::binary);
  // GIF inserted with the original framerate
  builder.addFrames(morphingFile);
  // GIF inserted with a constant framerate of 20ms per frame
  morphingFile.seekg(morphingFile.beg);
  builder.setDelayPolicy(QbGifBuilder::DelayPolicy::PER_FRAME);
  builder.addFrames(morphingFile, 20);
  // Note: 20 ms is the minimum delay.
  // If you specify a delay < 20ms, the frame delay will be forced to 20ms
  // 
  // GIF inserted with a framerate modified so that sequence will be played in 10s
  morphingFile.seekg(morphingFile.beg);
  builder.setDelayPolicy(QbGifBuilder::DelayPolicy::GLOBAL);
  builder.addFrames(morphingFile, 10000);
  // close file stream
  morphingFile.close();

  // Export
  ofstream outputFile( outputFileName, ofstream::binary);
  builder.exportGif(outputFile);
  outputFile.close();
}
