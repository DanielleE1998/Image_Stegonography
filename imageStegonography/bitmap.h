#ifndef BITMAP_H
#define BITMAP_H
#include <vector>
#include <string>
using namespace std;
extern bool debug;

const int BI_RGB  = 0; // no compression  
const int BI_RLE8 = 1; // 8bit RLE encoding  
const int BI_RLE4 = 2; // 4bit RLE encoding


// ----------------------------------------------------------------------------
//bit map header (14 bytes)
/*
Signature	2 bytes	0000h	'BM'
FileSize	4 bytes	0002h	File size in bytes
reserved	4 bytes	0006h	unused (=0)
DataOffset	4 bytes	000Ah	Offset from beginning of file to the beginning of the bitmap data
*/
#pragma pack(2) // Add this to pack header so that it is not rounded to multiples of 4
typedef struct //14 byte header
{   
    char signature[2];          // 2 bytes should be 'BM'   
	unsigned int file_size;     // 4 bytes file size in bytes
	unsigned short int reserve1;          // 2 bytes reserved
	unsigned short int reserve2;          // 2 bytesreserved
	unsigned int bmp_offset;    // 4 bytes Offset from beginning of file to the beginning of the bitmap data
}bmpfile_header;

//bmp info header (40 bytes)
/*
Size	4 bytes	000Eh	Size of InfoHeader =40 
Width	4 bytes	0012h	Horizontal width of bitmap in pixels
Height	4 bytes	0016h	Vertical height of bitmap in pixels
Planes	2 bytes	001Ah	Number of Planes (=1)
Bits Per Pixel	2 bytes	001Ch
Bits per Pixel used to store palette entry information. 
This also identifies in an indirect way the number of possible colors. Possible values are:
*/
//#pragma pack() 
typedef struct
{
  unsigned int header_size;     //Size of InfoHeader = 40
  signed int width;
  signed int height;
  
  unsigned short int num_planes; // 2 bytes or 16 bits
  unsigned short int bits_per_pixel;
  unsigned int compression; // ImaSte not supporting for compressed images
  unsigned int bmp_image_size; // in bytes
  
  signed int hres; // horizontal resolution
  signed int vres; // vertical resolution
  unsigned int num_colors;
  unsigned int num_important_colors;
  
} bmp_info_header; 

// ----------------------------------------------------------------------------
/**
 * Rsingle Pixel in the image. With  red, green, and blue components 
 * Each  values can ranges from 0 to 255
**/
class Pixel
{
public:
	int red, green, blue; //// individual color components.

	Pixel() : red(0), green(0), blue(0) { } // Initializes a Pixel with black color.
	Pixel(int r, int g, int b) : red(r), green(g), blue(b) { } // Initializes Pixel with the given RGB values.
};

// ----------------------------------------------------------------------------
/**
 * Represents a bitmap where a grid of pixels (in row-major order)
 * describes the color of each pixel within the image. Limited to Windows BMP
 * formatted images with no compression and 24 bit color depth.
**/

class Bitmap
{
 public:   
    Bitmap();
    ~Bitmap();
    
    bmpfile_header fh; //  headers
    bmp_info_header ih;

//methods:
  public:
    
    /*
    Opens a file associated with bitmap class file name
    validates the headers and and reads pixel the colors
    into a buffer
    Errors are reported and a -1 returns
    if success a 0 is returned
    */
    int open(string fileName);

    /*
    Saves the current image, represented by the matrix of pixels, as a
    Windows BMP file with the name provided by the parameter. File extension
    is not forced but should be .bmp. Any errors will cout and will NOT 
    attempt to save the file.
    @param name of the filename to be written as a bmp image
    */
    int save(string flieName);
    
    void updateImage(unsigned int r,unsigned int g,unsigned int b);
    
    bool encodeMsg(string msgFile);
    
    string decodeMsg(void);
};
#endif