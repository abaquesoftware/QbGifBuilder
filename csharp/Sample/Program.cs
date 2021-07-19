using System;
using System.Drawing.Imaging;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;

using QbGif;

class Program {

  
  const string logoFilename = "./Sample-files/abaqueSoftware.png";

  const string morphingFilename = "./Sample-files/morphing.gif";
  const string outputFilename = "./animation.gif";

  static void Main(string[] args) {
    QbGifBuilder builder = new QbGifBuilder();

    // - - - - - - - - - - - - - - -
    // Add cover (generated bitmap)
    // - - - - - - - - - - - - - - -
    // Generate a bitmap
    Bitmap cover = new Bitmap(220, 150);
    Graphics g = Graphics.FromImage(cover);
    g.FillRectangle(Brushes.White, new Rectangle(0, 0, 220, 150));      
    g.DrawString("QbGifBuilder", new Font("Tahoma",24), Brushes.Black, new RectangleF(5, 50, 210, 50));
    g.DrawRectangle(new Pen(Brushes.Green), new Rectangle(5, 5, 210, 38));
    g.DrawRectangle(new Pen(Brushes.Green), new Rectangle(6, 6, 208, 36));
    g.DrawRectangle(new Pen(Brushes.Green), new Rectangle(5, 100, 210, 45));
    g.DrawRectangle(new Pen(Brushes.Green), new Rectangle(6, 101, 208, 43));
    g.Flush();
    // Add bitmap as frame with a delay of 3s (3000 ms)
    builder.AddFrame(cover, 2000);

    // - - - - - - - - - - - -
    // Add PNG image
    // - - - - - - - - - - - -
    Bitmap logo = new Bitmap(logoFilename);
    // Add bitmap as frame with a delay of 3s (2000 ms)
    builder.AddFrame(logo, 3000);

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Add an animated GIF with different delayPolicies
    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    // Note the original GIF (Sample-files/morphing.gif) contains 52 frames
    // displayed with a delay of 100 ms per frame (total lasts 5.2 seconds) 

    using( var fileStream = new FileStream(morphingFilename, FileMode.Open)) {
      // GIF inserted with the original framerate
      builder.SetDelayPolicy(QbGuidBuilderDelayPolicy.IMAGE_ONLY);
      builder.AddFrames(fileStream);
      
      // GIF inserted with a constant framerate of 20ms per frame
      fileStream.Seek(0,SeekOrigin.Begin);
      builder.SetDelayPolicy(QbGuidBuilderDelayPolicy.PER_FRAME);
      builder.AddFrames(fileStream, 20);
      // Note: 20 ms is the minimum delay.
      // If you specify a delay < 20ms, the frame delay will be forced to 20ms

      // GIF inserted with a framerate modified so that sequence will be played in 10s
      fileStream.Seek(0,SeekOrigin.Begin);
      builder.SetDelayPolicy(QbGuidBuilderDelayPolicy.GLOBAL);
      builder.AddFrames(fileStream, 10000);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Export
    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // The total animation (cover + logo + 3 x morphing) will be displayed 3 times
    // (1 display + 2 repetition) 
    builder.SetNbRepetitions(2);

    using (var stream = new FileStream(outputFilename, FileMode.Create)) {
      builder.Export(stream);
      System.Console.WriteLine("Generated GIF stored in '" + outputFilename + "'");
      System.Console.WriteLine("Warning: The application 'Windows Photos - version 2019' won't display this file correctly (v.2020 works).");
    }

  }
}
