#pragma once

#include <cmath>

class MemoryPool
{
    public:
        MemoryPool(int size) : pool(new char[size]), offset(0) {}
        virtual ~MemoryPool() { delete[] pool; }

        /*template<typename T, typename...Args>
        T* Allocate(Args&&...args)
        {
            std::cout << "alignment: " << std::alignment_of<T>::value << " sizeof: " << sizeof(T) << std::endl;
            offset += sizeof(T);
            return new(pool + offset) T(args...);
        }*/

        template<typename T, typename...Args>
        T* Allocate(Args&&...args)
        {
            short totalBytesNeeded = sizeof(AllocationHeader) + sizeof(T);
            int byteOffset = 0;
            
            AllocationHeader* newHeader = nullptr;
            T* newObject = nullptr;

            // Get the latest header
            if (offset == 0)
            {
                newHeader = new(pool) AllocationHeader();
                byteOffset = sizeof(AllocationHeader);
            }
            else
            {
                AllocationHeader* header = (AllocationHeader*) pool[0];
                while (header->nextHeader != nullptr)
                {
                    byteOffset += header->totalAllocatedBytes;
                    header = header->nextHeader;
                }
            }

            if (newHeader == nullptr)
                throw std::runtime_error("This should never happen");

            //Figure out the alignment
            short alignmentRequired = std::alignment_of<T>::value;
            if (byteOffset % alignmentRequired == 0)
            {
                newObject = new(pool+byteOffset) T(args...);
            }
            else
            {
                int additionalOffset = 0;
                if (byteOffset < alignmentRequired)
                    additionalOffset = alignmentRequired - byteOffset;
                else
                    additionalOffset = (byteOffset % alignmentRequired);

                totalBytesNeeded += additionalOffset;
                newObject = new(pool+byteOffset+additionalOffset) T(args...);
            }

            newHeader->totalAllocatedBytes = totalBytesNeeded;
            newHeader->ptr = newObject;

            return newObject;
        }

        template<typename T>
        void Deallocate(T* t)
        {
            //if (std::is_trivially_destructible<T>::value == false)
            t->~T();
        }

    protected:
        char* pool;
        int offset;

        class AllocationHeader
        {
            friend class MemoryPool;
            void* ptr;
            unsigned short totalAllocatedBytes;
            AllocationHeader* nextHeader;
        };
};
