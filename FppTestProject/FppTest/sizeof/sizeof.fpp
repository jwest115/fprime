constant SizeofU8 = sizeof(U8)
constant SizeofU16 = sizeof(U16)
constant SizeofU32 = sizeof(U32)
constant SizeofU64 = sizeof(U64)
constant SizeofI8 = sizeof(I8)
constant SizeofI16 = sizeof(I16)
constant SizeofI32 = sizeof(I32)
constant SizeofI64 = sizeof(I64)
constant SizeofF32 = sizeof(F32)
constant SizeofF63 = sizeof(F64)
constant SizeofBool = sizeof(bool)


type StringDefaultAlias = string
constant SizeofStringDefaultAlias = sizeof(StringDefaultAlias)
type StringAlias = string size 100
constant SizeofStringAlias = sizeof(StringAlias)


type U64Alias = U64
constant SizeofU64Alias = sizeof(U64Alias)
type EnumAlias = E
constant SizeofEnumAlias = sizeof(EnumAlias)

array A = [3] U64
constant SizeofArray = sizeof(A)

enum E: U8 {
    X,
    Y,
    Z
}
constant SizeofEnum = sizeof(E)

struct S {
    m1: A,
    m2: F64
    m4: string size 10
}
constant SizeofStruct = sizeof(S)
