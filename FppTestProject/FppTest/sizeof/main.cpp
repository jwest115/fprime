// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include "FppTest/sizeof/AArrayAc.hpp"
#include "FppTest/sizeof/A2ArrayAc.hpp"
#include "FppTest/sizeof/FppConstantsAc.hpp"
#include "FppTest/sizeof/EEnumAc.hpp"
#include "FppTest/sizeof/SSerializableAc.hpp"
#include "FppTest/sizeof/S2SerializableAc.hpp"
#include "FppTest/sizeof/StringAliasAliasAc.hpp"
#include "FppTest/sizeof/StringDefaultAliasAliasAc.hpp"
#include "FppTest/sizeof/U64AliasAliasAc.hpp"
#include "FppTest/sizeof/EnumAliasAliasAc.hpp"
#include <gtest/gtest.h>

TEST(SizeofTest, Primitives) {
    ASSERT_TRUE(SizeofU8 == sizeof(U8));
    ASSERT_TRUE(SizeofU16 == sizeof(U16));
    ASSERT_TRUE(SizeofU32 == sizeof(U32));
    ASSERT_TRUE(SizeofU64 == sizeof(U64));
    ASSERT_TRUE(SizeofI8 == sizeof(I8));
    ASSERT_TRUE(SizeofI16 == sizeof(I16));
    ASSERT_TRUE(SizeofI32 == sizeof(I32));
    ASSERT_TRUE(SizeofI64 == sizeof(I64));
    ASSERT_TRUE(SizeofF32 == sizeof(F32));
    ASSERT_TRUE(SizeofF64 == sizeof(F64));
    ASSERT_TRUE(SizeofBool == sizeof(bool));
}

TEST(SizeofTest, AliasType) {
    ASSERT_TRUE(SizeofStringAlias == 102);
    ASSERT_TRUE(SizeofStringDefaultAlias == 258);
    ASSERT_TRUE(SizeofU64Alias == sizeof(U64Alias));
    ASSERT_TRUE(SizeofEnumAlias == EnumAlias::SERIALIZED_SIZE && SizeofEnumAlias == 2);
}

TEST(SizeofTest, Array) {
    ASSERT_TRUE(SizeofArray == A::SERIALIZED_SIZE && SizeofArray == 24);
    ASSERT_TRUE(SizeofArray2 == A2::SERIALIZED_SIZE && SizeofArray2 == 48);
}

TEST(SizeofTest, Enum) {
    ASSERT_TRUE(SizeofEnum == E::SERIALIZED_SIZE && SizeofEnum == 2);
}

TEST(SizeofTest, Struct) {
    ASSERT_TRUE(SizeofStruct == S::SERIALIZED_SIZE && SizeofStruct == 94);
    ASSERT_TRUE(SizeofStruct2 == S2::SERIALIZED_SIZE && SizeofStruct2 == 338);
}
