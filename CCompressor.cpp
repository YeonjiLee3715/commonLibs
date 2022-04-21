#include "CCompressor.h"
#include <zlib.h>

#include "CLogger.h"

bool CCompressor::gzipDecompress(QByteArray& compressData, QByteArray& decompressData )
{
#ifdef QT_QMF_HAVE_ZLIB
    if (compressData.size() <= 4)
    {
        LOGE( "CCompressor", "gUncompress: Input data is truncated" );
        return false;
    }

    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressData.size();
    strm.next_in = (Bytef*)(compressData.data());

    ret = inflateInit2(&strm, 15 +  32); // gzip decoding
    if (ret != Z_OK)
    {
        LOGE( "CCompressor", "Failed to inflateInit2 with error: %d", ret );
        return false;
    }

    // run inflate()
    do
    {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered

        switch (ret)
        {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            LOGE( "CCompressor", "Failed to inflate with error: %d", ret );
            decompressData.clear();
            return false;
        }

        decompressData.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);

    return true;
#else
    LOGE( "CCompressor", "ZLIB NOT SUPPORT" );
    return false;
#endif
}
