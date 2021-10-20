#ifndef _QBGIFBUILDER_H_
#define _QBGIFBUILDER_H_

#include <sys/types.h>
#include <vector>
#include <istream>

#include "./QbGifFrame.h"
#include "./QbGifBitmap.h"

using namespace std;


class QbGifBuilder {

  public:
    enum DelayPolicy { DP_UNKNOWN, DP_IMAGE_ONLY, DP_GLOBAL, DP_PER_FRAME };

#ifdef TDD_MODE
  public:
#else
  private:
#endif
    vector<QbGifFrame *> frameList;
    DelayPolicy delayPolicy;

    int defaultDelayInMs;
    int nbRepetitions;
  
  public:
    // CTOR
    QbGifBuilder();
    ~QbGifBuilder();

    void setDefaultDelayInMs (int delayInMs);    
    void setDelayPolicy (DelayPolicy policy);
    void setNbRepetitions (int nbRepetitions);

    void addFrame(const QbGifBitmap *bitmap);
    void addFrame(const QbGifBitmap *bitmap, int delayInMs);
    void addFrames(istream &gifStream);
    void addFrames(istream &gifStream, int delayInMs);

    void exportGif(ostream &outputStream);
};

#endif // _QBGIFBUILDER_H_

