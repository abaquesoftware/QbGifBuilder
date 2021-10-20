#include <fstream>

// Qt
#include <QtGui>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>

// QbGifBuilder
#include "QbGifBuilder.h"
#include "QbGifBitmap.h"

// debug
#include <iostream>

QbGifBitmap * buildQbGifBitmapFromQImage(const QImage &image) {
  QImage tmpImage = image.convertToFormat(QImage::Format_Indexed8);
  int width = tmpImage.width();
  int height = tmpImage.height();
  QPixmap tmpPixmap = QPixmap::fromImage(tmpImage);
  QbGifBitmap * result = new QbGifBitmap(width, height);
  // Copy color table
  QVector<QRgb> colorTable = tmpImage.colorTable();
  int pos = 0;
  for (QRgb color: colorTable) {
    result->colorTable[pos++] = qRed(color);
    result->colorTable[pos++] = qGreen(color);
    result->colorTable[pos++] = qBlue(color);
  }
  // Copy image definition (bitmap)
  pos = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      result->bitmap[pos++] = tmpImage.pixelIndex(x, y);
    }
  }  
  return result;
}

int main(int argc, char **argv) {
  QTextStream(stdout) << "===== Build GIF File =====" << endl;

  const char *logoFilename = "./Example-files/abaqueSoftware.png";
  const char *morphingFilename = "./Example-files/morphing.gif";
  const char *outputFilename = "./animation.gif";


  QApplication app(argc, argv);

  // ---------------------
  // Build GIF image
  // ---------------------

  QbGifBuilder builder;

  // Add computed image
  QPixmap coverPixmap(220,150);
  QImage coverImg = coverPixmap.toImage();
  QPainter painter(&coverImg);
  painter.setBrush(QBrush(QColor(255,250,205))); // lemonchiffon
  painter.drawRect(0,0,219,149);
  painter.setBrush(Qt::NoBrush);
  painter.setPen(Qt::GlobalColor::black);
  painter.setFont(QFont("Tahoma",24));
  painter.drawText(5,50,210,50,0,"QbGifBuilder");
  painter.setPen(Qt::GlobalColor::darkGreen);
  painter.drawRect(0,0,219,149);
  painter.drawRect(5, 5, 210, 38);
  painter.drawRect(6, 6, 208, 36);
  painter.drawRect(5, 100, 210, 45);
  painter.drawRect(6, 101, 208, 43);
  // Add comupted image as a frame
  QbGifBitmap * cover = buildQbGifBitmapFromQImage(coverImg);
  builder.addFrame(cover, 2000);
  delete (cover);
  
  // Add logo from PNG file 3 s (3000 ms)
  QImage logoImg(logoFilename);
  QbGifBitmap *logo = buildQbGifBitmapFromQImage(logoImg);
  builder.addFrame(logo, 3000);
  delete (logo);

  // Add Gif animated file
  ifstream morphingGifStream(morphingFilename, ifstream::binary);
  // GIF inserted with the original framerate
  builder.addFrames(morphingGifStream);
  // GIF inserted with a constant framerate of 20ms per frame
  morphingGifStream.seekg(morphingGifStream.beg);
  builder.setDelayPolicy(QbGifBuilder::DelayPolicy::DP_PER_FRAME);
  builder.addFrames(morphingGifStream, 20);
  // Note: 20 ms is the minimum delay.
  // If you specify a delay < 20ms, the frame delay will be forced to 20ms
  // 
  // GIF inserted with a framerate modified so that sequence will be played in 10s
  morphingGifStream.seekg(morphingGifStream.beg);
  builder.setDelayPolicy(QbGifBuilder::DelayPolicy::DP_GLOBAL);
  builder.addFrames(morphingGifStream, 10000);
  // close the GIF file stream
  morphingGifStream.close();

  // Export GIF file
  ofstream output( "./animation.gif", ofstream::binary);
  builder.exportGif(output);
  output.close();

  // Build window
  // QImage animatedImage = QImage("./animation.gif");
  QMovie movie("./animation.gif");
  QWidget window;
  window.resize(250, 180);
  window.setWindowTitle("QbGifBuilder");
  QLabel label;
  label.setMovie(&movie);
  QVBoxLayout layout;
  layout.addWidget(&label);
  window.setLayout(&layout);
  window.show();
  movie.start();

  return app.exec();
}
