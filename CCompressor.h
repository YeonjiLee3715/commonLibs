#ifndef CCOMPRESSOR_H
#define CCOMPRESSOR_H

#define ZLIB_CONST

#include <QByteArray>
#define DEC_MAGIC_NUM_FOR_GZIP 16

class CCompressor
{
public:
    CCompressor() {}
    ~CCompressor() {}

    static bool gzipDecompress(QByteArray &compressData , QByteArray &decompressData);
};
#endif // CCOMPRESSOR_H
