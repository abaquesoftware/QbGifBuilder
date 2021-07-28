#ifndef _QBGIFFRAME_H_
#define _QBGIFFRAME_H_

#include <sys/types.h>
#include <vector>
#include <iostream>

#include "QbGifBitmap.h"

using namespace std;

class QbGifFrame {

  // globalData = Logical Screen Descriptor + Global Color Table
  struct GlobalData {
    int width;
    int height;
    int backgroundColor;
    __u_char aspectRatioAsByte;
    // GCT (Global Color Table)
    __u_char gctFlag;
    int gctInitalColorResolution;
    bool gctSorted;
    int gctSize;
    __u_char * colorTable;
  };

  // localData = Local Image Descriptor + Local Color Table + Image payload
  struct LocalData {
    int left;
    int top;
    int width;
    int height;
    // LCT (Local Color Table)
    bool lctFlag;
    bool lctSorted;
    int lctSize;
    __u_char * colorTable;      
    // Extra data
    int disposalMethod;
    bool userInput;
    bool transparentFlag;
    int transparentColor;
    int delayInMs;
    // Payload
    bool isInterlaced;
    int lzwData; // Minimum LZW code size for Image Data
    long payloadSize;
    __u_char * payload;
  };

  // private:
  public:
    GlobalData globalData;
    LocalData localData; 
    
  private:
    // CTOR
    QbGifFrame ();
    QbGifFrame (GlobalData *globalData);
  public:
    QbGifFrame (const QbGifBitmap *bitmap);
    // DTOR
    ~QbGifFrame ();

    // Factory
    static vector<QbGifFrame *> createGifFrames (istream &gifStream);

    void setDelayInMs (int delayInMs);
    int getDelayInMs ();

    int getGlobalWidth ();
    int getGlobalHeight ();

    static void writeGlobalData (ostream &stream, int width, int height);
    static void writeNetscape20Extension (ostream &stream, int nbRepetitions);
    void writeLocalData (ostream &stream);
    
  private:
    void parseGlobalData(istream &stream);
    void readImageDescriptorAndPayload(istream &stream);
    void readGraphicControlExtension(istream &stream);
    static void skipExtension(istream &stream);
    // debug
    void dumpGlobalData ();
    void dumpLocalData ();

};

#endif // _QBGIFFRAME_H_
