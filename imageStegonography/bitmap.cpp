#include <iostream>
#include <fstream>
#include "bitmap.h"
#include <cstdlib>
using namespace std;

vector<char> buffer; // to hold all the pixel data

Bitmap::Bitmap(){
        fh = {0}; // clear file header
        ih = {0}; //clear bmp_info_header 

}
Bitmap::~Bitmap(){
}

// --------------------------------------------------------------
int Bitmap::open(string bmpFileName)
{
   ifstream file(bmpFileName.c_str(), ios::in | ios::binary);
    if(!file.fail()) { // file opened
        // get length of file:
        file.seekg (0, file.end);   // the position at the end
        int length = file.tellg();  // is the length of the file
        file.seekg (0, file.beg);   // return to file beginning
        
        //make sure file is bitmap format
		file.read((char*)(&fh), sizeof(bmpfile_header));
        if (fh.signature[0] != 'B' ||fh.signature[1] !='M'){
            cout <<"invalid bitmap signature, expected BM and read "<<
                    fh.signature[0]<<fh.signature[1]<<endl;
            return -1;
        } 
 //______________________________________________________________________________       
        // read the bmp file info header
        file.read((char*)(&ih), sizeof(bmp_info_header));
        //validate header info
        if (ih.header_size != 40){
             cout <<"Invalid header in bmp file, info header size is "<<ih.header_size<<" instead of 40\n";
             return -1;
        }
//_____________________________________________________________________________
        int curPosition = file.tellg();  
        length = length - curPosition; // figure out what is left to read
        buffer.resize(length);  // make room for the file to be read into memory
        if (debug) 
            cout << "current position " << curPosition << " should match fh.bmp_offset "<< fh.bmp_offset << endl;
        file.read(&buffer[0],length); // read in the pixels
        file.close();
    }
    else{
        cout <<"unable to open file "<<bmpFileName<<"did not open \n";
        return -1;
    }
    return 0; //success
}

// ----------------------------------------------------------------------------
int Bitmap::save(string filename)
{
	ofstream file(filename.c_str(), ios::out | ios::binary);
	if (file.fail())
	{
		cout<<filename<<" could not be opened for editing \n";
		return -1;
	}

    file.write((char*)(&fh), sizeof(bmpfile_header));
    file.write((char*)(&ih), sizeof(bmp_info_header));

		// Write each row and column of Pixels into the image file -- we write
		// the rows upside-down to satisfy the easiest BMP format.
        unsigned int bitMapIndex =0;
		for (int row = ih.height - 1; row >= 0; row--)
		{
			for (unsigned int col = 0; col < ih.width; col++)
			{
				//const Pixel& p= (buffer[bitMapIndex];
				file.put((unsigned char)(buffer[bitMapIndex++])); // blue part of pixel
				file.put((unsigned char)(buffer[bitMapIndex++])); // green part of pixel
				file.put((unsigned char)(buffer[bitMapIndex++])); // red part of pixel
			}
			// Rows are padded so that they're always a multiple of 4 bytes
			// This line skips the padding at the end of each row.
			for (unsigned int i = 0; i < ih.width % 4; i++)
				file.put(buffer[bitMapIndex++]);
		}
		file.close();
}

//________________________________________________________________________
bool isBitSet(char c, int bitIndex){
	c = c >> bitIndex; // shift right
	//check bit position
		return (c & 1); //0000 0001
}

//_____________________________________________________________________
bool Bitmap::encodeMsg(string msgFile){
    // open the text file to get the message 
    ifstream file(msgFile.c_str());
	if(!file.is_open()) {
		cout << " Error opening message file " << msgFile << endl;
		return false;
	}
    int bufferIndex = 0;
    int charIndex = 0;
    int bitIndex = 0;
    char charToEncode;
    //Each char (8 bits) is stored in the Least Significant Bits (LSB) of the RGB pixel values (Red,Green,Blue).
    file.get(charToEncode);  //init first character from message
    for (int row = 0; row < ih.height; row++) //y
    {
        for (int col = 0; col < ih.width; col++) //x
        {
            if (debug)  {
                cout <<"encoding character "<<charToEncode<<endl;
                cout <<"at buffer index "<<bufferIndex;
                cout <<" doing bit "<<bitIndex;
            }
            if (isBitSet(charToEncode,bitIndex++)){
                buffer[bufferIndex++] |=1; //set bit 0 and will shift left 
                if (debug) cout <<"set\n";
            }
            else{
               buffer[bufferIndex++] &= ~1; //clear bit 0 for this color entry
               if (debug) cout <<"clear\n";
            }
            if (debug) 
                cout <<"stored at offset "<<bufferIndex-1<<" = "<<hex<<+buffer[bufferIndex-1];
                
            if (bitIndex==8){
                if (charToEncode == '\0') return true; // we are done
                bitIndex=0;  // reset for next character
                file.get(charToEncode); // get next character
                if(file.eof()) {
                    charToEncode = '\0';
                }
            }
        }
    } 
    cout <<"unable to encode message\n";
    return false;
}

string Bitmap::decodeMsg(){
    int bufferIndex = 0;
    int bitIndex = 0;
    int characterIndex = 0;

    unsigned char decodedChar = 'a'; // make sure it is not '\0'
    string msg = "\n";
     for (int row = 0; row < ih.height; row++)
     {
        for (int col = 0; col < ih.width; col++)
        {
            if (bitIndex == 0){
                if (debug) cout <<"decoding character at index " << characterIndex << endl;
                if (characterIndex > 0){
                    if (debug) cout << "last decoded character " << decodedChar << endl;
                    msg.append(1, decodedChar);
                    if (decodedChar == '\0') {
                        return msg; // we are done
                    }
                }
            }
            
            if (debug){ 
                cout <<"at offset "<<bufferIndex<<" = "<<hex<<+buffer[bufferIndex];
                cout <<"reading bit "<<bitIndex;
            }
            
            if (isBitSet(buffer[bufferIndex++], 0)){ //check if bit 0 is set
                decodedChar |= 0x80;  //set most significant bit (will rotate right as we proceed to other bits)
                if (debug) cout <<" set\n";
            }
            
            else if (debug) 
                cout <<" clear\n";
                
            bitIndex++;
            if (bitIndex == 8){// char is 8 bits
                bitIndex = 0;  // time for new character
                characterIndex++;
            }
            else    
                decodedChar = decodedChar >> 1; // shift left
        }
    }
    return "no message found";
}

