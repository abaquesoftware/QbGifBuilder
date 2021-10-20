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
  std::cout << "================= Data builder\n";
 
  const int bitmapWidth = 220;
  const int bitmapHeight = 150;

  // ************************************************
  // Build red_green_blue.gif
  // ************************************************

  const char *rgbFileName = "/tmp/red_green_blue.gif";

  // Build red, green and blue bitmaps
  QbGifBitmap bitmap_red(bitmapWidth, bitmapHeight);
  QbGifBitmap bitmap_green(bitmapWidth, bitmapHeight);
  QbGifBitmap bitmap_blue(bitmapWidth, bitmapHeight);
  for (int i = 0; i <= 0xFF; i++) {
    int colorIndex = i;
    int red   = (i == 0) ? 0xFF : 0x00;
    int green = (i == 1) ? 0xFF : 0x00;
    int blue  = (i == 2) ? 0xFF : 0x00;
    bitmap_red.setColor(colorIndex, red, green, blue);
    bitmap_green.setColor(colorIndex, red, green, blue);
    bitmap_blue.setColor(colorIndex, red, green, blue);
  }
  for (int x = 0; x < bitmapWidth; x++) {
    for (int y = 0; y < bitmapHeight; y++) {
      int colorIndex_red = 0;
      int colorIndex_green = 1;
      int colorIndex_blue = 2;
      bitmap_red.setPixel(x, y, colorIndex_red);
      bitmap_green.setPixel(x, y, colorIndex_green);
      bitmap_blue.setPixel(x, y, colorIndex_blue);
    }
  }

  // use QbGifBuilder
  QbGifBuilder builder_rgb;

  // Add bitmap
  builder_rgb.addFrame(&bitmap_red, 1000);
  builder_rgb.addFrame(&bitmap_green, 1000);
  builder_rgb.addFrame(&bitmap_blue, 1000);

  // Export
  ofstream rgbFile(rgbFileName, ofstream::binary);
  builder_rgb.exportGif(rgbFile);
  rgbFile.close();

  printf( "File : '%s' genererated.\n", rgbFileName);

  // ************************************************
  // Build wiki_example.gif
  // ************************************************

  const char * wikiFileName = "/tmp/wiki_example.gif";
  // Befor GCT color table
  const __u_char file_header[] = {
    0x47,0x49,0x46,0x38,0x39,0x61, // Header (GIF89a)
    // GCT
    0x03,0x00, // width
    0x05,0x00, // height
    0xf7, // color table type (0xf7 -> 256 col in RGB)
    0x00, // background color
    0x00, // aspect ratio
  };

  __u_char gctColorTable[256*3];
  int pos = 0;
  for(int i = 0; i < 256; i++) {
    gctColorTable[pos++] = i;
    gctColorTable[pos++] = i;
    gctColorTable[pos++] = i;
  }
  // force 0x28 to be black
  pos =  0x28 * 3;
  gctColorTable[pos++] = 0;
  gctColorTable[pos++] = 0;
  gctColorTable[pos++] = 0;

  // after GCT color table;
  const __u_char file_footer[] = {
    0x21,0xf9, // GC extension
    0x04, // GCE data size
    0x01, // transp. color
    0x00, 0x00, // delay

    0x10, // transp.col.number
    0x00, // end of GCE
    //
    0x2c, // image descriptor
    0x00,0x00,0x00,0x00, // image position
    0x03,0x00,0x05,0x00, // image width x height
    0x00, // no LCT
    0x08, // start of image
    0x0b, // nb bytes to dec. image
    0x00,0x51,0xfc,0x1b,0x28,0x70,0xa0,0xc1,0x83,0x01,0x01, // image data
    0x00, // end of block
    0x3b // file termination
  };

  ofstream wikiFile(wikiFileName, ofstream::binary);
  // header
  int header_len = sizeof(file_header);
  wikiFile.write((char *)file_header, header_len);
  // GCT color table
  int gctColorTable_len = 256 * 3;
  wikiFile.write((char *)gctColorTable, gctColorTable_len);
  // footer
  int footer_len = sizeof(file_footer);
  wikiFile.write((char *)file_footer, footer_len);
  wikiFile.close();
  
  printf( "File : '%s' genererated.\n", wikiFileName);

  // ************************************************
  // Build gradient.gif
  // ************************************************

  const char *gradientFileName = "/tmp/gradient.gif";

  // Build bitmap
  QbGifBitmap bitmap_gradient( bitmapWidth, bitmapHeight);
  for (int i = 0; i <= 0xFF; i++) {
    int colorIndex = i;
    int red = i;
    int green = 0xFF;
    int blue = i;
    bitmap_gradient.setColor( colorIndex , red , green, blue);
  }
  for (int x = 0; x < bitmapWidth; x++) {
    for (int y = 0; y < bitmapHeight; y++) {
      int colorIndex = x;
      if( x < 30 || y < 30 || x > 190 || y > 120 ) {
        colorIndex = bitmapWidth - x;
      }
      bitmap_gradient.setPixel(x, y, colorIndex);
    }
  }

  // use QbGifBuilder
  QbGifBuilder builder_gradient;

  // Add bitmap
  builder_gradient.addFrame(&bitmap_gradient);

  // Export
  ofstream gradientFile(gradientFileName, ofstream::binary);
  builder_gradient.exportGif(gradientFile);
  gradientFile.close();

  printf( "File : '%s' genererated.\n", gradientFileName);


  // ************************************************
  // Build animation.gif
  // ************************************************

  const char *animationFileName = "/tmp/animation.gif";
  const char * morphingFileName = "../data/morphing.gif";

  // Build bitmap
  QbGifBitmap bitmap_animation( bitmapWidth, bitmapHeight);
  for (int i = 0; i <= 0xFF; i++) {
    int colorIndex = i;
    int red = i;
    int green = 0xFF;
    int blue = i;
    bitmap_animation.setColor( colorIndex , red , green, blue);
  }
  for (int x = 0; x < bitmapWidth; x++) {
    for (int y = 0; y < bitmapHeight; y++) {
      int colorIndex = x;
      if( x < 30 || y < 30 || x > 190 || y > 120 ) {
        colorIndex = bitmapWidth - x;
      }
      bitmap_animation.setPixel(x, y, colorIndex);
    }
  }

  // use QbGifBuilder
  QbGifBuilder builder_animation;

  // Add bitmap
  builder_animation.addFrame(&bitmap_animation, 2000);

  // Add Gif animated file
  ifstream morphingFile( morphingFileName, ifstream::binary);
  // GIF inserted with the original framerate
  builder_animation.addFrames(morphingFile);
  // GIF inserted with a constant framerate of 20ms per frame
  morphingFile.seekg(morphingFile.beg);
  builder_animation.setDelayPolicy(QbGifBuilder::DelayPolicy::DP_PER_FRAME);
  builder_animation.addFrames(morphingFile, 20);
  // Note: 20 ms is the minimum delay.
  // If you specify a delay < 20ms, the frame delay will be forced to 20ms
  // 
  // GIF inserted with a framerate modified so that sequence will be played in 10s
  morphingFile.seekg(morphingFile.beg);
  builder_animation.setDelayPolicy(QbGifBuilder::DelayPolicy::DP_GLOBAL);
  builder_animation.addFrames(morphingFile, 10000);
  // close file stream
  morphingFile.close();

  // Export
  ofstream animationFile(animationFileName, ofstream::binary);
  builder_animation.exportGif(animationFile);
  animationFile.close();

  printf( "File : '%s' genererated.\n", animationFileName);

  // ************************************************
  // Ok !
  // ************************************************
  printf( "\n");
  printf( "If needed, you can move these files to QbGifBuilder/cplusplus/TDD/data/");
  printf( "\n");

  return 0;  
}
