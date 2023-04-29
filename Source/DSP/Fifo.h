/*
 ==============================================================================
 
 Fifo.h
 Created: 17 Apr 2023 6:57:04pm
 Author:  zack
 
 ==============================================================================
 */

#pragma once
#include <JuceHeader.h>

/*!
 @class Fifo
 @brief A template class that implements a First-In-First-Out (FIFO) queue.
 The Fifo class holds either std::vector<float> or juce::AudioBuffer<float> objects, and
 implements a FIFO queue to store and retrieve these objects. The capacity of the queue is fixed
 at 30.
 @tparam T The type of object to be held in the Fifo, either std::vector<float> or
 juce::AudioBuffer<float>.
 Example usage:
 @code
 Fifo<std::vector<float>> floatFifo;
 floatFifo.prepare(1024);
 std::vector<float> data;
 data.resize(1024, 0);
 floatFifo.push(data);
 std::vector<float> retrievedData;
 floatFifo.pull(retrievedData);
 @endcode
 */
template<typename T>
struct Fifo
{

    
    /*!
    @brief Prepares the Fifo to hold juce::AudioBuffer<float> objects.
    This method is only applicable when the Fifo is holding juce::AudioBuffer<float> objects.
    @param numChannels The number of channels to be stored in each buffer.
    @param numSamples The number of samples to be stored in each buffer.
    */
    void prepare(int numChannels, int numSamples)
    {
        static_assert( std::is_same_v<T, juce::AudioBuffer<float>>,
                      "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
        for( auto& buffer : buffers)
        {
            buffer.setSize(numChannels,
                           numSamples,
                           false,   //clear everything?
                           true,    //including the extra space?
                           true);   //avoid reallocating if you can?
            buffer.clear();
        }
    }
    
    /*!
     @brief Prepares the Fifo to hold std::vector<float> objects.
     This method is only applicable when the Fifo is holding std::vector<float> objects.
     @param numElements The number of elements to be stored in each vector.
     */
    void prepare(size_t numElements)
    {
        static_assert( std::is_same_v<T, std::vector<float>>,
                      "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
        for( auto& buffer : buffers )
        {
            buffer.clear();
            buffer.resize(numElements, 0);
        }
    }
    
    /*!
    @brief Pushes an object of type T onto the end of the queue.
    @param t The object to be pushed onto the queue.
    @return True if the push was successful, false otherwise.
    */
    bool push(const T& t)
    {
        auto write = fifo.write(1);
        if( write.blockSize1 > 0 )
        {
            buffers[write.startIndex1] = t;
            return true;
        }
        
        return false;
    }
    
    /*!
    @brief Pulls an object of type T from the front of the queue.
    @param t A reference to an object of type T that will be populated with the object from the front
    of the queue.
    @return True if the pull was successful, false otherwise.
    */
    bool pull(T& t)
    {
        auto read = fifo.read(1);
        if( read.blockSize1 > 0 )
        {
            t = buffers[read.startIndex1];
            return true;
        }
        
        return false;
    }
    
    /*!
    @brief Gets the number of objects in the queue that are available for reading.
    @return The number of objects in the queue that are available for reading.
    */
    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
private:
    /** The maximum capacity of the queue. */
    static constexpr int Capacity = 30;
    /** An array to store the objects in the queue. */
    std::array<T, Capacity> buffers;
    /** A juce::AbstractFifo to implement the FIFO queue. */
    juce::AbstractFifo fifo {Capacity};
};
