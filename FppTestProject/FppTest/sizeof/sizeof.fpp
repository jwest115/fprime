constant SizeofU8 = sizeof(U8)
constant SizeofU16 = sizeof(U16)
constant SizeofU32 = sizeof(U32)
constant SizeofU64 = sizeof(U64)
constant SizeofI8 = sizeof(I8)
constant SizeofI16 = sizeof(I16)
constant SizeofI32 = sizeof(I32)
constant SizeofI64 = sizeof(I64)
constant SizeofF32 = sizeof(F32)
constant SizeofF64 = sizeof(F64)
constant SizeofBool = sizeof(bool)

type StringDefaultAlias = string
constant SizeofStringDefaultAlias = sizeof(StringDefaultAlias)
type StringAlias = string size 100
constant SizeofStringAlias = sizeof(StringAlias)

type U64Alias = U64
constant SizeofU64Alias = sizeof(U64Alias)
type EnumAlias = E
constant SizeofEnumAlias = sizeof(EnumAlias)

array A = [3] I64
constant SizeofArray = sizeof(A)

array A2 = [2] A
constant SizeofArray2 = sizeof(A2)

enum E: U16 {
    X,
    Y,
    Z
}
constant SizeofEnum = sizeof(E)

struct S {
    m1: [2] A,
    m2: F64
    m3: [1] EnumAlias
    m4: [3] string size 10
}
constant SizeofStruct = sizeof(S)

struct S2 {
    m1: A2
    m2: StringAlias
    m3: [2] S
}
constant SizeofStruct2 = sizeof(S2)
