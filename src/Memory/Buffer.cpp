#include "Buffer.hpp"

#include "../System/Log.hpp"
#include <cstring>

Buffer::Buffer()
    : data(NULL),
      length(0),
      bufferEnd(NULL),
      readLoc(NULL),
      writeLoc(NULL),
      external(false)
{
}

Buffer::Buffer(size_t maxSize)
{
    allocateData(maxSize);
}

Buffer::Buffer(char* buf, int bufferLength)
{
    setBuffer(buf, bufferLength);
}

Buffer::~Buffer()
{
    if (!external)
        delete [] data;
    data = NULL;
}

void Buffer::allocateData(size_t maxSize)
{
    data = new char[maxSize];
    length = maxSize;
    bufferEnd = &data[length];

    readLoc = writeLoc = data;

    external = false;
}

void Buffer::setBuffer(char* buf, int bufferLength)
{
    data = buf;
    length = bufferLength;
    bufferEnd = &data[length];

    readLoc = data;
    writeLoc = bufferEnd;

    external = true;

    Console::logf("%p, %d", data, length);
}

bool Buffer::atEnd()
{
    return readLoc == bufferEnd;
}

size_t Buffer::size()
{
    return (size_t)(bufferEnd - data);
}

void Buffer::rewind()
{
    readLoc = data;
}

void Buffer::move(int offset)
{
    writeLoc = data + offset;
}

void Buffer::clear()
{
    writeLoc = readLoc = data;
}

void Buffer::append(Buffer* b)
{
    memcpy(bufferEnd, b->getData(), b->size());
    writeLoc += b->size();
}

char* Buffer::getData()
{
    return data;
}

float Buffer::readFloat()
{
    float f;
    memcpy(&f, readLoc, sizeof(f));
    readLoc += sizeof(f);

    return f;
}

void Buffer::writeFloat(float f)
{
    memcpy(writeLoc, &f, sizeof(f));
    writeLoc += sizeof(f);
}

void Buffer::dumpBuffer()
{
    rewind();

    while (!atEnd())
    {
        Console::logf("%f", readFloat());
    }
}


