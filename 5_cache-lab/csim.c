// Names: Joel Meine & Paul Roylance
// IDs: A01764207 & A01875212
// Source: CS:APP3e 616-617

#include "cachelab.h" // printSummary
#include <getopt.h> // getopt, optarg
#include <stdlib.h> // atoi
#include <stdio.h> // EOF
#include <unistd.h>
#include <math.h> // pow

typedef struct {
    int s; // Number of set index bits (S = pow(2,s) is the number of sets)
    int S; // Number of sets from S = pow(2,s)
    int E; // Associativity (number of lines per set)
    int b; // Number of block bits (B = pow(2,b) is the block size)
    int B; // Block size in bytes from B = pow(2,b)
    char *t; // Name of the valgrind trace to replay
    int v; // Optional verbose flag that displays trace info

    unsigned int hit_count;
    unsigned int miss_count;
    unsigned int eviction_count;
} cache_parameters;

typedef unsigned long long memory_address; // 64-bit hexidecimal memory address

typedef struct {
    int valid; // Indicates whether or not the line contains meaningful information
    memory_address tag; // Uniquely identifies the block stored in the cache line
    char *block; // B = pow(2,b) bytes per cache block (the data)
    unsigned int used_count; // Counts the number of times accessed
} line_set;

typedef struct {
    line_set *lines;
} cache_set;

typedef struct {
    cache_set *sets;
} cache;

cache buildCache(int S, int E)
{
    // Initialize empty cache set and line.
    cache new_cache;
    cache_set set;
    line_set line;
    line.valid = 0;
    line.tag = 0;
    line.used_count = 0;
    
    // Allocate storage for cache sets.
    new_cache.sets = (cache_set *) malloc(sizeof(cache_set) * S);

    // Build each set of the cache.
    for (unsigned int setIndex = 0; setIndex < S; setIndex++)
    {
        // Allocate storage for a set line.
        set.lines = (line_set *) malloc(sizeof(line_set) * E);
        // Save new set with the allocated line storage.
        new_cache.sets[setIndex] = set;
        // Build each line of the set.
        for (unsigned int lineIndex = 0; lineIndex < E; lineIndex++)
            // Save new line to selected set.
            set.lines[lineIndex] = line;
    }
    return new_cache;
}

void emptyCache(cache sim_cache, int S, int E)
{
    // Load each set of the cache.
    for (unsigned int setIndex = 0; setIndex < S; setIndex++)
    {
        // Load selected set.
        cache_set set = sim_cache.sets[setIndex];
        // Check if selected set has no lines.
        if (set.lines != NULL)
            // YES
            free(set.lines); // Remove storage for selected set lines.
    }
    // Check if the cache has sets.
    if (sim_cache.sets != NULL)
        // YES
        free(sim_cache.sets); // Remove storage for cache sets.
}

int findEmptyLine(cache_parameters parameters, cache_set selected_set)
{
    // Load each line of the selected set.
    for (unsigned int lineIndex = 0; lineIndex < parameters.E; lineIndex++)
    {
        // Load selected line.
        line_set line = selected_set.lines[lineIndex];
        // Check if line contains meaningful information.
        if (!line.valid)
        {
            // NO
            return lineIndex; // Return index of empty line to be replaced.
        }
    }
    return -1; // No empty line was found.
}

int findEvictLine(cache_parameters parameters, cache_set selected_set, int *lines_used)
{
    unsigned int used_count = selected_set.lines[0].used_count;
    unsigned int more_used = used_count; // Initialize the most frequently used line access count.
    unsigned int less_used = used_count; // Initialize the least frequently used line access count.
    
    // Initialize the evict index to be returned to the cache simulator.
    unsigned int evictIndex = 0;
    
    // Load each line of the selected set.
    for (unsigned int lineIndex = 1; lineIndex < parameters.E; lineIndex++)
    {
        // Load selected line.
        line_set line = selected_set.lines[lineIndex];
	// Check if most frequently used line access count is less than the selected line's access count.
        if (line.used_count > more_used)
        {
            // YES
            more_used = line.used_count; // Update the most frequently used line access count.
        }
        // Check if least frequently used line access count is greater than the selected line's access count.
        if (line.used_count < less_used)
        {
            // YES
	    less_used = line.used_count; // Update the least frequently used line access count.
            evictIndex = lineIndex; // Update the evict index which is the LRU line.
        }
    }

    lines_used[0] = less_used;
    lines_used[1] = more_used;

    return evictIndex; // Return index of evict line to be replaced.
}

cache_parameters simulateCache(cache_parameters parameters, cache sim_cache, memory_address address)
{
    memory_address in_tag = address >> (parameters.s + parameters.b);
    
    unsigned int hit_count = parameters.hit_count;

    int cacheFull = 1;
  
    unsigned int m = 64; // Number of physical (main memory) address bits
    unsigned int tag_size = m - (parameters.s + parameters.b); // Number of tag bits
    memory_address temp = address << tag_size;
    memory_address setIndex = temp >> (parameters.b + tag_size);
    cache_set selected_set = sim_cache.sets[setIndex];

    // Load each line of the selected set.
    for (unsigned int lineIndex = 0; lineIndex < parameters.E; lineIndex++)
    {
        // Load selected line.
        line_set line = selected_set.lines[lineIndex];

	// Check if line is verified to be empty.
	if (cacheFull && !line.valid)
	{
	    // YES
            cacheFull = 0;
	}
	// Check if line contains meaningful information.
	else if (line.valid)
	{
	    // YES
            // Check if selected line tag matches input tag.
            if (in_tag == line.tag)
            {
                // YES; i.e. a cache hit.
	        parameters.hit_count++; // Increase hit count of cache parameters.
                if (parameters.v) printf("hit "); // Print 'hit' if verbose mode is enabled.
                line.used_count++; // Increase access count of selected line.
                selected_set.lines[lineIndex] = line;
            }
	}
    }
    // Check if hit count was not increased.
    if (parameters.hit_count != hit_count)
        // NO; i.e. a cache hit.
        return parameters; // Cache simulation run is finished.
    else
    {
        // YES
        parameters.miss_count++; // Increase miss count of cache parameters.
        if (parameters.v) printf("miss "); // Print 'miss' if verbose mode is enabled.
    }
    /* The target data was not found in the cache; i.e. a cache miss.
       Therefore, write the target data into the cache.
       However, the cache may be full and will not accept a data store.
       If cache is full, replace a cache line by LRU (least-recently used) policy.
       Using LRU policy will result in an increase of the eviction count.
       Proceed to write to cache only when space is available in the cache. */

    int *lines_used = (int*) malloc(sizeof(int) * 2); // Allocate storage for lines that have been accessed.
    int emptyIndex = findEmptyLine(parameters, selected_set); // Locate index of an available empty line.
    int evictIndex = findEvictLine(parameters, selected_set, lines_used); // Locate index of least recently used line.

    // Check if cache is full.
    if (!cacheFull)
    {
        // NO
	selected_set.lines[emptyIndex].valid = 1; // Mark target line within the selected set as having meaningful information.
	selected_set.lines[emptyIndex].tag = in_tag; // Update tag of line in selected set.
        selected_set.lines[emptyIndex].used_count = lines_used[1] + 1; // Update access count of line in selected set.
    }
    else
    {
	// YES
        parameters.eviction_count++; // Increase eviction count of cache parameters.
        if (parameters.v) printf("eviction "); // Print 'eviction' if verbose mode is enabled.
	selected_set.lines[evictIndex].tag = in_tag; // Update tag of line in selected set.
	selected_set.lines[evictIndex].used_count = lines_used[1] + 1; // Update access count of line in selected set.
    }

    free(lines_used); // Remove storage for lines that have been accessed.
    return parameters; // Cache simulation run is finished.
}

void printUsage()
{
    printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("   -h: Optional help flag that prints usage info\n");
    printf("   -v: Optional verbose flag that displays trace info\n");
    printf("   -s <s>: Number of set index bits (S = pow(2,s) is the number of sets)\n");
    printf("   -E <E>: Associativity (number of lines per set)\n");
    printf("   -b <b>: Number of block bits (B = pow(2,b) is the block size)\n");
    printf("   -t <tracefile>: Name of the valgrind trace to replay\n");
    printf("\n");
    printf("Example:\n");
    printf("\n");
    printf("linux> ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("hits:4 misses:5 evictions:3\n");
    printf("\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    /* 1. Read in command arguments and set cache parameters. */
    cache_parameters parameters;
    parameters.v = 0;

    char symbol;
    while( (symbol = getopt(argc, argv, "s:E:b:t:vh")) != EOF)
    {
        switch(symbol)
        {
            case 's': // Number of set index bits
                parameters.s = atoi(optarg);
                parameters.S = pow(2.0, parameters.s);
                break;
            case 'E': // Number of lines per set
                parameters.E = atoi(optarg);
                break;
            case 'b': // Number of block bits
                parameters.b = atoi(optarg);
                parameters.B = pow(2.0, parameters.b);
                break;
            case 't': // Name of the valgrind trace to replay
                parameters.t = optarg;
                break;
            case 'v': // Optional verbose flag that displays trace info
                parameters.v = 1;
                break;
            case 'h': // Optional help flag that prints usage info
                printUsage();
                exit(0);
            default:
                printUsage();
                exit(1);
        }
    }
    parameters.hit_count = 0;
    parameters.miss_count = 0;
    parameters.eviction_count = 0;

    /* 2. Verify that all required command arguments were given. */
    if (parameters.s <= 0 || parameters.E <= 0 || parameters.b <= 0 || parameters.t == NULL)
    {
        printf("ERROR: Invalid or missing command argument.");
        printf("\n");
        printUsage();
        exit(1);
    }

    /* 3. Build the cache. */
    cache sim_cache;
    sim_cache = buildCache(parameters.S, parameters.E);

    /* 4. Read in trace file and run simulation. */
    FILE *in_file; // Trace file input:
    /*
        [space]operation address,size

        Only operation type "I" will not have a preceding space.
    */
    char operation; // Operation input:
    /*
        "I" denotes an instruction load
        "L" denotes a data load
        "S" denotes a data store
        "M" denotes a data modify (i.e., a data load followed by a data store)
    */
    memory_address address;
    int size; // Number of bytes accessed by the operation

    in_file = fopen(parameters.t, "r"); // Load trace file
    while(fscanf(in_file, " %c %llx,%d", &operation, &address, &size) != EOF)
    {
        if (parameters.v && operation != 'I')
            printf("%c %llx,%d ", operation, address, size); // Print trace output if verbose mode is enabled.
        switch(operation)
        {
            case 'I': // Instruction
                break; // Ignore all instruction cache accesses
            case 'M': // Modify
                parameters = simulateCache(parameters, sim_cache, address);
            case 'L': // Load
            case 'S': // Store
                parameters = simulateCache(parameters, sim_cache, address);
                break;
            default:
                break;
        }
        if (parameters.v && operation != 'I')
            printf("\n");
    }

    /* 5. Print simulation results, empty the cache, and close the trace file. */

    printSummary(parameters.hit_count, parameters.miss_count, parameters.eviction_count);
    emptyCache(sim_cache, parameters.S, parameters.E);
    fclose(in_file);

    return 0;
}
