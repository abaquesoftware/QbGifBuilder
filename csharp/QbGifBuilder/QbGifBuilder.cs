namespace QbGif {
  using System;
  using System.IO;
  using System.Collections.Generic;
  using System.Drawing;
  using System.Drawing.Imaging;

  public enum QbGuidBuilderDelayPolicy {
    UNKNOWN, IMAGE_ONLY, GLOBAL, PER_FRAME
  }

  public class QbGifBuilder {

    private List<QbGifFrame> frameList;
    private QbGuidBuilderDelayPolicy delayPolicy = QbGuidBuilderDelayPolicy.IMAGE_ONLY;

    private int defaultDelayInMs = 100;

    private int nbRepetitions = 65535;

    // CTOR
    public QbGifBuilder() {
      frameList = new List<QbGifFrame>();
    }

    public void SetDefaultDelayInMs (int delayInMs) {
      this.defaultDelayInMs = delayInMs;
    }
    public void SetDelayPolicy (QbGuidBuilderDelayPolicy policy) {
      this.delayPolicy = policy;
    }

    public void SetNbRepetitions (int nbRepetitions) {
      if (nbRepetitions < 0) {
        nbRepetitions = 0;
      }
      if (nbRepetitions > 65535) {
        nbRepetitions = 0;
      }
      this.nbRepetitions = nbRepetitions;
    }

    public void AddFrame(Bitmap bitmap) {
      this.AddFrame( bitmap, this.defaultDelayInMs);
    }

    public void AddFrame(Bitmap bitmap, int delayInMs) {
      using (MemoryStream memStream = new MemoryStream()) {
        bitmap.Save(memStream, ImageFormat.Gif);
        memStream.Seek(0, SeekOrigin.Begin);
        List<QbGifFrame> frameList = QbGifFrame.CreateGifFrames(memStream);
        foreach (QbGifFrame frame in frameList) {
          frame.SetDelayInMs(delayInMs);
          this.frameList.Add(frame);
        }
      }
    }

    public void AddFrames(Stream stream) {
      AddFrames(stream, this.defaultDelayInMs);
    }

    public void AddFrames(Stream stream, int delayInMs) {
      List<QbGifFrame> frameList = QbGifFrame.CreateGifFrames(stream);
      // Apply delay policy
      if (frameList.Count > 0) {
        
        foreach (QbGifFrame frame in frameList) {
          if (frame.GetDelayInMs() == -1) {
            frame.SetDelayInMs( delayInMs );              
          }
        }

        switch ( this.delayPolicy) {

          case QbGuidBuilderDelayPolicy.PER_FRAME:
            foreach (QbGifFrame frame in frameList) {
              frame.SetDelayInMs(delayInMs);
            }
            break;              

          case QbGuidBuilderDelayPolicy.GLOBAL:
            // Calculate total duration
            int totalDuration = 0;
            foreach (QbGifFrame frame in frameList) {
              totalDuration += frame.GetDelayInMs();
            }
            // Recalculate all durations
            foreach (QbGifFrame frame in frameList) {
              int delay = delayInMs * frame.GetDelayInMs() / totalDuration;
              frame.SetDelayInMs(delay);
            }
            break;

          case QbGuidBuilderDelayPolicy.UNKNOWN:
          case QbGuidBuilderDelayPolicy.IMAGE_ONLY:
          default:
            // do nothing
            break;
      }

      // append new frame list to the QbGifBuilder
      foreach (QbGifFrame frame in frameList) {
          this.frameList.Add(frame);
        }
      }
    }

    public void Export(Stream outputStream) {
      
      if (this.frameList.Count == 0 ) {
        throw (new Exception("Cannot generate GIF. List is empty."));
      }

      // calculate maxWidth and minWidth
      int maxWidth = 0;
      int maxHeight = 0;
      foreach (QbGifFrame frame in this.frameList) {
        if (frame.GetGlobalWidth() > maxWidth) {
          maxWidth = frame.GetGlobalWidth();
        }
        if (frame.GetGlobalHeight() > maxHeight) {
          maxHeight = frame.GetGlobalHeight();
        }
      }
      QbGifFrame.WriteGlobalData(outputStream, maxWidth, maxHeight);
      QbGifFrame.WriteNetscape20Extension(outputStream, this.nbRepetitions);
      // Insert frames
      foreach (QbGifFrame frame in this.frameList) {
        frame.WriteLocalData(outputStream);
      }
      // terminator
      outputStream.WriteByte(0x3b);
    }

  }
}
