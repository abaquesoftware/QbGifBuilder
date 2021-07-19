namespace QbGif {
  using System;
  using System.IO;

  public static class QbStreamTools {
    
    public static void WriteShort (Stream stream, int value) {
      stream.WriteByte((byte)(value & 0xff));
      stream.WriteByte((byte)((value >> 8) & 0xff));
    }
    public static int ReadShort (Stream stream) {
      int b1 = stream.ReadByte();
      int b2 = stream.ReadByte();
      return b1 + b2 * 256;
    }

    public static void WriteString (Stream stream, string value) {
      foreach( char c in value.ToCharArray()) {
        stream.WriteByte(Convert.ToByte(c));
      }
    }

  }
}
