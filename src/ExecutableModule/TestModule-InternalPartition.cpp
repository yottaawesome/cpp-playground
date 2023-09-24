// MSVC is retarded and requires manually setting the compile option
// for internal partition units to work correctly.
// See https://developercommunity.visualstudio.com/t/Cannot-import-partition-implementation-u/10405337
module TestModule:InternalPartition;
