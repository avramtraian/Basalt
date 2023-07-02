// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"
#include "Core/Memory/Memory.h"
#include "Core/Misc/AssertionMacros.h"
#include "Hash.h"
#include "Pair.h"

#include <initializer_list>

namespace Basalt
{

/**
 * Container that stores key-value pairs. Each value has an associated key, which is used for
 * element retrieval. The structure is based on hashing keys, this strategy providing O(1) time
 * complexity for insertion, deletion and look-up operations.
 */
template<typename KeyType, typename ValueType>
class BASALT_S_API HashTable
{
private:
    /** The key-value pair where the hash table elements are stored. */
    using Bucket = Pair<KeyType, ValueType>;

    /** The state of a bucket. */
    enum class EBucketState : U8
    {
        /** The bucket is currently empty and it was never occupied. */
        Empty = 0x00,

        /** The bucket is currently occupied. */
        Occupied = 0xFF,

        /** The bucket is currently empty, but it was occupied at some point in the past. */
        Deleted = 0xDD,
    };
    
    /** The maximum load factor of the hash table when a new entry can still be added. */
    static constexpr double MaxLoadFactor = 0.75;

public:
    /** Default constructor. Doesn't allocate any memory. */
    FORCEINLINE HashTable()
        : m_buckets(nullptr)
        , m_bucket_states(nullptr)
        , m_capacity(0)
        , m_count(0)
    {}

    FORCEINLINE HashTable(const HashTable& other)
        : m_buckets(nullptr)
        , m_bucket_states(nullptr)
        , m_capacity(0)
        , m_count(0)
    {
        if (other.IsEmpty())
        {
            return;
        }

        for (Usize index = 0; index < other.m_capacity; ++index)
        {
            // Allocate enough buckets to store all hash table entries.
            ReAllocate(GetRequiredCapacity(other.m_count));

            if (other.m_bucket_states[index] == EBucketState::Occupied)
            {
                const Bucket& bucket = other.m_buckets[index];
                // Get the index where the bucket will be constructed.
                Usize slot_index = AddInternalUnsafe(bucket.a);

                // Construct the bucket.
                new (m_buckets + slot_index) Bucket(bucket);
            }
        }
    }

    FORCEINLINE HashTable(HashTable&& other)
        : m_buckets(other.m_buckets)
        , m_bucket_states(other.m_bucket_states)
        , m_capacity(other.m_capacity)
        , m_count(other.m_count)
    {
        other.m_buckets = nullptr;
        other.m_bucket_states = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;
    }

    FORCEINLINE HashTable(std::initializer_list<Bucket> init_list)
        : m_buckets(nullptr)
        , m_bucket_states(nullptr)
        , m_capacity(0)
        , m_count(0)
    {
        if (init_list.size() > 0)
        {
            // Allocate enough buckets to store all hash table entries.
            ReAllocate(GetRequiredCapacity(init_list.size()));

            for (Usize index = 0; index < init_list.size(); ++index)
            {
                const Bucket& bucket = init_list.begin()[index];
                // Get the index where the bucket will be constructed.
                Usize slot_index = AddInternal(bucket.a);
                // The provided list of elements contains duplicate keys.
                CheckDebug(slot_index != InvalidSize);

                // Construct the bucket.
                new (m_buckets + slot_index) Bucket(bucket);
            }
        }
    }

    /**
     * Destructor.
     * Destroys the occupied buckets and releases the hash table memory.
     */
    FORCEINLINE ~HashTable()
    {
        Clear();
        ReleaseMemory(m_buckets, m_capacity);
    }

public:
    /** @return Const-pointer to the memory where the hash table buckets are stored. */
    FORCEINLINE const Pair<KeyType, ValueType>* GetBuckets() const { return m_buckets; }
    
    /** @return Const-pointer to the memory where the bucket states are stored. */
    FORCEINLINE const U8* GetBucketStates() const { return m_bucket_states; }

    /** @return The number of entries currently stored in the hash table. */
    FORCEINLINE Usize Count() const { return m_count; }

    /** @return The number of entries that are currently allocated by the hash table. */
    FORCEINLINE Usize Capacity() const { return m_capacity; }

    /** @return True if the hash table has to elements stored in it; False otherwise. */
    FORCEINLINE bool IsEmpty() const { return (m_count == 0); }

    /** @return The current load factor of the hash table. */
    FORCEINLINE double GetLoadFactor() const
    {
        if (m_capacity == 0)
        {
            // The hash table has no capacity, so it is technically 100% occupied.
            return 1.0;
        }

        // The load factor represents the ratio between the occupied and total number of buckets.
        return (double)m_count / (double)m_capacity;
    }

public:
    /**
     * Finds the index of the bucket where the given key is stored.
     * If the key doesn't exist in the hash table, `InvalidSize` is returned.
     * 
     * @param key The key to find.
     * @return The index of the bucket where the key is stored, if it exists.
     */
    FORCEINLINE Usize Find(const KeyType& key) const
    {
        // Compute the hash of the key.
        const U64 hash = GetKeyHash(key);
        Usize index = hash % m_capacity;

        for (Usize counter = 0; counter < m_capacity; ++counter)
        {
            if (m_bucket_states[index] == EBucketState::Occupied)
            {
                if (m_buckets[index].a == key)
                {
                    // The key was found.
                    return index;
                }
            }
            else if (m_bucket_states[index] == EBucketState::Empty)
            {
                // The key doesn't exist in the hash table.
                return InvalidSize;
            }

            // Advance to the next hash table slot index.
            index = (index + 1) % m_capacity;
        }

        // The key doesn't exist in the hash table.
        return InvalidSize;
    }

    /**
     * Checks if the given key is present in the hash table.
     * 
     * @param key The key to check.
     * @return True if the key exists in the hash table; False otherwise.
     */
    FORCEINLINE bool Contains(const KeyType& key) const { return (Find(key) != InvalidSize); }

public:
    /**
     * Gets a reference to the value associated with the given key.
     * In debug configurations, if the key doesn't exist in the hash table an assert will be triggered.
     * 
     * @param key The key to look-up.
     * @return Reference to the value associated with the given key.
     */
    FORCEINLINE ValueType& At(const KeyType& key)
    {
        // Get the index where the key is stored.
        const Usize slot_index = AtInternal(key);
        // The key doesn't exist in the hash table.
        CheckDebug(slot_index != InvalidSize);

        return m_buckets[slot_index].b;
    }

    /**
     * Gets a const-reference to the value associated with the given key.
     * In debug configurations, if the key doesn't exist in the hash table an assert will be triggered.
     * Const version of the above.
     * 
     * @param key The key to look-up.
     * @return Const-reference to the value associated with the given key.
     */
    FORCEINLINE const ValueType& At(const KeyType& key) const
    {
        // Get the index where the key is stored.
        const Usize slot_index = AtInternal(key);
        // The key doesn't exist in the hash table.
        CheckDebug(slot_index != InvalidSize);

        return m_buckets[slot_index].b;
    }

    /**
     * Gets a reference to the value stored at the provided bucket index.
     * If the bucket stored at the given index is empty/deleted, or the given index is
     * out of range, an assert will be triggered.
     * 
     * @param bucket_index The index of the bucket to look-up.
     * @return Reference to the value stored at the given bucket index.
     */
    FORCEINLINE ValueType& AtIndex(Usize bucket_index)
    {
        // The provided index is out of range.
        Check(bucket_index < m_capacity);
        // The provided index is not valid.
        Check(m_bucket_states[bucket_index] == EBucketState::Occupied);

        return m_buckets[bucket_index].b;
    }

    /**
     * Gets a const-reference to the value stored at the provided bucket index.
     * If the bucket stored at the given index is empty/deleted, or the given index is
     * out of range, an assert will be triggered.
     * Const version of the above.
     * 
     * @param bucket_index The index of the bucket to look-up.
     * @return Const-reference to the value stored at the given bucket index.
     */
    FORCEINLINE const ValueType& At(Usize bucket_index) const
    {
        // The provided index is out of range.
        Check(bucket_index < m_capacity);
        // The provided index is not valid.
        Check(m_bucket_states[bucket_index] == EBucketState::Occupied);

        return m_buckets[bucket_index].b;
    }

public:
    /**
     * If the provided key is present in the hash table, a reference to the value associated with it
     * is returned. Otherwise, the key is added to the hash table and a value is created using
     * its default constructor.
     * 
     * @param key The key to find or to add.
     * @return Reference to the value associated with the provided key.
     */
    FORCEINLINE ValueType& operator[](const KeyType& key)
    {
        // Find the index where the key is stored, if exists, or where the bucket will be constructed otherwise.
        const Usize slot_index = GetOrAddInternal(key);

        if (m_bucket_states[slot_index] == EBucketState::Occupied)
        {
            // The key is present in the hash table.
            return m_buckets[slot_index].b;
        }

        // Construct the bucket.
        m_bucket_states[slot_index] = EBucketState::Occupied;
        ++m_count;
        new (&m_buckets[slot_index].a) KeyType(key);
        new (&m_buckets[slot_index].b) ValueType();

        return m_buckets[slot_index].b;
    }

    /**
     * If the provided key is present in the hash table, a reference to the value associated with it
     * is returned. Otherwise, the key is added to the hash table and a value is created using
     * its default constructor.
     * Move semantics version of the above.
     *
     * @param key The key to find or to add.
     * @return Reference to the value associated with the provided key.
     */
    FORCEINLINE ValueType& operator[](KeyType&& key)
    {
        // Find the index where the key is stored, if exists, or where the bucket will be constructed otherwise.
        const Usize slot_index = GetOrAddInternal(key);

        if (m_bucket_states[slot_index] == EBucketState::Occupied)
        {
            // The key is present in the hash table.
            return m_buckets[slot_index].b;
        }

        // Construct the bucket.
        m_bucket_states[slot_index] = EBucketState::Occupied;
        ++m_count;
        new (&m_buckets[slot_index].a) KeyType(Move(key));
        new (&m_buckets[slot_index].b) ValueType();

        return m_buckets[slot_index].b;
    }

public:
    /**
     * Adds a key-value pair to the hash table.
     * In debug configuration, if the key already exists in the hash table an assert will be issued.
     * 
     * @param key   The key to add to the hash table. The key will be constructed using its copy constructor.
     * @param value The value to add to the hash table. The value will be constructed using its copy constructor.
     * 
     * @return Reference to the newly added value, that is stored in the hash table.
     */
    FORCEINLINE ValueType& Add(const KeyType& key, const ValueType& value)
    {
        if (GetLoadFactor() > MaxLoadFactor)
        {
            // There are not enough buckets to store another entry.
            ReAllocate(m_capacity * 2 + 2);
        }

        // Find the index where the key-value pair will be constructed.
        Usize index = AddInternal(key);
        // The key already exists in the hash table.
        CheckDebug(index != InvalidSize);

        // Construct the key-value pair.
        new (m_buckets + index) Bucket(key, value);
        return m_buckets[index].b;
    }

    /**
     * Adds a key-value pair to the hash table.
     * In debug configuration, if the key already exists in the hash table an assert will be issued.
     *
     * @param key   The key to add to the hash table. The key will be constructed using its copy constructor.
     * @param value The value to add to the hash table. The value will be constructed using its move constructor.
     *
     * @return Reference to the newly added value, that is stored in the hash table.
     */
    FORCEINLINE ValueType& Add(const KeyType& key, ValueType&& value)
    {
        if (GetLoadFactor() > MaxLoadFactor)
        {
            // There are not enough buckets to store another entry.
            ReAllocate(m_capacity * 2 + 2);
        }

        // Find the index where the key-value pair will be constructed.
        Usize index = AddInternal(key);
        // The key already exists in the hash table.
        CheckDebug(index != InvalidSize);

        // Construct the key-value pair.
        new (m_buckets + index) Bucket(key, Move(value));
        return m_buckets[index].b;
    }

    /**
     * Adds a key-value pair to the hash table.
     * In debug configuration, if the key already exists in the hash table an assert will be issued.
     *
     * @param key   The key to add to the hash table. The key will be constructed using its move constructor.
     * @param value The value to add to the hash table. The value will be constructed using its copy constructor.
     *
     * @return Reference to the newly added value, that is stored in the hash table.
     */
    FORCEINLINE ValueType& Add(KeyType&& key, const ValueType& value)
    {
        if (GetLoadFactor() > MaxLoadFactor)
        {
            // There are not enough buckets to store another entry.
            ReAllocate(m_capacity * 2 + 2);
        }

        // Find the index where the key-value pair will be constructed.
        Usize index = AddInternal(key);
        // The key already exists in the hash table.
        CheckDebug(index != InvalidSize);

        // Construct the key-value pair.
        new (m_buckets + index) Bucket(Move(key), value);
        return m_buckets[index].b;
    }

    /**
     * Adds a key-value pair to the hash table.
     * In debug configuration, if the key already exists in the hash table an assert will be issued.
     *
     * @param key   The key to add to the hash table. The key will be constructed using its move constructor.
     * @param value The value to add to the hash table. The value will be constructed using its move constructor.
     *
     * @return Reference to the newly added value, that is stored in the hash table.
     */
    FORCEINLINE ValueType& Add(KeyType&& key, ValueType&& value)
    {
        if (GetLoadFactor() > MaxLoadFactor)
        {
            // There are not enough buckets to store another entry.
            ReAllocate(m_capacity * 2 + 2);
        }

        // Find the index where the key-value pair will be constructed.
        Usize index = AddInternal(key);
        // The key already exists in the hash table.
        CheckDebug(index != InvalidSize);

        // Construct the key-value pair.
        new (m_buckets + index) Bucket(Move(key), Move(value));
        return m_buckets[index].b;
    }

public:
    /**
     * Clears the hash table.
     * All occupied buckets are destroyed and all bucket states are set to `Empty`.
     */
    FORCEINLINE void Clear()
    {
        if (m_count > 0)
        {
            for (Usize index = 0; index < m_capacity; ++index)
            {
                if (m_bucket_states[index] == EBucketState::Occupied)
                {
                    m_buckets[index].~Bucket();
                }
            }
            
            m_count = 0;
        }

        Memory::Set(m_bucket_states, (U8)EBucketState::Empty, m_capacity * sizeof(EBucketState));
    }

private:
    /**
     * Computes the hash value of the given key.
     * 
     * @param key The key to compute its hash.
     * @return The hash of the provided key.
     */
    FORCEINLINE U64 GetKeyHash(const KeyType& key) const
    {
        const U64 hash = Hasher::Compute(key);
        return hash;
    }

    /**
     * Finds the bucket where the key should be constructed and marks it as occupied.
     * In debug configurations, this function also checks if the key already exists. If so, it will
     * return `InvalidSize`. This value will trigger an assert where the key-value pair is actually constructed.
     * 
     * @param key The key to find the bucket for.
     * @return The index of the found bucket.
     */
    FORCEINLINE Usize AddInternal(const KeyType& key)
    {
        // Compute the hash of the key.
        const U64 key_hash = GetKeyHash(key);
        Usize index = key_hash % m_capacity;

        while (true)
        {
            if (m_bucket_states[index] != EBucketState::Occupied)
            {
                // A free bucket was found.
                break;
            }

#if BT_BUILD_DEBUG
            if (m_bucket_states[index] == EBucketState::Occupied && m_buckets[index].a == key)
            {
                // The key already exists in the hash table.
                return InvalidSize;
            }
#endif // BT_BUILD_DEBUG

            // Advance to the next hash table slot index.
            index = (index + 1) % m_capacity;
        }

        m_bucket_states[index] = EBucketState::Occupied;
        ++m_count;
        return index;
    }

    /**
     * Finds the bucket where the key should be constructed and marks it as occupied.
     * This function doesn't check if the key already exists in the hash table. Should only
     * be used in contexts where it is impossible for duplicate keys to exist.
     *
     * @param key The key to find the bucket for.
     * @return The index of the found bucket.
     */
    FORCEINLINE Usize AddInternalUnsafe(const KeyType& key)
    {
        // Compute the hash of the key.
        const U64 key_hash = GetKeyHash(key);
        Usize index = key_hash % m_capacity;

        while (true)
        {
            if (m_bucket_states[index] != EBucketState::Occupied)
            {
                // A free bucket was found.
                break;
            }

            // Advance to the next hash table slot index.
            index = (index + 1) % m_capacity;
        }

        m_bucket_states[index] = EBucketState::Occupied;
        ++m_count;
        return index;
    }

    /**
     * Finds the index where the given key is stored. If the key couldn't be found in the hash table,
     * the index of the first empty or deleted bucket is returned.
     * 
     * @param key The key to find/store.
     * @return The index where the key is stored or where to construct it.
     */
    FORCEINLINE Usize GetOrAddInternal(const KeyType& key)
    {
        // Compute the hash of the key.
        const U64 key_hash = GetKeyHash(key);
        Usize index = key_hash % m_capacity;

        // Stores the first empty or deleted bucket, in case the provided key doesn't exist.
        Usize add_index = InvalidSize;

        // Because it is possible that the hash table only contains occupied and deleted buckets,
        // the iteration would never break if the key is not present in the hash table. So, in the
        // worst case scenario, we iterate over all the buckets in the hash table and then the 'for' loop exits.
        for (Usize counter = 0; counter < m_capacity; ++counter)
        {
            if (m_bucket_states[index] == EBucketState::Occupied)
            {
                if (m_buckets[index].a == key)
                {
                    // The key was found.
                    return index;
                }
            }
            else if (m_bucket_states[index] == EBucketState::Empty)
            {
                if (add_index == InvalidSize)
                {
                    // This is the first available index where a key can be inserted.
                    add_index = index;
                }

                // Because we encountered a bucket that never contained a value, we can be sure
                // that the provided key doesn't exist in the hash table.
                return add_index;
            }
            else if (m_bucket_states[index] == EBucketState::Deleted)
            {
                if (add_index == InvalidSize)
                {
                    // This is the first available index where a key can be inserted.
                    add_index = index;
                }
            }

            // Advance to the next slot index.
            index = (index + 1) % m_capacity;
        }

        // NOTE(traian): This is only triggered by implementation bugs!
        CheckDebug(add_index != InvalidSize);

        // The key is not present in the hash table, so we return the index of the first available index.
        return add_index;
    }

    /**
     * Gets the index where the given key is stored.
     * In debug configurations, if the key doesn't exist in hash table `InvalidSize` will be returned.
     * 
     * @param key The key to find.
     * @return The index where the key is stored, or `InvalidSize` if it doesn't exist.
     */
    FORCEINLINE Usize AtInternal(const KeyType& key) const
    {
        // Compute the hash of the key.
        const U64 key_hash = GetKeyHash(key);
        Usize index = key_hash % m_capacity;

#if BT_BUILD_DEBUG
        for (Usize counter = 0; counter < m_capacity; ++counter)
#else
        while (true)
#endif // BT_BUILD_DEBUG
        {
            if (m_bucket_states[index] == EBucketState::Occupied)
            {
                if (m_buckets[index].a == key)
                {
                    // The key was found.
                    return index;
                }
            }
#if BT_BUILD_DEBUG
            else if (m_bucket_states[index] == EBucketState::Empty)
            {
                // The key doesn't exist in the hash table.
                return InvalidSize;
            }
#endif // BT_BUILD_DEBUG

            // Advance to the next hash table slot index.
            index = (index + 1) % m_capacity;
        }

#if BT_BUILD_DEBUG
        // The key doesn't exist in the hash table.
        return InvalidSize;
#endif // BT_BUILD_DEBUG
    }

private:
    /**
     * Allocates a block of memory sufficient to store `capacity` buckets and their state.
     * All memory allocations performed by the hash table should pass through this function.
     * 
     * @param capacity          The number of buckets (and their state) the memory buffer must be able to store.
     * @param out_buckets       Out parameter that will hold the pointer to the location where the buckets are stored.
     * @param out_bucket_states Out parameter that will hold the pointer to the location where the bucket states are stored.
     */
    FORCEINLINE static void AllocateMemory(Usize capacity, Bucket*& out_buckets, EBucketState*& out_bucket_states)
    {
        void* memory = Memory::AllocateTaggedI(capacity * (sizeof(Bucket) + sizeof(EBucketState)));
        out_buckets = (Bucket*)memory;
        out_bucket_states = (EBucketState*)(out_buckets + capacity);
        Memory::Set(out_bucket_states, (U8)EBucketState::Empty, capacity * sizeof(EBucketState));
    }

    /**
     * Releases the hash table memory.
     * All memory deallocations performed by the hash table should pass through this function.
     * 
     * @param buckets   Pointer to where the buckets are stored in memory.
     * @param capacity  The number of buckets to release.
     */
    FORCEINLINE static void ReleaseMemory(Bucket* buckets, Usize capacity)
    {
        MAYBE_UNUSED const Usize bytes_count = capacity * (sizeof(Bucket) + sizeof(EBucketState));
        Memory::Free((void*)buckets);
    }

private:
    /**
     * Calculates the required capacity such that the hash table can store `count` occupied entries
     * without exceeding `MaxLoadFactor`.
     * 
     * @param count The number of occupied entries the hash table has.
     * @return The required capacity.
     */
    FORCEINLINE static Usize GetRequiredCapacity(Usize count)
    {
        double capacity = (double)count * (1.0 / MaxLoadFactor);
        return (Usize)capacity + 1;
    }

    /**
     * Allocates a new block of memory and moves the buckets and their state to it.
     * The new memory block capacity must be greater than the current one.
     * 
     * @param new_capacity The number of buckets (and their state) the newly allocate memory
     *                     block can store. Must be greater than the current capacity.
     */
    void ReAllocate(Usize new_capacity)
    {
        // NOTE(traian): Only triggered by implementation bugs.
        Check(new_capacity > m_capacity);

        if (m_count > 0)
        {
            // Save the current location where the buckets and their state are stored, as
            // well as the number of buckets allocated.
            Bucket* buckets             = m_buckets;
            EBucketState* bucket_states = m_bucket_states;
            Usize capacity              = m_capacity;

            // Allocate memory for the new buckets and their state.
            m_capacity = new_capacity;
            AllocateMemory(m_capacity, m_buckets, m_bucket_states);
            m_count = 0;

            for (Usize index = 0; index < capacity; ++index)
            {
                if (bucket_states[index] == EBucketState::Occupied)
                {
                    Bucket& bucket = buckets[index];
                    // Get the index where the bucket will be constructed.
                    Usize slot_index = AddInternalUnsafe(bucket.a);

                    // Construct the bucket.
                    new (m_buckets + slot_index) Bucket(Move(bucket));

                    // Delete the old bucket.
                    bucket.~Bucket();
                }
            }

            // Release the old buckets and their state.
            ReleaseMemory(buckets, capacity);
        }
        else
        {
            // Release the old buckets and their state.
            ReleaseMemory(m_buckets, m_capacity);

            // Allocate memory for the new buckets and their state.
            m_capacity = new_capacity;
            AllocateMemory(m_capacity, m_buckets, m_bucket_states);
            m_count = 0;
        }
    }

private:
    /** Pointer to the array where the key-value pairs (or buckets) are stored. */
    Bucket* m_buckets;
    
    /** Pointer to the array where the bucket states are stored. */
    EBucketState* m_bucket_states;
    
    /** The number of buckets that are currently allocated. */
    Usize m_capacity;

    /** The number of buckets currently occupied. */
    Usize m_count;
};

} // namespace Basalt
