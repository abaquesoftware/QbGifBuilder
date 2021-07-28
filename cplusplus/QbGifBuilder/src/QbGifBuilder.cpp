#include "QbGifBuilder.h"

#include "QbGifFrame.h"
#include "QbStreamTools.h"

QbGifBuilder::QbGifBuilder() {
  // Default values
  this->defaultDelayInMs = 100;
  this->delayPolicy = IMAGE_ONLY;
  this->nbRepetitions = 65535;
}

QbGifBuilder::~QbGifBuilder() {
  for (auto frame : this->frameList) {
    delete(frame); 
  }
}

void QbGifBuilder::setDefaultDelayInMs (int delayInMs) {
  this->defaultDelayInMs = delayInMs;
}

void QbGifBuilder::setDelayPolicy (DelayPolicy policy) {
  this->delayPolicy = policy;
}

void QbGifBuilder::setNbRepetitions (int nbRepetitions) {
  if (nbRepetitions < 0) {
    nbRepetitions = 0;
  }
  if (nbRepetitions > 65535) {
    nbRepetitions = 0;
  }
  this->nbRepetitions = nbRepetitions;
}

void QbGifBuilder::addFrame(const QbGifBitmap *bitmap) {
  this->addFrame(bitmap, this->defaultDelayInMs);
}

void QbGifBuilder::addFrame(const QbGifBitmap *bitmap, int delayInMs) {
  QbGifFrame * frame = new QbGifFrame(bitmap);
  frame->setDelayInMs(delayInMs);
  this->frameList.push_back(frame);
}

void QbGifBuilder::addFrames(istream &stream) {
  this->addFrames(stream, this->defaultDelayInMs);
}

void QbGifBuilder::addFrames(istream &stream, int delayInMs) {
  vector<QbGifFrame *> frameList = QbGifFrame::createGifFrames(stream);
  // Apply delay policy
  if (frameList.size() > 0) {
  for (QbGifFrame * frame : frameList) {
      if (frame->getDelayInMs() == -1) {
        frame->setDelayInMs( delayInMs );              
      }      
    }

    switch ( this->delayPolicy) {

      case PER_FRAME: {
        for (QbGifFrame * frame : frameList) {
          frame->setDelayInMs(delayInMs);
        }
        break;
      }     

      case GLOBAL: {
        // Calculate total duration
        int totalDuration = 0;
        for (QbGifFrame * frame : frameList) {
            totalDuration += frame->getDelayInMs();
        }
        // Recalculate all durations
        for (auto frame : frameList) {
          int delay = delayInMs * frame->getDelayInMs() / totalDuration;
          frame->setDelayInMs(delay);
        }
        break;
      }

      case UNKNOWN:
      case IMAGE_ONLY:      
      default: {
        // do nothing
        break;
      }

    }
    // append new frame list to the QbGifBuilder
    for (QbGifFrame * frame : frameList) {
      this->frameList.push_back(frame);
    }
  }
}

void QbGifBuilder::exportGif(ostream &outputStream) { 
  if (this->frameList.size() == 0 ) {
    cout << "Cannot generate GIF. List is empty.\n";
    return;
  }

  // calculate maxWidth and minWidth
  int maxWidth = 0;
  int maxHeight = 0;
  for (auto frame : this->frameList) {
    if (frame->getGlobalWidth() > maxWidth) {
      maxWidth = frame->getGlobalWidth();
    }
    if (frame->getGlobalHeight() > maxHeight) {
      maxHeight = frame->getGlobalHeight();
    }
  }
  QbGifFrame::writeGlobalData(outputStream, maxWidth, maxHeight);
  QbGifFrame::writeNetscape20Extension(outputStream, this->nbRepetitions);
  // Insert frames
  for (auto frame : this->frameList) {
    frame->writeLocalData(outputStream);
  }
  // terminator
  QbStreamTools::writeByte(outputStream, 0x3b);
}
