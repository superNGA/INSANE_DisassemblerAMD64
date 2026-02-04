//=========================================================================
//                      Arena Allocator
//=========================================================================
// by      : INSANE
// created : 04/02/2026
//
// purpose : General Purpose Arena Allocator, for fast memory allocations.
//-------------------------------------------------------------------------
#pragma once
#include <cassert>
#include <cstdint>
#include <vector>



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class ArenaAllocator_t
{
    public:
        ArenaAllocator_t(uint64_t iArenaSize = DEFAULT_ARENA_SIZE)
        {
            m_vecArenas.clear();
            m_vecManualAllocs.clear();

            m_iArenaSize = iArenaSize;
        }
        ArenaAllocator_t(const ArenaAllocator_t& other) = delete; // No copying.
 

        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline void* Allocate(size_t iSize)
        {
            return iSize <= m_iArenaSize ? AllocateToArena(iSize) : AllocateByMalloc(iSize);
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline uint64_t TotalSize() const
        {
            uint64_t iTotal = 0;

            // Total of all arenas.
            for(const Arena_t& arena : m_vecArenas)
                iTotal += arena.m_iUsedTill;

            // Total of all mallocs.
            for(const MallocedMem_t& mallocMem : m_vecManualAllocs)
                iTotal += mallocMem.m_iSize;

            return iTotal;
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline uint64_t TotalCapacity() const
        {
            uint64_t iTotal = 0;

            // Total of all arenas.
            for(const Arena_t& arena : m_vecArenas)
                iTotal += arena.m_iSize;

            // Total of all mallocs.
            for(const MallocedMem_t& mallocMem : m_vecManualAllocs)
                iTotal += mallocMem.m_iSize;

            return iTotal;
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline void ResetAllArena()
        {
            for(Arena_t& arena : m_vecArenas)
                arena.m_iUsedTill = 0;
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline void FreeAll()
        {
            for(MallocedMem_t& mallocMem : m_vecManualAllocs)
            {
                if(mallocMem.m_pMemory != nullptr)
                {
                    free(mallocMem.m_pMemory);
                }
            }
            m_vecManualAllocs.clear();


            for(Arena_t& arena : m_vecArenas)
            {
                if(arena.m_pMemory != nullptr)
                {
                    free(arena.m_pMemory);
                    arena.m_pMemory   = nullptr;
                    arena.m_iSize     = 0;
                    arena.m_iUsedTill = 0;
                }
            }
            m_vecArenas.clear();
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline size_t ArenaCount() const 
        {
            return m_vecArenas.size();
        }



    private:
        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline void* AllocateToArena(size_t iSize)
        {
            // Do we have any arenas?
            if(m_vecArenas.empty() == true)
            {
                m_vecArenas.emplace_back(Arena_t());
                m_vecArenas.back().InitializeArena(m_iArenaSize);
            }

            Arena_t* pArena = &m_vecArenas.back();


            // 8 byte aligned pos.
            uint64_t iAlignedPos = ((pArena->m_iUsedTill + sizeof(void*) - 1llu) & ~(sizeof(void*) - 1llu));
            uint64_t iNewPos     = iAlignedPos + iSize; // last valid byte @ iNewPos - 1. @ NewPos is no valid data.

            // Enough memory left in this arena?
            if(iNewPos <= pArena->m_iSize)
            {
                pArena->m_iUsedTill = static_cast<uint32_t>(iNewPos);
                return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(pArena->m_pMemory) + iAlignedPos);
            }


            // In case we don't have enough space, we need to allcoate one more arena.
            m_vecArenas.emplace_back(Arena_t());
            pArena = &m_vecArenas.back();
            pArena->InitializeArena(m_iArenaSize);
            pArena->m_iUsedTill = static_cast<uint32_t>(iSize);
            return pArena->m_pMemory;
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        inline void* AllocateByMalloc(size_t iSize)
        {
            MallocedMem_t mem;
            mem.m_pMemory = malloc(iSize);
            mem.m_iSize   = iSize;

            assert(mem.m_pMemory != nullptr && "Malloc failed");

            m_vecManualAllocs.push_back(mem);

            return mem.m_pMemory;
        }
                                                                

        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        struct Arena_t
        {
            Arena_t()
            {
                m_iSize     = 0;
                m_iUsedTill = 0;
                m_pMemory   = nullptr;
            }
            

            ///////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////
            inline void InitializeArena(uint64_t iSize)
            {
                assert(m_pMemory == nullptr && "Why is this not nullptr?");

                m_iSize     = static_cast<uint32_t>(iSize);
                m_iUsedTill = 0;
                m_pMemory   = malloc(iSize);

                // too far gone.
                assert(m_pMemory != nullptr && "Why is this nullptr?");
            }


            uint32_t m_iSize;
            uint32_t m_iUsedTill;
            void*    m_pMemory;
        };


        static const uint64_t DEFAULT_ARENA_SIZE = 4 * 1024; // 4 KiB by default.
        uint64_t m_iArenaSize = DEFAULT_ARENA_SIZE;
                                                                

        // All arena objects made.
        std::vector<Arena_t> m_vecArenas;

        // In case we need to allocate something bigger than an arena, 
        // we malloc it a pointer to it here.
        struct MallocedMem_t { uint64_t m_iSize = 0llu; void* m_pMemory = nullptr; };
        std::vector<MallocedMem_t> m_vecManualAllocs;
};
