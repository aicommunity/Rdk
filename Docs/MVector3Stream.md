# MVector<T,3> standard stream contract

The specialized standard stream overloads in Core/Math/MUIOStream.h referred
to nonexistent x/y/z fields. Instantiating the formatter for inherited Coord
in the real TimeLearner integration tests failed on both worker compilers.

Use the existing public element accessor operator()(index). Preserve the
brace/comma representation and the caller's stream precision/formatting.
Narrow and wide streams use the corresponding CharT delimiters. Input requires
three values and valid delimiters; malformed input sets failbit and preserves
the complete previous vector instead of partially assigning coordinates.

This does not change MDMatrix XML precision or physical integration. General
MVector dimensions and other legacy stream types are outside this scoped fix.
Server build, six production-header stream tests and native Coord/Activity/
frequency checks are pending. The original compilation failures are retained.
