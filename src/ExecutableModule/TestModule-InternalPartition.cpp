// MSVC is retarded and requires manually setting the compile option
// for internal partition units to work correctly.
// See https://developercommunity.visualstudio.com/t/Cannot-import-partition-implementation-u/10405337
// and https://developercommunity.visualstudio.com/t/C20-Modules-Module-Partition-Impleme/1634347
// The main difference between internal module partitions and module
// partitions is that internal module partitions do not have the 
// export keyword and cannot export any symbols.
module TestModule:InternalPartition;

// Stuff in here is only visible inside the module
namespace NotExported
{
	constexpr int Blah = 1;
}