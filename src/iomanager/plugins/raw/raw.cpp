//--------------------------------------------------
// Author: Chung Lab / MIT.
//
// Raw image file format.
//
// This is a plugin for a raw image format. The format
// consists of two big-endian 32-bit integers that give
// the dimensions of the 2D file, followed by the data.
//
// Our A/D converter on our camera is 16-bit. We assume
// that the file is saved as 16-bit bigendian words.
//
//---------------------------------------------------

#include <cstdio>
#include <cstdint>
#include "raw.h"
#include "VirtualFmtMngr.h"

TERASTITCHER_REGISTER_IO_PLUGIN_2D(raw)

// insert here your plugin description that will be displayed on the user interface
std::string iomanager::raw::desc()
{
	return	"******************************************************\n"
			"* raw file format: Chung Lab, MIT                    *\n"
			"******************************************************\n"
			"*                                                    *\n"
			"* 2D image-based I/O plugin for raw file format imgs *\n"
			"*                                                    *\n"
			"* Supported image format extentions:                 *\n"
			"*  - RAW                                             *\n"
			"*                                                    *\n"
			"* Accepted configuration parameters:                 *\n"
			"*  - none                                            *\n"
			"*                                                    *\n"
			"******************************************************\n";
}

// Return if channels are interleaved (in case the image has just one channel return value is indefinite)
bool
	iomanager::raw::isChansInterleaved( )
{
	return true;
}

int read_raw_int(FILE *file)
throw (iom::exception)
{
    int result = 0;
    int c;
    for (int i=0; i<4; i++) {
        result *= 256;
        c = std::fgetc(file);
        if (c < 0) {
            throw iom::exception("Encountered end of file.");
        }
        result += c;
    }
    return result;
}
// read image metadata from a 2D image file
void
	iomanager::raw::readMetadata(
	std::string img_path,			// (INPUT)  image filepath
	int & img_width,				// (OUTPUT) image width  (in pixels)
	int & img_height,				// (OUTPUT) image height (in pixels)
	int & img_bytes_x_chan,			// (OUTPUT) number of bytes per channel
	int & img_chans,				// (OUTPUT) number of channels
	const std::string & params)		// (INPUT)  additional parameters <param1=val, param2=val, ...>
throw (iom::exception)
{
    FILE *file = std::fopen(img_path.c_str(), "rb");
    if (! file) {
        throw iom::exception(iom::strprintf("Unable to open %s", img_path));
    }
    try {
        img_width = read_raw_int(file);
        img_height = read_raw_int(file);
        img_bytes_x_chan = 2;
        img_chans = 1;
    } catch (const iom::exception &e) {
        std::fclose(file);
        throw e;
    }
    std::fclose(file);

}

// Read 2D image data
unsigned char *						// (OUTPUT) a buffer storing the 2D image
	iomanager::raw::readData(
	std::string img_path,			// (INPUT)	image filepath
	int & img_width,				// (INPUT/OUTPUT) image width  (in pixels)
	int & img_height,				// (INPUT/OUTPUT) image height (in pixels)
	int & img_bytes_x_chan,			// (INPUT/OUTPUT) number of bytes per channel
	int & img_chans,				// (INPUT/OUTPUT) number of channels to be read
	unsigned char *data,			// (INPUT) image data
	const std::string & params)		// (INPUT) additional parameters <param1=val, param2=val, ...>
throw (iom::exception)
{
    int true_width;
    int true_height;
    int idx = 0;
    int c;
    FILE *file = std::fopen(img_path.c_str(), "rb");
    if (! file) {
        throw iom::exception(iom::strprintf("Unable to open %s", img_path));
    }
    try {
        true_width = read_raw_int(file);
        true_height = read_raw_int(file);
        for (int i=0; i<img_height; i++) {
            for (int j=0; j<img_width; j++) {
                c = std::fgetc(file);
                if (c < 0) {
                    throw iom::exception("End of file");
                }
                data[idx+1] = (char)c;
                c = std::fgetc(file);
                if (c < 0) {
                    throw iom::exception("End of file");
                }
                data[idx] = (char)c;
                idx += 2;
            }
            if (img_width < true_width) {
                fseek(file, (true_width - img_width) * 2, SEEK_CUR);
            }
        }
    } catch (const iom::exception &e) {
        std::fclose(file);
        throw e;
    }
    std::fclose(file);
}

// Write 2D image data into a single (2D) image file
void
	iomanager::raw::writeData(
	std::string img_path,			// (INPUT)	image filepath (it includes the file extension)
	unsigned char *raw_img,			// (INPUT)	image data to be saved into the file
	int img_height,					// (INPUT)	image height
	int img_width,					// (INPUT)	image width
	int img_bytes_x_chan,			// (INPUT)  number of bytes per channel
	int img_chans,					// (INPUT)	number of channels
	int y0,							// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int y1,							// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int x0,							// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int x1,							// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	const std::string & params)		// (INPUT) additional parameters <param1=val, param2=val, ...>
throw (iom::exception)
{
    throw iom::exception("Writing raw is not implemented.");
}

// read 3D image data from a stack of (2D) image files
iom::real_t*						// (OUTPUT) a [0.0,1.0]-valued array storing the 3D image in channel->slice->row order
	iomanager::raw::readData(
	char **files,					// (INPUT)	array of C-strings storing image filenames
	int files_size,					// (INPUT)	size of 'files'
	const char *path /*= 0*/,		// (INPUT)	path to be concatenated to the i-th entry of 'files'
	int first /*= -1*/,				// (INPUT)	selects a range [first, last] of files to be loaded
	int last /*= -1*/,				// (INPUT)	selects a range [first, last] of files to be loaded
	bool is_sparse /*= false*/,		// (INPUT)	if true, 'files' is a sparse array and null entries should be treated as empty (black) images
	iom::channel chan,				// (INPUT)	channel selection { ALL, R, G, B }.
	const std::string & params)		// (INPUT)	additional parameters <param1=val, param2=val, ...>
throw (iom::exception)
{
	throw iom::exception("Not implemented");
}

// write 2D image data into a single (2D) image file
void
	iomanager::raw::writeData(
	std::string img_path,		// (INPUT)	image filepath (it includes the file extension)
	iom::real_t* raw_img,			// (INPUT)	a [0.0,1.0]-valued array storing the 2D image in channel->row order
	int img_height,				// (INPUT)	image height
	int img_width,				// (INPUT)	image width
	int img_chans,				// (INPUT)	number of channels
	int y0,						// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int y1,						// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int x0,						// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int x1,						// (INPUT)	region of interest [x0,x1][y0,y1] to be set on the image
	int bpp,					// (INPUT)	color depth (bits per pixel)
	const std::string & params)	// (INPUT)	additional parameters <param1=val, param2=val, ...>
	throw (iom::exception)
{
	throw iom::exception(iom::strprintf("not implemented yet (params = \"%s\")", params.c_str()), __iom__current__function__);

	/**/iom::debug(iom::LEV3, iom::strprintf("img_path = %s, img_height = %d, img_width = %d, y0 = %d, y1 = %d, x0 = %d, x1 = %d, bpp = %d, params = \"%s\"",
		img_path.c_str(), img_height, img_width, y0, y1, x0, x1, bpp, params.c_str()).c_str(), __iom__current__function__);
}
