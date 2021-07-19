# QbGifBuilder for C#

Example of GIF file generated bi QbGifBuilder.  
![Example](https://abaquesoftware.com/projects/QbToolbox/QbGifBuilder.gif)

## API presentation

This API generates animated GIF files. It is based on GDI+ library.   
A GIF file can be built from BITMAP or from stream pointing to a GIF file (static or animated).  
As a consequence, the generated GIF bitmap is a set of frame list. Each frame list can be:  
- a 'code-generated' image (via the Bitmap GDI+ API),
- an image file (bmp, gif, png, ...) converted into a BITMAP,
- a GIF animated file.
For each framelist, one can specifies the display delay (in ms).  
Once the GIF is defined, it can be exported to a stream (usually a file stream).  

## Installation, configuration

**Requirements for Linux and MacOS**  

Debian/Ubuntu : apt install libgdiplus  
RedHat/CentOs: yum install libgdiplus  
MacOS: brew install mono-libgdiplus  

**File copy**

In your project, copy the directory named 'QbGifBuilder' from this Git repository.  

**csproj file configuration**  

In the file < your-project >.csproj, add the following lines:  

        <ItemGroup>
          <PackageReference Include="System.Drawing.Common" Version="5.0.2" />
        </ItemGroup>
        <ItemGroup>
          <Compile Include="<your-project-path>/QbGifBuilder/*.cs" />
        </ItemGroup>



## QbGif.QbGifBuilder

QbGifBuilder provides the following methods:  

  **QbGifBuilder()**  
  Constructor    
  
  **void AddFrame(Bitmap bitmap)**  
  **void AddFrame(Bitmap bitmap, int delayInMs)**  
  Add a frame as a Bitmap.  
  
  **void AddFrames(Stream stream)**  
  **void AddFrames(Stream stream, int delayInMs)**  
  Add one or several frames from a stream.  
  The stream MUST contain a GIF image (animated or not).  
  
  **void Export(Stream outputStream)**  
  Generate the GIF and write it to outpoutStream.  
  
  **void SetDefaultDelayInMs (int delayInMs)**
  Set the default frame delay (if it is not provided).  
  
  **SetNbRepetitions (int nbRepetitions)**  
  Set the number of repetitions. (must be between 0 and 65535).  
  
  **SetDelayPolicy (QbGuidBuilderDelayPolicy policy)**  
  Set the policy to compute frame delays when adding animated GIF.  
  QbGuidBuilderDelayPolicy.IMAGE_ONLY:  
    Frame delay value will only be applied on image and animated GIF frame delays won't be modified (default).
  QbGuidBuilderDelayPolicy.PER_FRAME:  
    The same frame delay will be applied to all frames (images or frames from animated GIF)  
  QbGuidBuilderDelayPolicy.GLOBAL:  
    The delay will be applied to the entire animation.  
  

  Notes:
  - If SetDefaultDelayInMs or SetDelayPolicy is called, the change will be only applied to the next append frames.  
  - frame delays must be > 20 ms and < 655350 ms.   
    If a frame delay = 0 -> it will be set to 100 ms  
    If a frame delay is > 0 and < 20 ms -> it will be set to 20 ms  
    If a frame delay is > 655360 ms -> it will be set to 655360 ms   
  

## Example:

  An example is available in Sample/program.cs  
