#include "debug.h"
#include "cache.h"


int main(int argc, char *argv[]) {
    // args: <L1_BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L1_REPLACEMENT_POLICY> <L1_WRITE_POLICY> <tracefile>
    std::string tracefile = argv[6];
    Cache cache(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), tracefile);

    // read from tracefile, format: r|w <32 bit hex address>
    FILE *fp = fopen(tracefile.c_str(), "r");
    if (fp == NULL) {
        fp = fopen((tracefile + std::string(".txt")).c_str(), "r");  // try with .txt
    }
    if (fp == NULL) {
        fp = fopen((std::string("../traces/") + tracefile).c_str(), "r");  // try with ../trace/
    }
    if (fp == NULL) {
        fp = fopen((std::string("../traces/") + tracefile + std::string(".txt")).c_str(), "r");
    }

    Assert(fp != NULL, "tracefile open failed");

    char op;
    uint32_t addr;

    while (fscanf(fp, " %c %x", &op, &addr) != EOF) {
        if (op == 'r') {
            cache.read(addr);
        } else if (op == 'w') {
            cache.write(addr);
        }
    }

    cache.print_summary();
    return 0;
}
