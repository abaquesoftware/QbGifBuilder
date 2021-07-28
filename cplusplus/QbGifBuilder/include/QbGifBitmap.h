#ifndef _QBGIFBITMAP_H_
#define _QBGIFBITMAP_H_

#include <sys/types.h>
#include <istream>
#include <vector>

using namespace std;

class QbGifBitmap {
  public:
    int left;
    int top;    
    int width;
    int height;
    int globalWidth;
    int globalHeight;
    int colorTableSize;
    __u_char * colorTable;
    __u_char * bitmap;

  public:
    // CTOR
    QbGifBitmap (int width, int height);
    QbGifBitmap (void * frame);
    // CTOR by copy to use vector<QbGifBitmap>
    QbGifBitmap (const QbGifBitmap &other);
    // DTOR
    ~QbGifBitmap ();

    // WARNING: EXPERIMENTAL
    // Factory - Convert GIF stream -> QbGifBitmap 
    static vector<QbGifBitmap> createQbGifBitmaps(istream &gifStream);


    void setColor( int index, __u_char red,  __u_char green,  __u_char blue);
    void setPixel( int x, int y, __u_char colorIndex);
};

#endif // _QBGIFBITMAP_H_
