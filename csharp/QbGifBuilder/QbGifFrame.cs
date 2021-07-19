namespace QbGif {
  using System;
  using System.IO;
  using System.Collections.Generic;

  // Source : https://www.w3.org/Graphics/GIF/spec-gif89a.txt

  // globalData = Logical Screen Descriptor + Global Color Table
  struct GlobalData {
    public int width;
    public int height;
    public int backgroundColor;
    public byte aspectRatioAsByte;
    // GCT (Global Color Table)
    public bool gctFlag;
    public int gctInitalColorResolution;
    public bool gctSorted;
    public int gctSize;
    public (int, int, int)[] colorTable;
  }

  // localData = Local Image Descriptor + Local Color Table + Image payload
  struct LocalData {
    public int left;
    public int top;
    public int width;
    public int height;
    // LCT (Local Color Table)
    public bool lctFlag;
    public bool lctSorted;
    public int lctSize;
    public (int, int, int)[] colorTable;      
    // Extra data
    public int disposalMethod;
    public bool userInput;
    public bool transparentFlag;
    public int transparentColor;
    public int delayInMs;
    // Payload
    public bool isInterlaced;
    public int lzwData; // Minimum LZW code size for Image Data
    public byte[] payload;
  }

  public class QbGifFrame {
    private const String GIF87A = "GIF87a";
    private const String GIF89A = "GIF89a";

    private GlobalData globalData;
    private LocalData localData; 
    
    // CTOR
    private QbGifFrame (GlobalData globalData, LocalData localData) {
      this.globalData = globalData;
      this.localData = localData;
    }

    public void SetDelayInMs (int delayInMs) {
      this.localData.delayInMs = delayInMs;
      if (delayInMs <= 0) {
        this.localData.delayInMs = 100;
      }
    }

    public int GetDelayInMs () {
      return this.localData.delayInMs;
    }

    public int GetGlobalWidth () {
      return this.globalData.width;
    }
    public int GetGlobalHeight () {
      return this.globalData.height;
    }

    // Factory
    static public List<QbGifFrame> CreateGifFrames (Stream gifStream) {
      List<QbGifFrame> result = new List<QbGifFrame>();
      // Read signature + version (must be "GIF87a" or "GIF89a" )
      int len = 6; // length of "GIF87a" or "GIF89a"
      string signatureAndVersion = "";
      for( int i = 0 ; i < len ; i++) {
        int b = gifStream.ReadByte();
        signatureAndVersion += Convert.ToChar(b);
      }
      if ( !signatureAndVersion.Equals(GIF87A) && !signatureAndVersion.Equals(GIF89A)) {
          throw( new Exception("Unsupported GIF format"));        
      }
      // global data
      GlobalData globalData = new GlobalData();
      QbGifFrame.ParseGlobalData(gifStream, ref globalData);
      LocalData localData = new LocalData();
      QbGifFrame.InitializeLocalData(ref localData);
      
      // main loop
      bool exit = false;
      while (!exit) {
        int firstByte = gifStream.ReadByte();
        switch ( firstByte ) {
          case 0x2c:
            // Parse frame
            QbGifFrame.ReadImageDescriptorAndPayload(gifStream, ref localData);
            QbGifFrame newFrame = new QbGifFrame(globalData, localData);
            result.Add(newFrame);
            localData = new LocalData();
            QbGifFrame.InitializeLocalData(ref localData);

            break;
          case 0x21:
            int secondByte = gifStream.ReadByte();
            if( secondByte == 0xf9) {
              QbGifFrame.ReadGraphicControlExtension(gifStream, ref localData);
            } else {
              QbGifFrame.SkipExtension(gifStream);
            }
            break;
          case 0x3b:
          case -1:
            exit = true;
            break;
          default:
            throw( new Exception("Parsing error. Unknown descriptor (" + String.Format("{0:X}", firstByte) + ")."));
        }
      }
      return result;
    }

    public static void WriteGlobalData (Stream stream, int width, int height) {
      byte globalPacked = 0x00; // No GCT
      byte backgroundColor = 0x00;
      byte aspectRatioAsByte = 0x00;
      QbStreamTools.WriteString(stream, "GIF89a");
      QbStreamTools.WriteShort(stream, width);
      QbStreamTools.WriteShort(stream, height);
      stream.WriteByte(globalPacked);
      stream.WriteByte(backgroundColor);
      stream.WriteByte(aspectRatioAsByte);
   }

    public static void WriteNetscape20Extension (Stream stream, int nbRepetitions) {
      // Application extension
      stream.WriteByte(0x21);
      stream.WriteByte(0xFF);
      stream.WriteByte(0x0B);
      QbStreamTools.WriteString(stream, "NETSCAPE2.0");
      stream.WriteByte(0x03);
      stream.WriteByte(0x01);
      QbStreamTools.WriteShort(stream, nbRepetitions);
      stream.WriteByte(0x00);
    }

    public void WriteLocalData (Stream stream) {

      //   .............................
      //   Graphics Control Extension
      //   .............................
      //   1 byte  - Introducer (0x21)
      //   1 byte  - Label = Graphic Control Label (0xF9) 
      //   1 byte  - BlockSize (0x04) 
      //   1 byte  - Packed (Method of graphics disposal)
      //   2 bytes - Delay ( = ms / 10 )
      //   1 byte  - Transparent Color
      //   1 byte  - Terminator (0x00) 
      //
      //   packed
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   --- reserved ---    | Disposal method       |UserInp|Tr.col.|
      //   +-------+-------+-------+-------+-------+-----------------------+
      
      // Build packet
      byte packed = (byte)(   (this.localData.disposalMethod & 0x0F) << 2 
                            | (this.localData.userInput?1:0) << 1
                            | (this.localData.transparentFlag?1:0) ); 
      stream.WriteByte(0x21);
      stream.WriteByte(0xF9);
      stream.WriteByte(0x04);
      stream.WriteByte(packed);
      int delay = this.localData.delayInMs / 10;
      if (delay < 2 ) delay = 2;
      if (delay > 65535) delay = 65535;
      QbStreamTools.WriteShort(stream, delay);
      stream.WriteByte((byte)this.localData.transparentColor);
      stream.WriteByte(0x00);

      //   ................
      //   Local Image Descriptor
      //   ................
      //   (already read) -> 1 byte  - separator (0x2c)
      //   2 bytes - left (short)
      //   2 bytes - top (short)
      //   2 bytes - width (short)
      //   2 bytes - heigth (short)
      //   1 byte  - packed = image and color LOCAL table information
      //
      //   packed byte:
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   | LCT ? |Interla| Sort  | -- Reserved - |n=tableSizeIndicator(*)|
      //   +-------+-------+-------+-------+-------+-----------------------+
      //
      stream.WriteByte(0x2C);
      QbStreamTools.WriteShort(stream, this.localData.left);
      QbStreamTools.WriteShort(stream, this.localData.top);
      QbStreamTools.WriteShort(stream, this.localData.width);
      QbStreamTools.WriteShort(stream, this.localData.height);
      // Build packed byte
      bool flag = this.localData.lctFlag;
      (int, int, int)[] colorTable = this.localData.colorTable;
      bool sorted = this.localData.lctSorted;      
      if (!flag) {
        flag = this.globalData.gctFlag;
        colorTable = this.globalData.colorTable;
        sorted = this.globalData.gctSorted;
      }
      packed = (byte) (
               ( (flag?1:0) << 7 )
             | ( (this.localData.isInterlaced?1:0) << 6)
             | ( (sorted?1:0) << 5)
             | ( (byte)(Math.Log2(colorTable.Length)-1) & 0x07)
      );
      stream.WriteByte(packed);

      // Color table
      foreach ((byte,byte,byte) color in colorTable) {          
        stream.WriteByte(color.Item1);
        stream.WriteByte(color.Item2);
        stream.WriteByte(color.Item3);
      }

      // lzw data
      stream.WriteByte((byte)this.localData.lzwData);

      // payload
      foreach (byte b in this.localData.payload) {
        stream.WriteByte(b);
      }
    }

    // ********************** private *******************

    private static void InitializeLocalData( ref LocalData localData) {
      localData.delayInMs = -1;
      localData.disposalMethod = 1;
      // TODO : read this parameters from stream
      localData.userInput = false;
      localData.transparentFlag = false;
      localData.transparentColor = 0xFF;
    }
    private static void ParseGlobalData( Stream stream, ref GlobalData globalData) {
      //   ................      
      //   Logical Screen Descriptor
      //   ................      
      //   2 bytes - width (short)
      //   2 bytes - height (short)
      //   1 byte  - Packed = screen and color GLOBAL table informations
      //   1 byte  - background color
      //   1 byte  - a = aspectRatioIndicator(*)
      //   (*) Aspect ratio = ( a + 15 ) / 64

      globalData.width = QbStreamTools.ReadShort(stream);
      globalData.height = QbStreamTools.ReadShort(stream);
      int packed = stream.ReadByte();
      globalData.backgroundColor = stream.ReadByte();
      globalData.aspectRatioAsByte = (byte)stream.ReadByte();

      //   ................
      //   packed (GLOBAL)
      //   ................
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   | GCT ? |   Color resolution    |Sorted |n=tableSizeIndicator(*)|
      //   +-------+-------+-------+-------+-------+-----------------------+
      //   (*) Table size = 2 ^ (n+1)   (= nb of colors in the table)
      //
      globalData.gctFlag = ( (packed & 0x80) != 0 );
      globalData.gctInitalColorResolution = (packed & 0x70) >> 4;
      if (globalData.gctFlag) {
        globalData.gctSorted = ( ( (packed & 0x08) >> 3 ) != 0 );
        globalData.gctSize =  1 << ( ( packed & 0x07 ) + 1);
      } else {
        globalData.gctSorted = false;
        globalData.gctSize = 0;
      }

      //   .....................
      //   Global Color Table
      //   .....................
      //   for each color:
      //      1 byte - red
      //      1 byte - green
      //      1 byte - blue
      //
      globalData.colorTable = new (int, int, int)[globalData.gctSize];
      for (int i = 0; i < globalData.gctSize; i++) {
        globalData.colorTable[i] = (stream.ReadByte(),stream.ReadByte(),stream.ReadByte());
      }
    }

    private static void ReadImageDescriptorAndPayload(Stream stream, ref LocalData localData) {
      //   ................
      //   Local Image Descriptor
      //   ................
      //   (already read) -> 1 byte  - separator ( 0x2c )
      //   2 bytes - left (short)
      //   2 bytes - top (short)
      //   2 bytes - width (short)
      //   2 bytes - heigth (short)
      //   1 byte  - packed = image and color LOCAL table information
      //
      localData.left = QbStreamTools.ReadShort(stream);
      localData.top = QbStreamTools.ReadShort(stream);
      localData.width = QbStreamTools.ReadShort(stream);
      localData.height = QbStreamTools.ReadShort(stream);
      int packed = stream.ReadByte();

      //   ................
      //   packed (LOCAL)
      //   ................
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   | LCT ? |Interla| Sort  | -- Reserved - |n=tableSizeIndicator(*)|
      //   +-------+-------+-------+-------+-------+-----------------------+
      //

      localData.lctFlag = ( (packed & 0x80) != 0 );
      localData.isInterlaced = ( ( (packed & 0x40) >> 6) != 0);
      if (localData.lctFlag) {
        localData.lctSorted = ( ( (packed & 0x20) >> 5 ) != 0 );
        localData.lctSize =  1 << ( ( packed & 0x07 ) + 1);
      } else {
        localData.lctSorted = false;
        localData.lctSize =  0;
      }

      //   .....................
      //   Local Color Table
      //   .....................
      //   for each color:
      //      1 byte - red
      //      1 byte - green
      //      1 byte - blue
      //
      localData.colorTable = new (int, int, int)[localData.lctSize];
      for (int i = 0; i < localData.lctSize; i++) {
        localData.colorTable[i] = (stream.ReadByte(),stream.ReadByte(),stream.ReadByte());
      }

      localData.lzwData = stream.ReadByte();

      // payload 
      long payloadPos = stream.Position;
      int payloadSize = 0;

      // calculate payload size (including lzwData)
      int blocSize = 1;
      while (blocSize > 0) {
        blocSize = stream.ReadByte();
        payloadSize++;
        for (int i = 0; i < blocSize; i++) {
          stream.ReadByte();
          payloadSize++;
        }
      }

      // read payload size
      stream.Seek(payloadPos, SeekOrigin.Begin);
      localData.payload = new byte[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        localData.payload[i] = (byte)stream.ReadByte();
      }
    }

    private static void ReadGraphicControlExtension(Stream stream, ref LocalData localData) {
      //   .....................
      //   Extension
      //   .....................
      //   (already read) -> 1 byte  - separator ( 0x21 )
      //   (already read) -> 1 byte  - extension type (0xf9)
      //   1 byte  - bloc size (0x04)
      //   1 byte  - packed
      //   2 bytes - frame delay (in hundreds of second)
      //   1 byte  - transparent color
      //   1 byte  - terminator (0x00)
      //
      //   Packed:
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
      //   +-------+-------+-------+-------+-------+-------+-------+-------+
      //   |   --- reserved ---    | Disposal method       |UserInp|Tr.col.|
      //   +-------+-------+-------+-------+-------+-----------------------+
     
      stream.ReadByte(); // bloc size (0x04)
      int packed = stream.ReadByte();
      localData.delayInMs = QbStreamTools.ReadShort(stream) * 10;
      if (localData.delayInMs <= 0) {
        localData.delayInMs = 100;
      }
      localData.transparentColor = stream.ReadByte();
      stream.ReadByte(); // terminator (0x00)

      // Parse packed byte
      localData.disposalMethod = (byte)( (packed >> 2) & 0x0F );
      localData.userInput = (packed & 0x02) != 0;
      localData.transparentFlag = (packed & 0x01) != 0;
    }

    private static void SkipExtension(Stream stream) {
      //   .....................
      //   Extension
      //   .....................
      //   (already read) -> 1 byte  - separator ( 0x21 )
      //   (already read) -> 1 byte  - extension type
      //   1 byte  - bloc size
      //   ... bloc
      //   1 byte  - bloc size
      //   ... bloc
      //       ...
      //   1 byte  - terminator (0x00)
      //
      int blocSize = 1;
      while (blocSize > 0) {
        blocSize = stream.ReadByte();
        stream.Seek(blocSize, SeekOrigin.Current);
      }
    }

  }
}