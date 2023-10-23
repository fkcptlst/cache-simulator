#include <iostream>
#include <cmath>
#include <iomanip>
#include "cache.h"

int **init_mat(int rows, int cols, int init_val = 0) {
    int **mat = new int *[rows];
    for (int i = 0; i < rows; i++) {
        mat[i] = new int[cols];
        for (int j = 0; j < cols; j++) {
            mat[i][j] = init_val;
        }
    }
    return mat;
}

Cache::Cache(int l1_blocksize, int l1_size, int l1_assoc, int l1_replacement_policy, int l1_write_policy,
             std::string trace_file) {
    // 1. init config
    config.L1_BLOCK_SIZE = l1_blocksize;
    config.L1_SIZE = l1_size;
    config.L1_ASSOC = l1_assoc;
    config.L1_REPLACEMENT_POLICY = l1_replacement_policy;
    config.L1_WRITE_POLICY = l1_write_policy;

    config.trace_file = trace_file;

    // 2. init stats
    stats.L1_READS = 0;
    stats.L1_READ_MISSES = 0;
    stats.L1_WRITES = 0;
    stats.L1_WRITEBACKS = 0;

    // 3. init cache
    const int _rows = config.L1_NUM_SETS();
    const int _cols = config.L1_ASSOC;
    TAG_MAT = init_mat(_rows, _cols);
    VALID_MAT = init_mat(_rows, _cols);
    DIRTY_MAT = init_mat(_rows, _cols);
    LRU_MAT = init_mat(_rows, _cols);
    LFU_MAT = init_mat(_rows, _cols);
}

Cache::~Cache() {
    // TODO: delete TAG_MAT, VALID_MAT, DIRTY_MAT, LRU_MAT, LFU_MAT
}

void Cache::decode_addr(uint32_t addr, int &tag, int &set, int &offset) {
    offset = addr & (config.L1_BLOCK_SIZE - 1);
    set = (addr >> (int) log2(config.L1_BLOCK_SIZE)) & (config.L1_NUM_SETS() - 1);
    tag = addr >> (int) (log2(config.L1_BLOCK_SIZE) + log2(config.L1_NUM_SETS()));
}

uint32_t Cache::encode_addr(int tag, int set, int offset) {
    return (tag << (int) (log2(config.L1_BLOCK_SIZE) + log2(config.L1_NUM_SETS()))) |
           (set << (int) log2(config.L1_BLOCK_SIZE)) |
           offset;
}

void Cache::update_block_used(int index, int way) {
    if (config.L1_REPLACEMENT_POLICY == LRU) {
        // find the max of LRU_MAT[index]
        int max = LRU_MAT[index][0];
        for (int i = 1; i < config.L1_ASSOC; i++) {
            if (LRU_MAT[index][i] > max) {
                max = LRU_MAT[index][i];
            }
        }
        // update LRU_MAT[index][way]
        LRU_MAT[index][way] = max + 1;
    } else if (config.L1_REPLACEMENT_POLICY == LFU) {
        LFU_MAT[index][way]++;
    }
}

int Cache::choose_block_to_evict(int index) {
    int min_index = 0;

    if (config.L1_REPLACEMENT_POLICY == LRU) {
        for (int i = 1; i < config.L1_ASSOC; i++) {
            if (LRU_MAT[index][i] < LRU_MAT[index][min_index]) {
                min_index = i;
            }
        }
        return min_index;
    } else if (config.L1_REPLACEMENT_POLICY == LFU) {
        for (int i = 1; i < config.L1_ASSOC; i++) {
            if (LFU_MAT[index][i] < LFU_MAT[index][min_index]) {
                min_index = i;
            }
        }

    }
    return min_index;
}

int idx_in_array(int val, int *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) {
            return i;
        }
    }
    return -1;
}


void Cache::read(uint32_t addr) {
    int current_tag, set_index, offset;
    decode_addr(addr, current_tag, set_index, offset);
    stats.L1_READS++;

    // 1. check if hit
    int col_index = idx_in_array(current_tag, TAG_MAT[set_index], cols);
    if (col_index != -1 && VALID_MAT[set_index][col_index] == 1) {
        // hit
        update_block_used(set_index, col_index);
        return;
    } else {
        // miss
        stats.L1_READ_MISSES++;
        // 2. check if there is an empty block
        int empty_block_index = idx_in_array(0, VALID_MAT[set_index], cols);
        if (empty_block_index != -1) {
            // 2.1. there is an empty block
            // READ
            stats.mem_traffic++;

            TAG_MAT[set_index][empty_block_index] = current_tag;
            VALID_MAT[set_index][empty_block_index] = 1;
            update_block_used(set_index, empty_block_index);
            return;
        } else {
            // 2.2. there is no empty block
            // 2.2.1. choose a block to evict
            int evict_block_index = choose_block_to_evict(set_index);
            // 2.2.2. check if the block is dirty (if WBWA)
            if (config.L1_WRITE_POLICY == WBWA && DIRTY_MAT[set_index][evict_block_index] == 1) {
                // issue a writeback
                stats.mem_traffic++;
                stats.L1_WRITEBACKS++;
                DIRTY_MAT[set_index][evict_block_index] = 0;
            }
            // 2.2.3. read
            stats.mem_traffic++;
            TAG_MAT[set_index][evict_block_index] = current_tag;
            VALID_MAT[set_index][evict_block_index] = 1;
            update_block_used(set_index, evict_block_index);
        }
    }
}

void Cache::write(uint32_t addr) {
    int current_tag, set_index, offset;
    decode_addr(addr, current_tag, set_index, offset);
    stats.L1_WRITES++;

    // 1. check if hit
    int col_index = idx_in_array(current_tag, TAG_MAT[set_index], cols);
    if (col_index != -1 && VALID_MAT[set_index][col_index] == 1) {
        TAG_MAT[set_index][col_index] = current_tag;

        if (config.L1_WRITE_POLICY == WTNA) {
            TAG_MAT[set_index][col_index] = current_tag;
            stats.mem_traffic++;
        } else if (config.L1_WRITE_POLICY == WBWA) {
            DIRTY_MAT[set_index][col_index] = 1;
        }


        update_block_used(set_index, col_index);
    } else {
        stats.L1_WRITE_MISSES++;

        if (config.L1_WRITE_POLICY == WTNA) {
            stats.mem_traffic++;  // issue write
        } else if (config.L1_WRITE_POLICY == WBWA) {
            // 2. check if there is an empty block
            int empty_block_index = idx_in_array(0, VALID_MAT[set_index], cols);
            if (empty_block_index != -1) {
                // 2.1. there is an empty block
                // READ
                stats.mem_traffic++;

                TAG_MAT[set_index][empty_block_index] = current_tag;
                VALID_MAT[set_index][empty_block_index] = 1;
                DIRTY_MAT[set_index][empty_block_index] = 1;
                update_block_used(set_index, empty_block_index);
                return;
            } else {
                // 2.2. there is no empty block
                // 2.2.1. choose a block to evict
                int evict_block_index = choose_block_to_evict(set_index);
                // 2.2.2. check if the block is dirty (if WBWA)
                if (DIRTY_MAT[set_index][evict_block_index] == 1) {
                    // issue a writeback
                    stats.mem_traffic++;
                    stats.L1_WRITEBACKS++;
                    DIRTY_MAT[set_index][evict_block_index] = 0;
                }
                // 2.2.3. read
                stats.mem_traffic++;
                TAG_MAT[set_index][evict_block_index] = current_tag;
                VALID_MAT[set_index][evict_block_index] = 1;
                DIRTY_MAT[set_index][evict_block_index] = 1;
                update_block_used(set_index, evict_block_index);
            }
        }
    }
}


void Cache::print_summary() {
    // print config
    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "L1_BLOCKSIZE:\t" << config.L1_BLOCK_SIZE << std::endl;
    std::cout << "L1_SIZE:\t" << config.L1_SIZE << std::endl;
    std::cout << "L1_ASSOC:\t" << config.L1_ASSOC << std::endl;
    std::cout << "L1_REPLACEMENT_POLICY:\t" << config.L1_REPLACEMENT_POLICY << std::endl;
    std::cout << "L1_WRITE_POLICY:\t" << config.L1_WRITE_POLICY << std::endl;
    std::cout << "trace_file:\t" << config.trace_file << std::endl;
    std::cout << "===================================" << std::endl;

    // print content of L1 cache
    char dirty_char[2] = {' ', 'D'};
    std::cout << std::endl << "===== L1 contents =====" << std::endl;
    for (int i = 0; i < config.L1_NUM_SETS(); i++) {
        std::cout << "set\t" << std::dec << i << ":\t";
        for (int j = 0; j < cols; j++) {
            if (config.L1_WRITE_POLICY == WTNA) {
                std::cout << std::hex << TAG_MAT[i][j] << " ";
            } else {
                // also print dirty bit
                std::cout << std::hex << TAG_MAT[i][j] << " "
                          << dirty_char[DIRTY_MAT[i][j]] << " ";
            }

        }
        std::cout << std::endl;
    }
    // print simulation results
    std::cout << std::dec << std::endl;
    std::cout << "====== Simulation results (raw) ======" << std::endl;
    std::cout << "a. number of L1 reads:\t" << stats.L1_READS << std::endl;
    std::cout << "b. number of L1 read misses:\t" << stats.L1_READ_MISSES << std::endl;
    std::cout << "c. number of L1 writes:\t" << stats.L1_WRITES << std::endl;
    std::cout << "d. number of L1 write misses:\t" << stats.L1_WRITE_MISSES << std::endl;
    // keep 4 digits after decimal point
    std::cout << "e. L1 miss rate:\t" << std::fixed << std::setprecision(4)
              << (double) (stats.L1_READ_MISSES + stats.L1_WRITE_MISSES) / (stats.L1_READS + stats.L1_WRITES)
              << std::endl;
    std::cout << "f. number of writebacks from L1:\t" << stats.L1_WRITEBACKS << std::endl;
    std::cout << "g. total memory traffic:\t" << stats.mem_traffic << std::endl;


    std::cout << std::endl << "==== Simulation results (performance) ====" << std::endl;

    const double HT =
            0.25 + 2.5 * ((double ) config.L1_SIZE / 512 / 1024) + 0.025 * ((double ) config.L1_BLOCK_SIZE / 16) + 0.025 * config.L1_ASSOC;
    const double MissPenalty = 20 + 0.5 * ((double ) config.L1_BLOCK_SIZE / 16);

    double avg_access_time =
            HT + (stats.L1_READ_MISSES + stats.L1_WRITE_MISSES) * MissPenalty / (stats.L1_READS + stats.L1_WRITES);
    std::cout << "1. average access time:\t" << std::fixed << std::setprecision(4)
              << avg_access_time
              << " ns";
}