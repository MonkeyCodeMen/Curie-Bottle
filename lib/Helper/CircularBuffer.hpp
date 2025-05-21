#pragma once

#include <Arduino.h>
#include <Mutex.hpp> // Include mutex for thread safety
#include <vector>


template <typename T>
class CircularBuffer {
public:
    // Constructor to initialize the buffer with a given size
    CircularBuffer();

    // Constructor to initialize the buffer with a given size
    CircularBuffer(size_t bufferSize);

    // Add a sample vector to the buffer
    void addSample(const T& sampleVector);

    // Retrieve the oldest sample (from the front of the buffer)
    bool getOldestSample(T& sampleVector);

    // Check if the buffer is full
    bool isFull() const;


    void setBufferSize(size_t bufferSize);

private:
    std::vector<T> _buffer; // Buffer holding data
    size_t _bufferSize;     // Maximum size of the buffer
    size_t _writeIndex;     // Index for writing to the buffer
    size_t _readIndex;      // Index for reading from the buffer
    bool _full;             // Flag to check if the buffer is full
    Mutex _mutex;           // Mutex for thread safety, protecting only indices

    void _clear();

};

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t bufferSize)
    : _bufferSize(bufferSize), _writeIndex(0), _readIndex(0), _full(false) {
       
    _buffer.resize(bufferSize);
    _mutex.free();
}

template <typename T>
CircularBuffer<T>::CircularBuffer()
    : _bufferSize(0), _writeIndex(0), _readIndex(0), _full(false) {
    _mutex.free();
}

template <typename T>
void CircularBuffer<T>::addSample(const T& sampleVector) {
    _mutex.lock();

    // If the buffer is full, overwrite the oldest sample
    if (_full) {
        _readIndex = (_readIndex + 1) % _bufferSize;  // Move the read index forward
    }

    _buffer[_writeIndex] = sampleVector;  // Store the sample at the write index
    _writeIndex = (_writeIndex + 1) % _bufferSize;  // Move the write index forward

    // If the write index catches up to the read index, the buffer is full
    if (_writeIndex == _readIndex) {
        _full = true;
    }
    _mutex.free();

}

template <typename T>
bool CircularBuffer<T>::getOldestSample(T& sampleVector) {
    _mutex.lock();

    if (_writeIndex != _readIndex || _full) {  // If there are samples to read
        sampleVector = _buffer[_readIndex];
        _readIndex = (_readIndex + 1) % _bufferSize;
        _full = false;  // Reset the full flag after reading
        _mutex.free();
        return true;
    }
    _mutex.free();
    return false;  // No data to read
}

template <typename T>
bool CircularBuffer<T>::isFull() const {
    return _full;
}

template <typename T>
void CircularBuffer<T>::_clear() {
    _writeIndex = 0;
    _readIndex = 0;
    _full = false;

}


template <typename T>
void CircularBuffer<T>::setBufferSize(size_t bufferSize) {
    _mutex.lock();
    _bufferSize = bufferSize;
    _buffer.resize(bufferSize);
    _clear();
    _mutex.free();
}