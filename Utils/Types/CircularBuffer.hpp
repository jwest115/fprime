/*
 * CircularBuffer.hpp:
 *
 * Buffer used to efficiently store data in ring data structure. Uses an externally supplied
 * data store as the backing for this buffer. Thus it is dependent on receiving sole ownership
 * of the supplied buffer.
 *
 * Note: this given implementation loses one byte of the data store in order to ensure that a
 * separate wrap-around tracking variable is not needed.
 *
 *  Created on: Apr 4, 2019
 *      Author: lestarch
 *  Revised March 2022
 *      Author: bocchino
 */

#ifndef TYPES_CIRCULAR_BUFFER_HPP
#define TYPES_CIRCULAR_BUFFER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Types {

class CircularBuffer {

    friend class CircularBufferTester;

    public:
        /**
         * Circular buffer constructor. Wraps the supplied buffer as the new data store. Buffer
         * size is supplied in the 'size' argument.
         *
         * Note: specification of storage buffer must be done using `setup` before use.
         */
        CircularBuffer();

        /**
         * Circular buffer constructor. Wraps the supplied buffer as the new data store. Buffer
         * size is supplied in the 'size' argument. This is equivalent to calling the no-argument constructor followed
         * by setup(buffer, size).
         *
         * Note: ownership of the supplied buffer is held until the circular buffer is deallocated
         *
         * \param buffer: supplied buffer used as a data store.
         * \param size: the of the supplied data store.
         */
        CircularBuffer(U8* const buffer, const FwSizeType size);

        /**
         * Wraps the supplied buffer as the new data store. Buffer size is supplied in the 'size' argument. Cannot be
         * called after successful setup.
         *
         * Note: ownership of the supplied buffer is held until the circular buffer is deallocated
         *
         * \param buffer: supplied buffer used as a data store.
         * \param size: the of the supplied data store.
         */
        void setup(U8* const buffer, const FwSizeType size);

        /**
         * Serialize a given buffer into this circular buffer. Will not accept more data than
         * space available. This means it will not overwrite existing data.
         * \param buffer: supplied buffer to be serialized.
         * \param size: size of the supplied buffer.
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus serialize(const U8* const buffer, const FwSizeType size);

        /**
         * Deserialize data into the given variable without moving the head index
         * \param value: value to fill
         * \param offset: offset from head to start peak. Default: 0
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus peek(char& value, FwSizeType offset = 0) const;
        /**
         * Deserialize data into the given variable without moving the head index
         * \param value: value to fill
         * \param offset: offset from head to start peak. Default: 0
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus peek(U8& value, FwSizeType offset = 0) const;
        /**
         * Deserialize data into the given variable without moving the head index
         * \param value: value to fill
         * \param offset: offset from head to start peak. Default: 0
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus peek(U32& value, FwSizeType offset = 0) const;

        /**
         * Deserialize data into the given buffer without moving the head variable.
         * \param buffer: buffer to fill with data of the peek
         * \param size: size in bytes to peek at
         * \param offset: offset from head to start peak. Default: 0
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus peek(U8* buffer, FwSizeType size, FwSizeType offset = 0) const;

        /**
         * Rotate the head index, deleting data from the circular buffer and making
         * space. Cannot rotate more than the available space.
         * \param amount: amount to rotate by (in bytes)
         * \return Fw::FW_SERIALIZE_OK on success or something else on error
         */
        Fw::SerializeStatus rotate(FwSizeType amount);

        /**
         * Get the number of bytes allocated in the buffer
         * \return number of bytes
         */
        FwSizeType get_allocated_size() const;

        /**
         * Get the number of free bytes, i.e., the number
         * of bytes that may be stored in the buffer without
         * deleting data and without exceeding the buffer capacity
         */
        FwSizeType get_free_size() const;

        /**
         * Get the logical capacity of the buffer, i.e., the number of available
         * bytes when the buffer is empty
         */
        FwSizeType get_capacity() const;

        /**
         * Return the largest tracked allocated size
         */
        FwSizeType get_high_water_mark() const;

        /**
         * Clear tracking of the largest allocated size
         */
        void clear_high_water_mark();

    private:
        /**
         * Returns a wrap-advanced index into the store.
         * \param idx: index to advance and wrap.
         * \param amount: amount to advance
         * \return: new index value
         */
        FwSizeType advance_idx(FwSizeType idx, FwSizeType amount = 1) const;
        //! Physical store backing this circular buffer
        U8* m_store;
        //! Size of the physical store
        FwSizeType m_store_size;
        //! Index into m_store of byte zero in the logical store.
        //! When memory is deallocated, this index moves forward and wraps around.
        FwSizeType m_head_idx;
        //! Allocated size (size of the logical store)
        FwSizeType m_allocated_size;
        //! Maximum allocated size
        FwSizeType m_high_water_mark;
};
} //End Namespace Types
#endif

