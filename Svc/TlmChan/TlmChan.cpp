/**
 * \file
 * \author T. Canham
 * \brief Implementation file for channelized telemetry storage component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#include <FpConfig.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/TlmChan/TlmChan.hpp>

namespace Svc {

// Definition of TLMCHAN_HASH_BUCKETS is >= number of telemetry ids
static_assert(std::numeric_limits<FwChanIdType>::max() >= TLMCHAN_HASH_BUCKETS, "Cannot have more hash buckets than maximum telemetry ids in the system");
// TLMCHAN_HASH_BUCKETS >= TLMCHAN_NUM_TLM_HASH_SLOTS >= 0
static_assert(std::numeric_limits<FwChanIdType>::max() >= TLMCHAN_NUM_TLM_HASH_SLOTS, "Cannot have more hash slots than maximum telemetry ids in the system");

TlmChan::TlmChan(const char* name) : TlmChanComponentBase(name), m_activeBuffer(0) {
    // clear slot pointers
    for (FwChanIdType entry = 0; entry < TLMCHAN_NUM_TLM_HASH_SLOTS; entry++) {
        this->m_tlmEntries[0].slots[entry] = nullptr;
        this->m_tlmEntries[1].slots[entry] = nullptr;
    }
    // clear buckets
    for (FwChanIdType entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        this->m_tlmEntries[0].buckets[entry].used = false;
        this->m_tlmEntries[0].buckets[entry].updated = false;
        this->m_tlmEntries[0].buckets[entry].bucketNo = entry;
        this->m_tlmEntries[0].buckets[entry].next = nullptr;
        this->m_tlmEntries[0].buckets[entry].id = 0;
        this->m_tlmEntries[1].buckets[entry].used = false;
        this->m_tlmEntries[1].buckets[entry].updated = false;
        this->m_tlmEntries[1].buckets[entry].bucketNo = entry;
        this->m_tlmEntries[1].buckets[entry].next = nullptr;
        this->m_tlmEntries[1].buckets[entry].id = 0;
    }
    // clear free index
    this->m_tlmEntries[0].free = 0;
    this->m_tlmEntries[1].free = 0;
}

TlmChan::~TlmChan() {}

FwChanIdType TlmChan::doHash(FwChanIdType id) {
    return (id % TLMCHAN_HASH_MOD_VALUE) % TLMCHAN_NUM_TLM_HASH_SLOTS;
}

void TlmChan::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

void TlmChan::TlmGet_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    // Compute index for entry

    FwChanIdType index = this->doHash(id);

    // Search to see if channel has been stored
    TlmEntry* entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
    for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
        if (entryToUse) {  // If bucket exists, check id
            if (entryToUse->id == id) {
                break;
            } else {  // otherwise go to next bucket
                entryToUse = entryToUse->next;
            }
        } else {  // no buckets left to search
            break;
        }
    }

    if (entryToUse) {
        val = entryToUse->buffer;
        timeTag = entryToUse->lastUpdate;
    } else {  // requested entry may not be written yet; empty buffer
        val.resetSer();
    }
}

void TlmChan::TlmRecv_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    // Compute index for entry

    FwChanIdType index = this->doHash(id);
    TlmEntry* entryToUse = nullptr;
    TlmEntry* prevEntry = nullptr;

    // Search to see if channel has already been stored or a bucket needs to be added
    if (this->m_tlmEntries[this->m_activeBuffer].slots[index]) {
        entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
        for (FwChanIdType bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
            if (entryToUse) {
                if (entryToUse->id == id) {  // found the matching entry
                    break;
                } else {  // try next entry
                    prevEntry = entryToUse;
                    entryToUse = entryToUse->next;
                }
            } else {
                // Make sure that we haven't run out of buckets
                FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
                // add new bucket from free list
                entryToUse =
                    &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
                FW_ASSERT(prevEntry);
                prevEntry->next = entryToUse;
                // clear next pointer
                entryToUse->next = nullptr;
                break;
            }
        }
    } else {
        // Make sure that we haven't run out of buckets
        FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
        // create new entry at slot head
        this->m_tlmEntries[this->m_activeBuffer].slots[index] =
            &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
        entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
        entryToUse->next = nullptr;
    }

    // copy into entry
    FW_ASSERT(entryToUse);
    entryToUse->used = true;
    entryToUse->id = id;
    entryToUse->updated = true;
    entryToUse->lastUpdate = timeTag;
    entryToUse->buffer = val;
}

void TlmChan::Run_handler(FwIndexType portNum, U32 context) {
    // Only write packets if connected
    if (not this->isConnected_PktSend_OutputPort(0)) {
        return;
    }

    // lock mutex long enough to modify active telemetry buffer
    // so the data can be read without worrying about updates
    this->lock();
    this->m_activeBuffer = 1 - this->m_activeBuffer;
    // set activeBuffer to not updated
    for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        this->m_tlmEntries[this->m_activeBuffer].buckets[entry].updated = false;
    }
    this->unLock();

    // go through each entry and send a packet if it has been updated
    Fw::TlmPacket pkt;
    pkt.resetPktSer();

    for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
        TlmEntry* p_entry = &this->m_tlmEntries[1 - this->m_activeBuffer].buckets[entry];
        if ((p_entry->updated) && (p_entry->used)) {
            Fw::SerializeStatus stat = pkt.addValue(p_entry->id, p_entry->lastUpdate, p_entry->buffer);

            // check to see if this packet is full, if so, send it
            if (Fw::FW_SERIALIZE_NO_ROOM_LEFT == stat) {
                this->PktSend_out(0, pkt.getBuffer(), 0);
                // reset packet for more entries
                pkt.resetPktSer();
                // add entry to new packet
                stat = pkt.addValue(p_entry->id, p_entry->lastUpdate, p_entry->buffer);
                // if this doesn't work, that means packet isn't big enough for
                // even one channel, so assert
                FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
            } else if (Fw::FW_SERIALIZE_OK == stat) {
                // if there was still room, do nothing move on to the next channel in the packet
            } else  // any other status is an assert, since it shouldn't happen
            {
                FW_ASSERT(0, static_cast<FwAssertArgType>(stat));
            }
            // flag as updated
            p_entry->updated = false;
        }  // end if entry was updated
    }      // end for each entry

    // send remnant entries
    if (pkt.getNumEntries() > 0) {
        this->PktSend_out(0, pkt.getBuffer(), 0);
    }
}  // end run handler

}  // namespace Svc
