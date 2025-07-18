#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/ExternalString.hpp>
#include <Fw/Types/InternalInterfaceString.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Types/ObjectName.hpp>
#include <Fw/Types/PolyType.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/StringTemplate.hpp>
#include <Os/IntervalTimer.hpp>
//
// Created by mstarch on 12/7/20.
//
#include <Fw/Types/StringUtils.hpp>
#include <cstring>

#include <cstdio>
#include <cstring>
#include <iostream>

#include <iostream>

#include <iostream>

#define DEBUG_VERBOSE 0

#include <gtest/gtest.h>
#include "SerializeBufferBaseTester.hpp"

class SerializeTestBuffer : public Fw::SerializeBufferBase {
  public:
    FwSizeType getBuffCapacity() const {  // !< returns capacity, not current size, of buffer
        return sizeof(m_testBuff);
    }

    U8* getBuffAddr() {  // !< gets buffer address for data filling
        return m_testBuff;
    }
    const U8* getBuffAddr() const {  // !< gets buffer address for data reading
        return m_testBuff;
    }

  private:
    U8 m_testBuff[255];
};

TEST(SerializationTest, Serialization1) {
    printf("Testing Serialization code\n");

    SerializeTestBuffer buff;

#if DEBUG_VERBOSE
    printf("U8 Test\n");
#endif

    U8 u8t1 = 0xAB;
    U8 u8t2 = 0;
    U8* ptr = buff.getBuffAddr();

    Fw::SerializeStatus stat1;
    Fw::SerializeStatus stat2;

    // Test chars
    buff.resetSer();
    Fw::SerializeBufferBaseTester::assertResetState(buff);

    Fw::SerializeBufferBaseTester::verifyU8Serialization(buff, u8t1);
    ASSERT_EQ(0xAB, ptr[0]); // Verify the actual byte value in the buffer

    Fw::SerializeBufferBaseTester::verifyU8Deserialization(buff, u8t2, u8t1);

    ASSERT_EQ(u8t2, u8t1);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u8t1, u8t2, stat1, stat2);
    printf("I8 Test\n");
#endif

    buff.resetSer();
    Fw::SerializeBufferBaseTester::assertResetState(buff);

    I8 i8t1 = static_cast<I8>(0xFF);
    I8 i8t2 = 0;

    // Use the tester to verify I8 serialization
    Fw::SerializeBufferBaseTester::verifyI8Serialization(buff, i8t1);
    ASSERT_EQ(0xFF, ptr[0]); // Verify the actual byte value in the buffer

    // Use the tester to verify I8 deserialization
    Fw::SerializeBufferBaseTester::verifyI8Deserialization(buff, i8t2, i8t1);

    ASSERT_EQ(i8t1, i8t2);

    buff.resetSer();
    Fw::SerializeBufferBaseTester::assertResetState(buff);

    // double check negative numbers
    i8t1 = -100;
    i8t2 = 0;

    // Use the tester to verify I8 serialization with negative number
    Fw::SerializeBufferBaseTester::verifyI8Serialization(buff, i8t1);

    // Use the tester to verify I8 deserialization with negative number
    Fw::SerializeBufferBaseTester::verifyI8Deserialization(buff, i8t2, i8t1);

    ASSERT_EQ(i8t1, i8t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i8t1, i8t2, stat1, stat2);
    printf("U16 Test\n");
#endif

    U16 u16t1 = 0xABCD;
    U16 u16t2 = 0;

    // Test shorts

    buff.resetSer();
    stat1 = buff.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 2);
    ASSERT_EQ(0xAB, ptr[0]);
    ASSERT_EQ(0xCD, ptr[1]);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u16t1, u16t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u16t1, u16t2, stat1, stat2);
    printf("I16 test\n");
#endif

    I16 i16t1 = static_cast<I16>(0xABCD);
    I16 i16t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 2);
    // 2s complement
    ASSERT_EQ(0xAB, ptr[0]);
    ASSERT_EQ(0xCD, ptr[1]);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i16t1, i16t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 2);

    // double check negative number
    i16t1 = -1000;
    i16t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 2);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i16t1, i16t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i16t1, i16t2, stat1, stat2);

    printf("U32 Test\n");
#endif

    U32 u32t1 = 0xABCDEF12;
    U32 u32t2 = 0;

    // Test ints

    buff.resetSer();
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 4);
    ASSERT_EQ(0xAB, ptr[0]);
    ASSERT_EQ(0xCD, ptr[1]);
    ASSERT_EQ(0xEF, ptr[2]);
    ASSERT_EQ(0x12, ptr[3]);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u32t1, u32t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 4);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u32t1, u32t2, stat1, stat2);
    printf("I32 Test\n");
#endif

    I32 i32t1 = 0xABCDEF12;
    I32 i32t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 4);
    ASSERT_EQ(0xAB, ptr[0]);
    ASSERT_EQ(0xCD, ptr[1]);
    ASSERT_EQ(0xEF, ptr[2]);
    ASSERT_EQ(0x12, ptr[3]);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 4);
    ASSERT_EQ(i32t1, i32t2);

    // double check negative number
    i32t1 = -1000000;
    i32t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 4);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 4);
    ASSERT_EQ(i32t1, i32t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i32t1, i32t2, stat1, stat2);

    printf("U64 Test\n");
#endif

    U64 u64t1 = 0x0123456789ABCDEF;
    U64 u64t2 = 0;

    // Test ints

    buff.resetSer();
    stat1 = buff.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 8);
    ASSERT_EQ(0x01, ptr[0]);
    ASSERT_EQ(0x23, ptr[1]);
    ASSERT_EQ(0x45, ptr[2]);
    ASSERT_EQ(0x67, ptr[3]);
    ASSERT_EQ(0x89, ptr[4]);
    ASSERT_EQ(0xAB, ptr[5]);
    ASSERT_EQ(0xCD, ptr[6]);
    ASSERT_EQ(0xEF, ptr[7]);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u64t1, u64t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 8);

#if DEBUG_VERBOSE
    printf("Val: in: %lld out: %lld stat1: %d stat2: %d\n", u64t1, u64t2, stat1, stat2);
    printf("I64 Test\n");
#endif

    I64 i64t1 = 0x0123456789ABCDEF;
    I64 i64t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 8);
    ASSERT_EQ(0x01, ptr[0]);
    ASSERT_EQ(0x23, ptr[1]);
    ASSERT_EQ(0x45, ptr[2]);
    ASSERT_EQ(0x67, ptr[3]);
    ASSERT_EQ(0x89, ptr[4]);
    ASSERT_EQ(0xAB, ptr[5]);
    ASSERT_EQ(0xCD, ptr[6]);
    ASSERT_EQ(0xEF, ptr[7]);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i64t1, i64t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 8);

    // double check negative number
    i64t1 = -1000000000000;
    i64t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 8);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i64t1, i64t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 8);

#if DEBUG_VERBOSE
    printf("Val: in: %lld out: %lld stat1: %d stat2: %d\n", i64t1, i64t2, stat1, stat2);

    printf("F32 Test\n");
#endif

    F32 f32t1 = -1.23;
    F32 f32t2 = 0;

    // Test ints

    buff.resetSer();
    stat1 = buff.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 4);
    ASSERT_EQ(0xBF, ptr[0]);
    ASSERT_EQ(0x9D, ptr[1]);
    ASSERT_EQ(0x70, ptr[2]);
    ASSERT_EQ(0xA4, ptr[3]);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_FLOAT_EQ(f32t1, f32t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 4);

#if DEBUG_VERBOSE
    printf("Val: in: %f out: %f stat1: %d stat2: %d\n", f32t1, f32t2, stat1, stat2);
    printf("F64 Test\n");
#endif

    F64 f64t1 = 100.232145345346534;
    F64 f64t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    Fw::SerializeBufferBaseTester::verifySerLocEq(buff, 8);
    ASSERT_EQ(0x40, ptr[0]);
    ASSERT_EQ(0x59, ptr[1]);
    ASSERT_EQ(0x0E, ptr[2]);
    ASSERT_EQ(0xDB, ptr[3]);
    ASSERT_EQ(0x78, ptr[4]);
    ASSERT_EQ(0x26, ptr[5]);
    ASSERT_EQ(0x8B, ptr[6]);
    ASSERT_EQ(0xA6, ptr[7]);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_DOUBLE_EQ(f64t1, f64t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, 8);

#if DEBUG_VERBOSE
    printf("Val: in: %lf out: %lf stat1: %d stat2: %d\n", f64t1, f64t2, stat1, stat2);
    printf("bool Test\n");
#endif

    bool boolt1 = true;
    bool boolt2 = false;

    buff.resetSer();
    stat1 = buff.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(boolt1, boolt2);

#if DEBUG_VERBOSE
    printf("Val: in: %s out: %s stat1: %d stat2: %d\n", boolt1 ? "TRUE" : "FALSE", boolt2 ? "TRUE" : "FALSE", stat1,
           stat2);
    printf("Pointer Test\n");
#endif

    U32 u32Var = 0;
    void* ptrt1 = &u32Var;
    void* ptrt2 = nullptr;

    buff.resetSer();
    stat1 = buff.serialize(ptrt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat2 = buff.deserialize(ptrt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(ptrt1, ptrt2);

#if DEBUG_VERBOSE
    printf("Val: in: %p out: %p stat1: %d stat2: %d\n", ptrt1, ptrt2, stat1, stat2);

    printf("Skip deserialization Tests\n");
#endif

    // Test sizes

    FwSizeType size1 = std::numeric_limits<FwSizeStoreType>::max();
    FwSizeType size2 = 0;

    buff.resetSer();
    stat1 = buff.serializeSize(size1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat2 = buff.deserializeSize(size2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u64t1, u64t2);
    Fw::SerializeBufferBaseTester::verifyDeserLocEq(buff, sizeof(FwSizeStoreType));

#if DEBUG_VERBOSE
    printf("Val: in: %" PRI_FwSizeType " out: %" PRI_FwSizeType " stat1: %d stat2: %d\n", size1, size2, stat1, stat2);
    printf("Size Test\n");
#endif

    // Test skipping:

    buff.resetSer();
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat2 = buff.serialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);

    // should fail:
    stat1 = buff.deserializeSkip(10);
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH, stat1);

    // skip everything:
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat2 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);

    // should fail:
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, stat1);

    // skip half/read half:
    buff.resetDeser();
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    U32 u32val;
    stat2 = buff.deserialize(u32val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u32t2, u32val);

#if DEBUG_VERBOSE
    printf("\nDeserialization Tests\n");
#endif

    SerializeTestBuffer buff2;

    // Do a series of serializations
    u8t2 = 0;
    i8t2 = 0;
    u16t2 = 0;
    i16t2 = 0;
    u32t2 = 0;
    i32t2 = 0;
    u64t2 = 0;
    i64t2 = 0;
    f32t2 = 0.0;
    f64t2 = 0.0;
    boolt2 = false;
    ptrt2 = nullptr;

    buff.resetSer();
    stat1 = buff.serialize(u8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(i8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(i64t1);
    printf("i64t1 in stat: %d\n", stat1);
    stat1 = buff.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff.serialize(ptrt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);

    // TKC - commented out due to fprime-util choking on output
    // std::cout << "Buffer contents: " << buff << std::endl;

    // Serialize second buffer and test for equality
    buff2.resetSer();
    stat1 = buff2.serialize(u8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(i8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(i64t1);
    printf("i64t1 in stat: %d\n", stat1);
    stat1 = buff2.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);
    stat1 = buff2.serialize(ptrt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat1);

    ASSERT_EQ(buff, buff2);

    // deserialize

    stat2 = buff.deserialize(u8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u8t1, u8t2);
    stat2 = buff.deserialize(i8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i8t1, i8t2);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u16t1, u16t2);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i16t1, i16t2);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u32t1, u32t2);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i32t1, i32t2);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u64t1, u64t2);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i64t1, i64t2);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_FLOAT_EQ(f32t1, f32t2);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_DOUBLE_EQ(f64t1, f64t2);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(boolt1, boolt2);
    stat2 = buff.deserialize(ptrt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(ptrt1, ptrt2);

// reset and deserialize again
#if DEBUG_VERBOSE
    printf("\nReset and deserialize again.\n");
#endif

    buff.resetDeser();

    u8t2 = 0;
    i8t2 = 0;
    u16t2 = 0;
    i16t2 = 0;
    u32t2 = 0;
    i32t2 = 0;
    u64t2 = 0;
    i64t2 = 0;
    f32t2 = 0.0;
    f64t2 = 0.0;
    boolt2 = false;
    ptrt2 = nullptr;

    stat2 = buff.deserialize(u8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u8t1, u8t2);
    stat2 = buff.deserialize(i8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i8t1, i8t2);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u16t1, u16t2);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i16t1, i16t2);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u32t1, u32t2);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i32t1, i32t2);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(u64t1, u64t2);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(i64t1, i64t2);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_FLOAT_EQ(f32t1, f32t2);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_DOUBLE_EQ(f64t1, f64t2);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(boolt1, boolt2);
    stat2 = buff.deserialize(ptrt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, stat2);
    ASSERT_EQ(ptrt1, ptrt2);

    // serialize string
    Fw::String str1;
    Fw::String str2;

    str1 = "Foo";
    str2 = "BarBlat";
    buff.resetSer();
    str1.serialize(buff);
    str2.deserialize(buff);
    ASSERT_EQ(str1, str2);

    // serialize string template
    Fw::StringTemplate<80> strTmpl1("Foo");
    Fw::StringTemplate<80> strTmpl2("Bar");
    buff.resetSer();
    strTmpl1.serialize(buff);
    strTmpl2.deserialize(buff);
    ASSERT_EQ(strTmpl1, strTmpl2);
}

struct TestStruct {
    U32 m_u32;
    U16 m_u16;
    U8 m_u8;
    F32 m_f32;
    U8 m_buff[25];
};

class MySerializable : public Fw::Serializable {
  public:
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const {
        buffer.serialize(m_testStruct.m_u32);
        buffer.serialize(m_testStruct.m_u16);
        buffer.serialize(m_testStruct.m_u8);
        buffer.serialize(m_testStruct.m_f32);
        buffer.serialize(m_testStruct.m_buff, sizeof(m_testStruct.m_buff));
        return Fw::FW_SERIALIZE_OK;
    }

    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) {
        buffer.serialize(m_testStruct.m_buff, sizeof(m_testStruct.m_buff));
        buffer.serialize(m_testStruct.m_f32);
        buffer.serialize(m_testStruct.m_u8);
        buffer.serialize(m_testStruct.m_u16);
        buffer.serialize(m_testStruct.m_u32);
        return Fw::FW_SERIALIZE_OK;
    }

  private:
    TestStruct m_testStruct;
};

TEST(PerformanceTest, SerPerfTest) {
    Os::IntervalTimer timer;

    MySerializable in;
    MySerializable out;
    SerializeTestBuffer buff;

    timer.start();

    I32 iterations = 1000000;
    for (I32 iter = 0; iter < iterations; iter++) {
        in.serialize(buff);
        out.deserialize(buff);
    }

    timer.stop();

    printf("%d iterations took %d us (%f each).\n", iterations, timer.getDiffUsec(),
           static_cast<F64>(timer.getDiffUsec()) / iterations);
}

TEST(PerformanceTest, StructCopyTest) {
    char buff[sizeof(TestStruct)];
    TestStruct ts;

    Os::IntervalTimer timer;

    timer.start();

    I32 iterations = 1000000;
    for (I32 iter = 0; iter < iterations; iter++) {
        // simulate the incoming MSL-style call by doing member assignments
        ts.m_u32 = 0;
        ts.m_u16 = 0;
        ts.m_u8 = 0;
        ts.m_f32 = 0.0;
        memcpy(ts.m_buff, "1234567890123456789012345", sizeof(ts.m_buff));

        memcpy(buff, &ts, sizeof(ts));
        memcpy(&ts, buff, sizeof(buff));
    }

    timer.stop();

    printf("%d iterations took %d us (%f each).\n", iterations, timer.getDiffUsec(),
           static_cast<F64>(timer.getDiffUsec()) / iterations);
}

TEST(PerformanceTest, ClassCopyTest) {
    char buff[sizeof(MySerializable)];
    MySerializable ms;

    Os::IntervalTimer timer;

    timer.start();

    I32 iterations = 1000000;
    for (I32 iter = 0; iter < iterations; iter++) {
        memcpy(buff, reinterpret_cast<void*>(&ms), sizeof(ms));
        memcpy(reinterpret_cast<void*>(&ms), buff, sizeof(buff));
    }

    timer.stop();

    printf("%d iterations took %d us (%f each).\n", iterations, timer.getDiffUsec(),
           static_cast<F64>(timer.getDiffUsec()) / iterations);
}

void printSizes() {
    printf("Sizeof TestStruct: %lu\n", sizeof(TestStruct));
    printf("Sizeof MySerializable: %lu\n", sizeof(MySerializable));
}

void AssertTest() {
    printf("Assert Tests\n");

    // Since native FW_ASSERT actually asserts,
    // manually test by setting arguments to
    // unequal values below one by one
    //    FW_ASSERT(0 == 1);
    //    FW_ASSERT(0 == 1, 1);
    //    FW_ASSERT(0 == 1, 1, 2);
    //    FW_ASSERT(0 == 1, 1, 2, 3);
    //    FW_ASSERT(0 == 1, 1, 2, 3, 4);
    //    FW_ASSERT(0 == 1, 1, 2, 3, 4, 5);
    //    FW_ASSERT(0 == 1, 1, 2, 3, 4, 5, 6);

    // Define an Assert handler
    class TestAssertHook : public Fw::AssertHook {
      public:
        TestAssertHook() {}
        virtual ~TestAssertHook() {}
        void reportAssert(FILE_NAME_ARG file,
                          FwSizeType lineNo,
                          FwSizeType numArgs,
                          FwAssertArgType arg1,
                          FwAssertArgType arg2,
                          FwAssertArgType arg3,
                          FwAssertArgType arg4,
                          FwAssertArgType arg5,
                          FwAssertArgType arg6) {
            this->m_file = file;
            this->m_lineNo = lineNo;
            this->m_numArgs = numArgs;
            this->m_arg1 = arg1;
            this->m_arg2 = arg2;
            this->m_arg3 = arg3;
            this->m_arg4 = arg4;
            this->m_arg5 = arg5;
            this->m_arg6 = arg6;
        };

        void doAssert() { this->m_asserted = true; }

        FILE_NAME_ARG getFile() { return this->m_file; }

        FwSizeType getLineNo() { return this->m_lineNo; }

        FwSizeType getNumArgs() { return this->m_numArgs; }

        FwAssertArgType getArg1() { return this->m_arg1; }

        FwAssertArgType getArg2() { return this->m_arg2; }

        FwAssertArgType getArg3() { return this->m_arg3; }

        FwAssertArgType getArg4() { return this->m_arg4; }

        FwAssertArgType getArg5() { return this->m_arg5; }

        FwAssertArgType getArg6() { return this->m_arg6; }

        bool asserted() {
            bool didAssert = this->m_asserted;
            this->m_asserted = false;
            return didAssert;
        }

      private:
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        FILE_NAME_ARG m_file = 0;
#else
        FILE_NAME_ARG m_file = nullptr;
#endif
        FwSizeType m_lineNo = 0;
        FwSizeType m_numArgs = 0;
        FwAssertArgType m_arg1 = 0;
        FwAssertArgType m_arg2 = 0;
        FwAssertArgType m_arg3 = 0;
        FwAssertArgType m_arg4 = 0;
        FwAssertArgType m_arg5 = 0;
        FwAssertArgType m_arg6 = 0;
        bool m_asserted = false;
    };

    // register the class
    TestAssertHook hook;
    hook.registerHook();

    // issue an assert
    FW_ASSERT(0);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(0u, hook.getNumArgs());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(1u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1, 2);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(2u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
    ASSERT_EQ(2u, hook.getArg2());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1, 2, 3);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(3u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
    ASSERT_EQ(2u, hook.getArg2());
    ASSERT_EQ(3u, hook.getArg3());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1, 2, 3, 4);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(4u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
    ASSERT_EQ(2u, hook.getArg2());
    ASSERT_EQ(3u, hook.getArg3());
    ASSERT_EQ(4u, hook.getArg4());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1, 2, 3, 4, 5);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(5u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
    ASSERT_EQ(2u, hook.getArg2());
    ASSERT_EQ(3u, hook.getArg3());
    ASSERT_EQ(4u, hook.getArg4());
    ASSERT_EQ(5u, hook.getArg5());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif

    // issue an assert
    FW_ASSERT(0, 1, 2, 3, 4, 5, 6);
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(6u, hook.getNumArgs());
    ASSERT_EQ(1u, hook.getArg1());
    ASSERT_EQ(2u, hook.getArg2());
    ASSERT_EQ(3u, hook.getArg3());
    ASSERT_EQ(4u, hook.getArg4());
    ASSERT_EQ(5u, hook.getArg5());
    ASSERT_EQ(6u, hook.getArg6());
#else
    // assert does not fire when asserts are off
    ASSERT_FALSE(hook.asserted());
#endif
}

TEST(TypesTest, CheckAssertTest) {
    AssertTest();
}

TEST(TypesTest, PolyTest) {
    Fw::String str;

    // U8 Type  ===============================================================
    U8 in8 = 13;
    U8 out8;

    Fw::PolyType pt(in8);

    out8 = static_cast<U8>(pt);
    ASSERT_EQ(in8, out8);

    // Test assigning to polytype and return type of assignment
    in8 = 218;
    // Can assign Polytype to U8 via overridden cast operator
    out8 = (pt = in8);
    ASSERT_EQ(static_cast<U8>(pt), 218u);
    ASSERT_EQ(static_cast<U8>(pt), in8);
    ASSERT_EQ(out8, in8);

#if FW_SERIALIZABLE_TO_STRING
    pt.toString(str);
    ASSERT_STREQ(str.toChar(), "218 ");
#endif

    // U16 Type  ==============================================================
    U16 inU16 = 34;
    U16 outU16;
    Fw::PolyType ptU16(inU16);

    outU16 = static_cast<U16>(ptU16);
    ASSERT_EQ(inU16, outU16);

    inU16 = 45000;
    outU16 = (ptU16 = inU16);
    ASSERT_EQ(static_cast<U16>(ptU16), inU16);
    ASSERT_EQ(outU16, inU16);

#if FW_SERIALIZABLE_TO_STRING
    ptU16.toString(str);
    ASSERT_STREQ(str.toChar(), "45000 ");
#endif

    // U32 Type  ==============================================================
    U32 inU32 = 89;
    U32 outU32;
    Fw::PolyType ptU32(inU32);

    outU32 = static_cast<U32>(ptU32);
    ASSERT_EQ(inU32, outU32);

    inU32 = 3222111000;
    outU32 = (ptU32 = inU32);
    ASSERT_EQ(static_cast<U32>(ptU32), inU32);
    ASSERT_EQ(outU32, inU32);

#if FW_SERIALIZABLE_TO_STRING
    ptU32.toString(str);
    ASSERT_STREQ(str.toChar(), "3222111000 ");
#endif

    // U64 Type  ==============================================================
    U64 inU64 = 233;
    U64 outU64;
    Fw::PolyType ptU64(inU64);

    outU64 = static_cast<U64>(ptU64);
    ASSERT_EQ(inU64, outU64);

    inU64 = 555444333222111;
    outU64 = (ptU64 = inU64);
    ASSERT_EQ(static_cast<U64>(ptU64), inU64);
    ASSERT_EQ(outU64, inU64);

#if FW_SERIALIZABLE_TO_STRING
    ptU64.toString(str);
    ASSERT_STREQ(str.toChar(), "555444333222111 ");
#endif

    // I8 Type  ===============================================================
    I8 inI8 = 2;
    I8 outI8;
    Fw::PolyType ptI8(inI8);

    outI8 = static_cast<I8>(ptI8);
    ASSERT_EQ(inI8, outI8);

    inI8 = -3;
    outI8 = (ptI8 = inI8);
    ASSERT_EQ(static_cast<I8>(ptI8), inI8);
    ASSERT_EQ(outI8, inI8);

#if FW_SERIALIZABLE_TO_STRING
    ptI8.toString(str);
    ASSERT_STREQ(str.toChar(), "-3 ");
#endif

    // I16 Type  ==============================================================
    I16 inI16 = 5;
    I16 outI16;
    Fw::PolyType ptI16(inI16);

    outI16 = static_cast<I16>(ptI16);
    ASSERT_EQ(inI16, outI16);

    inI16 = -7;
    outI16 = (ptI16 = inI16);
    ASSERT_EQ(static_cast<I16>(ptI16), inI16);
    ASSERT_EQ(outI16, inI16);

#if FW_SERIALIZABLE_TO_STRING
    ptI16.toString(str);
    ASSERT_STREQ(str.toChar(), "-7 ");
#endif

    // I32 Type  ==============================================================
    I32 inI32 = 11;
    I32 outI32;
    Fw::PolyType ptI32(inI32);

    outI32 = static_cast<I32>(ptI32);
    ASSERT_EQ(inI32, outI32);

    inI32 = -13;
    outI32 = (ptI32 = inI32);
    ASSERT_EQ(static_cast<I32>(ptI32), inI32);
    ASSERT_EQ(outI32, inI32);

#if FW_SERIALIZABLE_TO_STRING
    ptI32.toString(str);
    ASSERT_STREQ(str.toChar(), "-13 ");
#endif

    // I64 Type  ==============================================================
    I64 inI64 = 17;
    I64 outI64;
    Fw::PolyType ptI64(inI64);

    outI64 = static_cast<I64>(ptI64);
    ASSERT_EQ(inI64, outI64);

    inI64 = -19;
    outI64 = (ptI64 = inI64);
    ASSERT_EQ(static_cast<I64>(ptI64), inI64);
    ASSERT_EQ(outI64, inI64);

#if FW_SERIALIZABLE_TO_STRING
    ptI64.toString(str);
    ASSERT_STREQ(str.toChar(), "-19 ");
#endif

    // F32 Type  ==============================================================
    F32 inF32 = 23.32;
    F32 outF32;
    Fw::PolyType ptF32(inF32);

    outF32 = static_cast<F32>(ptF32);
    ASSERT_EQ(inF32, outF32);

    inF32 = 29.92;
    outF32 = (ptF32 = inF32);
    ASSERT_EQ(static_cast<F32>(ptF32), inF32);
    ASSERT_EQ(outF32, inF32);

    // F64 Type  ==============================================================
    F64 inF64 = 31.13;
    F64 outF64;
    Fw::PolyType ptF64(inF64);

    outF64 = static_cast<F64>(ptF64);
    ASSERT_EQ(inF64, outF64);

    inF64 = 37.73;
    outF64 = (ptF64 = inF64);
    ASSERT_EQ(static_cast<F64>(ptF64), inF64);
    ASSERT_EQ(outF64, inF64);

    // bool Type  =============================================================
    bool inbool = true;
    bool outbool;
    Fw::PolyType ptbool(inbool);

    outbool = static_cast<bool>(ptbool);
    ASSERT_EQ(inbool, outbool);

    inbool = false;
    outbool = (ptbool = inbool);
    ASSERT_EQ(static_cast<bool>(ptbool), inbool);
    ASSERT_EQ(outbool, inbool);

    // ptr Type  ==============================================================
    void* inPtr = &ptbool;
    void* outPtr;
    Fw::PolyType ptPtr(inPtr);

    outPtr = static_cast<void*>(ptPtr);
    ASSERT_EQ(inPtr, outPtr);

    inPtr = &ptF64;
    outPtr = (ptPtr = inPtr);
    ASSERT_EQ(static_cast<void*>(ptPtr), inPtr);
    ASSERT_EQ(outPtr, inPtr);
}

TEST(TypesTest, StringTest) {
    Fw::String str;
    str = "foo";
    Fw::String str2;
    str2 = "foo";
    ASSERT_EQ(str, str2);
    ASSERT_EQ(str, "foo");
    str2 = "doodie";
    ASSERT_NE(str, str2);

    Fw::String str3(str);
    str3 += str2;
    ASSERT_EQ(str3, "foodoodie");

    str3 += "hoo";
    ASSERT_EQ(str3, "foodoodiehoo");

    Fw::String copyStr("ASTRING");
    ASSERT_EQ(copyStr, "ASTRING");
    Fw::String copyStr2("ASTRING");
    ASSERT_EQ(copyStr2, "ASTRING");
    Fw::String copyStr3(copyStr2);
    ASSERT_EQ(copyStr3, "ASTRING");

    Fw::InternalInterfaceString ifstr("IfString");
    Fw::String if2(ifstr);

    ASSERT_EQ(ifstr, if2);
    ASSERT_EQ(if2, "IfString");

    std::cout << "Stream: " << str2 << std::endl;

    char buffer[Fw::String::STRING_SIZE];
    Fw::ExternalString es(buffer, sizeof buffer, "ExternalString");
    Fw::ObjectName es2(es);

    ASSERT_EQ(es, es2);
    ASSERT_EQ(es2, "ExternalString");

}

TEST(TypesTest, ObjectNameTest) {
    Fw::ObjectName str;
    str = "foo";
    Fw::ObjectName str2;
    str2 = "foo";
    ASSERT_EQ(str, str2);
    ASSERT_EQ(str, "foo");
    str2 = "_bar";
    ASSERT_NE(str, str2);

    Fw::ObjectName str3(str);
    str3 += str2;
    ASSERT_EQ(str3, "foo_bar");

    str3 += "_foo";
    ASSERT_EQ(str3, "foo_bar_foo");

    Fw::ObjectName copyStr("ASTRING");
    ASSERT_EQ(copyStr, "ASTRING");
    Fw::ObjectName copyStr2(copyStr);
    ASSERT_EQ(copyStr2, "ASTRING");

    char buffer[Fw::ObjectName::STRING_SIZE];
    Fw::ExternalString es(buffer, sizeof buffer, "ExternalString");
    Fw::ObjectName es2(es);

    ASSERT_EQ(es, es2);
    ASSERT_EQ(es2, "ExternalString");
}

TEST(TypesTest, StringFormatTest) {
    Fw::String str;
    str.format("Int %d String %s", 10, "foo");
    ASSERT_STREQ(str.toChar(), "Int 10 String foo");
}

TEST(TypesTest, FormatSpecifierTest) {
    Fw::String str;

    U8 numU8 = 10;
    str.format("U8: %" PRI_U8, numU8);
    ASSERT_STREQ(str.toChar(), "U8: 10");

    I8 numI8 = -10;
    str.format("I8: %" PRI_I8, numI8);
    ASSERT_STREQ(str.toChar(), "I8: -10");

    #if FW_HAS_16_BIT
    U16 numU16 = 10;
    str.format("U16: %" PRI_U16, numU16);
    ASSERT_STREQ(str.toChar(), "U16: 10");

    I16 numI16 = -10;
    str.format("I16: %" PRI_I16, numI16);
    ASSERT_STREQ(str.toChar(), "I16: -10");
    #endif

    #if FW_HAS_32_BIT
    U32 numU32 = 10;
    str.format("U32: %" PRI_U32, numU32);
    ASSERT_STREQ(str.toChar(), "U32: 10");

    I32 numI32 = -10;
    str.format("I32: %" PRI_I32, numI32);
    ASSERT_STREQ(str.toChar(), "I32: -10");
    #endif

    #if FW_HAS_64_BIT
    U64 numU64 = 10;
    str.format("U64: %" PRI_U64, numU64);
    ASSERT_STREQ(str.toChar(), "U64: 10");

    I64 numI64 = -10;
    str.format("I64: %" PRI_I64, numI64);
    ASSERT_STREQ(str.toChar(), "I64: -10");
    #endif

    F32 numF32 = 12.3456789;
    str.format("F32: %" PRI_F64, static_cast<double>(numF32));
    ASSERT_STREQ(str.toChar(), "F32: 12.345679");

    #if FW_HAS_F64
    F64 numF64 = 12.3456789;
    str.format("F64: %" PRI_F64, numF64);
    ASSERT_STREQ(str.toChar(), "F64: 12.345679");
    #endif

    char c = 'A';
    str.format("CHAR: %" PRI_CHAR, c);
    ASSERT_STREQ(str.toChar(), "CHAR: A");
}

TEST(PerformanceTest, F64SerPerfTest) {
    SerializeTestBuffer buff;

#if DEBUG_VERBOSE
    printf("U8 Test\n");
#endif

    F64 in = 10000.0;
    F64 out = 0;

    FwSizeType iters = 1000000;

    Os::IntervalTimer timer;
    timer.start();

    for (FwSizeType iter = 0; iter < iters; iter++) {
        buff.resetSer();
        buff.serialize(in);
        buff.deserialize(out);
    }

    timer.stop();

    printf("%" PRI_FwSizeType " iterations took %d us (%f us each).\n", iters, timer.getDiffUsec(),
           static_cast<F64>(timer.getDiffUsec()) / static_cast<F64>(iters));
}

TEST(AllocatorTest, MallocAllocatorTest) {
    // Since it is a wrapper around malloc, the test consists of requesting
    // memory and verifying a non-zero pointer, unchanged size, and not recoverable.
    Fw::MallocAllocator allocator;
    FwSizeType size = 100;  // one hundred bytes
    bool recoverable;
    void* ptr = allocator.allocate(10, size, recoverable);
    ASSERT_EQ(100, size);
    ASSERT_NE(ptr, nullptr);
    ASSERT_FALSE(recoverable);
    // deallocate memory
    allocator.deallocate(100, ptr);
}

TEST(Nominal, string_copy) {
    const char* copy_string = "abc123\n";  // Length of 7
    char buffer_out_test[10];
    char buffer_out_truth[10];

    char* out_truth = ::strncpy(buffer_out_truth, copy_string, static_cast<FwSizeType>(sizeof(buffer_out_truth)));
    char* out_test = Fw::StringUtils::string_copy(buffer_out_test, copy_string, static_cast<FwSizeType>(sizeof(buffer_out_test)));

    ASSERT_EQ(sizeof(buffer_out_truth), sizeof(buffer_out_test)) << "Buffer size mismatch";

    // Check the outputs, both should return the input buffer
    ASSERT_EQ(out_truth, buffer_out_truth) << "strncpy didn't return expected value";
    ASSERT_EQ(out_test, buffer_out_test) << "string_copy didn't return expected value";

    // Check string correct
    ASSERT_STREQ(out_test, copy_string) << "Strings not equal from strncpy";
    ASSERT_STREQ(out_test, out_truth) << "Copied strings differ from strncpy";

    // Should output 0s for the remaining buffer
    for (U32 i = ::strnlen(buffer_out_truth, sizeof(buffer_out_truth)); i < static_cast<U32>(sizeof(buffer_out_truth));
         i++) {
        ASSERT_EQ(buffer_out_truth[i], 0) << "strncpy didn't output 0 fill";
        ASSERT_EQ(buffer_out_test[i], 0) << "string_copy didn't output 0 fill";
    }
}

TEST(OffNominal, string_copy) {
    const char* copy_string = "abc123\n";  // Length of 7
    char buffer_out_test[sizeof(copy_string) - 1];
    char buffer_out_truth[sizeof(copy_string) - 1];

    char* out_truth = ::strncpy(buffer_out_truth, copy_string, static_cast<FwSizeType>(sizeof(buffer_out_truth)));
    char* out_test = Fw::StringUtils::string_copy(buffer_out_test, copy_string, static_cast<FwSizeType>(sizeof(buffer_out_test)));

    ASSERT_EQ(sizeof(buffer_out_truth), sizeof(buffer_out_test)) << "Buffer size mismatch";

    // Check the outputs, both should return the input buffer
    ASSERT_EQ(out_truth, buffer_out_truth) << "strncpy didn't return expected value";
    ASSERT_EQ(out_test, buffer_out_test) << "string_copy didn't return expected value";

    // Check string correct up to last digit
    U32 i = 0;
    ASSERT_STRNE(out_test, out_truth) << "Strings not equal";
    for (i = 0; i < static_cast<U32>(sizeof(copy_string)) - 2; i++) {
        ASSERT_EQ(out_test[i], out_truth[i]);
    }
    ASSERT_EQ(out_truth[i], '\n') << "strncpy did not error as expected";
    ASSERT_EQ(out_test[i], 0) << "string_copy didn't properly null terminate";
}

TEST(Nominal, string_len) {
    const char* test_string = "abc123";
    ASSERT_EQ(Fw::StringUtils::string_length(test_string, static_cast<FwSizeType>(50)), 6);
    ASSERT_EQ(Fw::StringUtils::string_length(test_string, static_cast<FwSizeType>(3)), 3);
}

TEST(OffNominal, string_len_zero) {
    const char* test_string = "abc123";
    ASSERT_EQ(Fw::StringUtils::string_length(test_string, static_cast<FwSizeType>(0)), 0);
}

TEST(OffNominal, sub_string_no_match) {
    const char* source_string = "abc123";
    const char* sub_string = "456";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),-1);
}

TEST(Nominal, sub_string_match_begin) {
    const char* source_string = "abc123";
    const char* sub_string = "abc";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),0);
}

TEST(Nominal, sub_string_match_end) {
    const char* source_string = "abc123";
    const char* sub_string = "123";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),3);
}

TEST(Nominal, sub_string_match_partway_1) {
    const char* source_string = "abc123";
    const char* sub_string = "c12";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),2);
}

TEST(OffNominal, sub_string_partial_match_begin) {
    const char* source_string = "abc123";
    const char* sub_string = "ab1";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),-1);
}

TEST(OffNominal, sub_string_partial_match_middle) {
    const char* source_string = "abc123";
    const char* sub_string = "c13";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),-1);
}

TEST(OffNominal, sub_string_partial_match_end) {
    const char* source_string = "abc123";
    const char* sub_string = "234";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),-1);
}

TEST(Nominal, sub_string_exact_match) {
    const char* source_string = "abc123";
    const char* sub_string = "abc123";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,3),0);
}

TEST(OffNominal, sub_string_source_zero_size) {
    const char* source_string = "";
    const char* sub_string = "234";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,0,sub_string,3),-1);
}

TEST(OffNominal, sub_string_substring_zero_size) {
    const char* source_string = "abc123";
    const char* sub_string = "";
    ASSERT_EQ(Fw::StringUtils::substring_find(source_string,6,sub_string,0),0);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
