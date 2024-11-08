#include "ImageLoader.h"
#include "Logger.h"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

#include <jpeglib.h>
#include <png.h>

GLuint ImageLoader::loadTextureFromFile(const std::string &filePath) {
    logDebug("Attempting to load texture from file: " + filePath);
    if (isPNG(filePath)) {
        logDebug("Detected PNG format for file: " + filePath);
        return loadPNGTexture(filePath);
    } else if (isJPG(filePath)) {
        logDebug("Detected JPG format for file: " + filePath);
        return loadJPGTexture(filePath);
    } else {
        logError("Unsupported image format: " + filePath);
        throw std::runtime_error("Unsupported image format");
    }
}

bool ImageLoader::isPNG(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        logWarn("Failed to open file for PNG check: " + filePath);
        return false;
    }

    png_byte header[8];
    file.read(reinterpret_cast<char *>(header), 8);
    return !png_sig_cmp(header, 0, 8);
}

bool ImageLoader::isJPG(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        logWarn("Failed to open file for JPG check: " + filePath);
        return false;
    }

    unsigned char header[2];
    file.read(reinterpret_cast<char *>(header), 2);
    return (header[0] == 0xFF && header[1] == 0xD8);
}

GLuint ImageLoader::loadPNGTexture(const std::string &filePath) {
    logDebug("Loading PNG texture from file: " + filePath);

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        logError("Could not open image file: " + filePath);
        return 0;
    }

    png_byte header[8];
    file.read(reinterpret_cast<char *>(header), 8);
    if (!file.good() || png_sig_cmp(header, 0, 8)) {
        logError("File is not recognized as a PNG file: " + filePath);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        logError("Failed to create PNG read structure");
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        logError("Failed to create PNG info structure");
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        logError("Failed to set PNG error handling");
        return 0;
    }

    png_set_read_fn(png_ptr, (png_voidp)&file, [](png_structp pngPtr, png_bytep data, png_size_t length) {
        std::ifstream *stream = reinterpret_cast<std::ifstream *>(png_get_io_ptr(pngPtr));
        stream->read(reinterpret_cast<char *>(data), length);
    });

    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 imgWidth, imgHeight;
    int bitDepth, colorType;
    png_get_IHDR(png_ptr, info_ptr, &imgWidth, &imgHeight, &bitDepth, &colorType, nullptr, nullptr, nullptr);

    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bitDepth == 16)
        png_set_strip_16(png_ptr);
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    png_read_update_info(png_ptr, info_ptr);

    std::vector<png_bytep> row_pointers(imgHeight);
    for (size_t i = 0; i < imgHeight; i++) {
        row_pointers[i] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];
    }

    png_read_image(png_ptr, row_pointers.data());

    std::vector<png_byte> imageData(imgWidth * imgHeight * 4);
    for (png_uint_32 i = 0; i < imgHeight; ++i) {
        std::memcpy(&imageData[i * imgWidth * 4], row_pointers[i], png_get_rowbytes(png_ptr, info_ptr));
        delete[] row_pointers[i]; // Free each row after copying
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    logDebug("Successfully loaded PNG texture with ID: " + std::to_string(textureID));
    return textureID;
}

GLuint ImageLoader::loadJPGTexture(const std::string &filePath) {
    logDebug("Loading JPG texture from file: " + filePath);

    FILE *infile = fopen(filePath.c_str(), "rb");
    if (!infile) {
        logError("Error: Failed to open image " + filePath);
        return false;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int rowStride = cinfo.output_width * cinfo.output_components;
    std::vector<unsigned char> imageData(cinfo.output_width * cinfo.output_height * 3);

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *rowPointer = &imageData[cinfo.output_scanline * rowStride];
        jpeg_read_scanlines(&cinfo, &rowPointer, 1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    logTrace("JPG image width: " + std::to_string(cinfo.output_width) +
             ", height: " + std::to_string(cinfo.output_height));

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cinfo.output_width, cinfo.output_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imageData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    logDebug("Successfully loaded JPG texture with ID: " + std::to_string(textureID));
    return textureID;
}
