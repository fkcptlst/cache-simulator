#ifndef PROJ1_1_CACHE_H
#define PROJ1_1_CACHE_H

#include <string>
#include <cstdint>

const int WBWA = 0;
const int WTNA = 1;

const int LRU = 0;
const int LFU = 1;

struct L1CacheConfig {
    int L1_BLOCK_SIZE;
    int L1_SIZE;
    int L1_ASSOC;
    int L1_REPLACEMENT_POLICY;
    int L1_WRITE_POLICY;

    std::string trace_file;

    int L1_NUM_BLOCKS() const {
        return L1_SIZE / L1_BLOCK_SIZE;
    }

    int L1_NUM_SETS() const {
        return L1_NUM_BLOCKS() / L1_ASSOC;
    }

};

struct L1CacheStats {
    int L1_READS;
    int L1_READ_MISSES;

    int L1_WRITES;
    int L1_WRITE_MISSES;

    int L1_WRITEBACKS;

    int mem_traffic;
};

class Cache {
public:
    Cache(
            int l1_blocksize,
            int l1_size,
            int l1_assoc,
            int l1_replacement_policy,
            int l1_write_policy,
            std::string trace_file);

    ~Cache();

    void read(uint32_t addr);
    void write(uint32_t addr);

    void print_summary();

private:
    L1CacheConfig config;
    L1CacheStats stats{};

    int** TAG_MAT;
    int** VALID_MAT;
    int** DIRTY_MAT;
    int** LRU_MAT;
    int** LFU_MAT;

    const int &cols = config.L1_ASSOC;  // alias

    void decode_addr(uint32_t addr, int &tag, int &set, int &offset);

    uint32_t encode_addr(int tag, int set, int offset);

    void update_block_used(int index, int way);
    int choose_block_to_evict(int index);
};


#endif
