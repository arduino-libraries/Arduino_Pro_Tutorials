#include "SDMMCBlockDevice.h" // Multi Media Card APIs
#include "FATFileSystem.h"    // Mbed API for portable  and embedded systems
SDMMCBlockDevice blockDevice;
mbed::FATFileSystem fileSystem("fs");

#include "camera.h" // Arduino Mbed Core Camera APIs
#include "himax.h"  // Exclusive Camera library for the Portenta Vision Shield
HM01B0 himax;
Camera cam(himax);

FrameBuffer fb; // Buffer to save the capture

// Settings for our setup
#define RES_H (unsigned int)240
#define RES_W (unsigned int)320
#define IMAGE_MODE CAMERA_GRAYSCALE
#define IMAGE_BPP (unsigned int)8
// Headers info
#define HEADER_FILE_HEADER (unsigned int)14
#define HEADER_DIB_SIZE (unsigned int)40
#define HEADER_FULL_SIZE (HEADER_FILE_HEADER + HEADER_DIB_SIZE)
#define PALETTE_SIZE (2 ^ IMAGE_BPP) * 4 // 4 bytes per color

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    // Mount SD Card
    mountSD();

    // Init the cam QVGA, 30FPS, Grayscale
    if (!cam.begin(CAMERA_R320x240, IMAGE_MODE, 30))
    {
        Serial.println("Unable to find the camera");
    }

    // Save the headers and the image data into the .bmp file
    parseData();
}

void loop()
{
    while (1)
        ;
}

// Mount File system block
void mountSD()
{
    Serial.println("Mounting SD Card...");

    int error = fileSystem.mount(&blockDevice);
    if (error)
    {
        Serial.println("No SD Card found");
        while (1)
            ;
    }
}

void parseData()
{
    unsigned char *imgData = NULL;
    int fileSize = HEADER_FILE_HEADER + RES_W * RES_H;

    FILE *file = fopen("/fs/image.bmp", "w+");

    // Get a Frame
    if (cam.grabFrame(fb, 3000) == 0)
    {
        // Save the raw image data (8bpp grayscale)
        imgData = fb.getBuffer();
    }
    else
    {
        Serial.println("could not grab the frame");
        while (1)
            ;
    }
    // Bitmap structure (Head + DIB Head + ColorMap + binary image)
    unsigned char bitmapFileHeader[HEADER_FILE_HEADER];
    unsigned char bitmapDIBHeader[HEADER_DIB_SIZE];
    unsigned char colorMap[PALETTE_SIZE]; // Needed for <=8bpp grayscale bitmaps

    // Set the file headers to 0
    memset(bitmapFileHeader, (unsigned char)(0), HEADER_FILE_HEADER);
    memset(bitmapDIBHeader, (unsigned char)(0), HEADER_DIB_SIZE);
    memset(colorMap, (unsigned char)(0), PALETTE_SIZE);

    // Write the headers info
    // File header
    bitmapFileHeader[0] = 'B';
    bitmapFileHeader[1] = 'M';
    bitmapFileHeader[2] = (unsigned char)(fileSize);
    bitmapFileHeader[3] = (unsigned char)(fileSize >> 8);
    bitmapFileHeader[4] = (unsigned char)(fileSize >> 16);
    bitmapFileHeader[5] = (unsigned char)(fileSize >> 24);
    bitmapFileHeader[10] = (unsigned char)HEADER_FULL_SIZE + PALETTE_SIZE;

    // Info header
    bitmapDIBHeader[0] = (unsigned char)(HEADER_DIB_SIZE);
    bitmapDIBHeader[4] = (unsigned char)(RES_W);
    bitmapDIBHeader[5] = (unsigned char)(RES_W >> 8);
    bitmapDIBHeader[8] = (unsigned char)(RES_H);
    bitmapDIBHeader[8] = (unsigned char)(RES_H >> 8);
    bitmapDIBHeader[14] = (unsigned char)(IMAGE_BPP);

    // Color palette for grayscale Bitmaps (8bpp)
    for (int i = 0; i < (2 ^ IMAGE_BPP); i++)
    {
        colorMap[i * 4] = i;
        colorMap[i * 4 + 1] = i;
        colorMap[i * 4 + 2] = i;
    }

    // Write theh bitmap file
    fwrite(bitmapFileHeader, 1, HEADER_FILE_HEADER, file);
    fwrite(bitmapDIBHeader, 1, HEADER_DIB_SIZE, file);
    fwrite(colorMap, 1, PALETTE_SIZE, file); // Color map
    fwrite(imgData, 1, RES_H * RES_W, file);

    // Close the stream (bitmap file)
    fclose(file);
}