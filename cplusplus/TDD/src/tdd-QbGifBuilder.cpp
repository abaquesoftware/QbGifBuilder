#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbGifBuilder.h"
#include "tdd-tools.inc.cpp"

using namespace std;
using namespace Catch::Matchers;

TEST_CASE( "constructor", "[QbGifBuilder]" ) {
  QbGifBuilder builder;

  CHECK(builder.frameList.size() == 0);
  CHECK(builder.defaultDelayInMs == 100);
  CHECK(builder.delayPolicy == QbGifBuilder::DP_IMAGE_ONLY);
  CHECK(builder.nbRepetitions == 0xFFFF);
}

TEST_CASE( "setters", "[QbGifBuilder]" ) {
  QbGifBuilder builder;

  builder.setDefaultDelayInMs(2000);
  builder.setDelayPolicy(QbGifBuilder::DP_GLOBAL);
  builder.setNbRepetitions(3);

  CHECK(builder.frameList.size() == 0);
  CHECK(builder.defaultDelayInMs == 2000);
  CHECK(builder.delayPolicy == QbGifBuilder::DP_GLOBAL);
  CHECK(builder.nbRepetitions == 3);
}

TEST_CASE( "addFrame (from bitmap)", "[QbGifBuilder]" ) {
  // Generate a GIF file with 3 frames (RGB)
  // and compare with red_green_blue.gif

  // NOTE :
  // ../data/red_green_blue.gif is a GIF animation built by
  // ../dataBuilder/dataBuilder.x
  // It contains 3 frames (220x150) displayed 1 second:
  // - frame 0 = red  : all pixels with color index = 0 (RGB = 255,0,0)
  // - frame 1 = green: all pixels with color index = 0 (RGB = 0,255,0)
  // - frame 2 = blue : all pixels with color index = 0 (RGB = 0,0,255)

  // -- Build red, green and blue bitmaps
  int bitmapWidth = 220;
  int bitmapHeight = 150;
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

  // -- Generate GIF file
  const char *rgbFileName = "/tmp/red_green_blue.gif";
  QbGifBuilder builder_rgb;
  builder_rgb.setDefaultDelayInMs(1000);
  // Add bitmap
  builder_rgb.addFrame(&bitmap_red);
  builder_rgb.addFrame(&bitmap_green);
  builder_rgb.addFrame(&bitmap_blue);
  // Export
  ofstream rgbFile(rgbFileName, ofstream::binary);
  builder_rgb.exportGif(rgbFile);
  rgbFile.close();

  // -- Compare /tmp/red_green_blue.gif and
  //    ../data/red_green_blue.gif
  const char *rgbFileName_ref = "../data/red_green_blue.gif";
  CHECK( are_files_identical(rgbFileName, rgbFileName_ref) );
}

TEST_CASE( "addFrame (from GIF files)", "[QbGifBuilder]" ) {
  // Generate a GIF file (/tmp/animation.gif) with:
  // - 1 bitmap (gradient)
  // - 1 sequence (extracted from data/morphing.gif) with initial frame rate
  // - 1 sequence (extracted from data/morphing.gif) with 20 ms / frame
  // - 1 sequence (extracted from data/morphing.gif) with 10 seconds for the 
  //   whole sequence
  //
  //  Then, /tmp/animation.gif is compared with data/animation.gif

  const char *animationFileName = "/tmp/animation.gif";
  const char * morphingFileName = "../data/morphing.gif";

  // Build bitmap
  int bitmapWidth = 220;
  int bitmapHeight = 150;
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

  // -- Compare /tmp/red_green_blue.gif and
  //    ../data/red_green_blue.gif
  const char *animationFileName_ref = "../data/animation.gif";
  CHECK( are_files_identical(animationFileName, animationFileName_ref) );
}
