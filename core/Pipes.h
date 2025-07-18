/**
 * @file Pipes.h
 * @brief Function Pipeline implementation - dynamic array of function pointers
 * @defgroup FunctionPipeline Function Pipeline System
 * @{
 */

#ifndef PIPES_H
#define PIPES_H

#include "Mental.h"
#include <stdlib.h> // Для realloc/free

/**
 * @brief Default capacity for newly created pipelines
 */
extern int default_pipeline_capacity;
int default_pipeline_capacity = 24;

/**
 * @brief Function pointer type for pipeline functions
 * @param arg Void pointer argument passed to the function
 * @return Void pointer result (can be NULL if unused)
 */
typedef void* (*fptr)(void*);

/**
 * @brief Configuration structure for pipeline creation
 */
typedef struct FunctionPipelineInfo {
    MentalStructureType sType;   ///< Structure type identifier
    u32 capacity;               ///< Requested pipeline capacity (0 for default)
    u32 initialCapacity;        ///< Initial capacity (reserved for future use)
} FunctionPipelineInfo;

/**
 * @brief Main pipeline structure
 * 
 * Contains dynamic array of function pointers and management information
 */
typedef struct FunctionPipeline {
    MentalStructureType sType;   ///< Structure type identifier
    u32 size;                   ///< Current allocated size of functions array
    u32 last_index;             ///< Next available index for new functions
    fptr* functions;            ///< Dynamic array of function pointers
} FunctionPipeline;

/**
 * @brief Creates a new function pipeline
 * @param info Configuration parameters for the pipeline
 * @param pipe Pointer to pipeline structure to initialize
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 * 
 * @note If info.capacity is 0, default_pipeline_capacity will be used
 */
MentalResult mentalCreatePipeline(FunctionPipelineInfo info, FunctionPipeline* pipe) {
    if (!pipe) {
        MENTAL_WARN("Pipeline is NULL.");
        return MENTAL_ERROR;
    }

    pipe->sType = info.sType;
    pipe->size = info.capacity > 0 ? info.capacity : default_pipeline_capacity;
    pipe->last_index = 0;
    pipe->functions = (fptr*)malloc(pipe->size * sizeof(fptr));
    
    if (!pipe->functions) {
        MENTAL_WARN("Memory allocation failed.");
        return MENTAL_ERROR;
    }
    return MENTAL_OK;
}

/**
 * @brief Destroys pipeline and releases all resources
 * @param pipe Pointer to pipeline to destroy
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 */
MentalResult mentalDestroyPipeline(FunctionPipeline* pipe) {
    if (!pipe) return MENTAL_ERROR;
    free(pipe->functions);
    pipe->functions = NULL;
    pipe->size = 0;
    pipe->last_index = 0;
    return MENTAL_OK;
}

/**
 * @brief Adds function to the pipeline
 * @param pipe Pointer to target pipeline
 * @param func Function pointer to add
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 * 
 * @note Automatically expands pipeline capacity if needed
 */
MentalResult addF(FunctionPipeline* pipe, fptr func) {
    if (!pipe || !func) {
        MENTAL_WARN("Invalid Pipeline or function.");
        return MENTAL_ERROR;
    }

    // Если массив заполнен, увеличиваем его
    if (pipe->last_index >= pipe->size) {
        u32 new_size = pipe->size * 2;
        fptr* new_functions = (fptr*)realloc(pipe->functions, new_size * sizeof(fptr));
        if (!new_functions) {
            MENTAL_WARN("Failed to expand Pipeline.");
            return MENTAL_ERROR;
        }
        pipe->functions = new_functions;
        pipe->size = new_size;
    }

    pipe->functions[pipe->last_index++] = func;
    return MENTAL_OK;
}

/**
 * @brief Removes function from pipeline by index
 * @param pipe Pointer to target pipeline
 * @param index Index of function to remove
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 * 
 * @note Shifts all subsequent functions to fill the gap
 */
MentalResult removeF(FunctionPipeline* pipe, u32 index) {
    if (!pipe || index >= pipe->last_index) {
        MENTAL_WARN("Invalid index or Pipeline.");
        return MENTAL_ERROR;
    }

    // Сдвигаем все элементы после index на 1 влево
    for (u32 i = index; i < pipe->last_index - 1; i++) {
        pipe->functions[i] = pipe->functions[i + 1];
    }
    pipe->last_index--;

    return MENTAL_OK;
}

/**
 * @brief Executes function at specified index
 * @param pipe Pointer to target pipeline
 * @param index Index of function to execute
 * @param args Argument to pass to the function
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 * 
 * @note Passes void* argument to the target function
 */
MentalResult executeByIndex(FunctionPipeline* pipe, u32 index, void* args) {
    if (!pipe || index >= pipe->last_index) {
        MENTAL_WARN("Invalid index or Pipeline.");
        return MENTAL_ERROR;
    }
    
    if (!pipe->functions[index]) {
        MENTAL_WARN("Function at index is NULL.");
        return MENTAL_ERROR;
    }
    
    pipe->functions[index](args);
    return MENTAL_OK;
}

/**
 * @brief Executes all functions in pipeline sequentially
 * @param pipe Pointer to target pipeline
 * @param args Argument to pass to all functions
 * @return MentalResult MENTAL_OK on success, MENTAL_ERROR on failure
 * 
 * @note Passes the same argument to all functions in pipeline
 */
MentalResult executeAll(FunctionPipeline* pipe, void* args) {
    if (!pipe) {
        MENTAL_WARN("Pipeline is NULL.");
        return MENTAL_ERROR;
    }
    
    for (u32 i = 0; i < pipe->last_index; i++) {
        if (!pipe->functions[i]) {
            MENTAL_WARN("Found NULL function at index %u", i);
            continue;
        }
        pipe->functions[i](args);
    }
    
    return MENTAL_OK;
}

/** @} */ // End of FunctionPipeline group


/* ==================== USAGE EXAMPLES ==================== */

/**
 * @example SimplePipeline.c
 * 
 * Basic pipeline usage example:
 * @code
 * #include "Pipes.h"
 * 
 * void* print_int(void* arg) {
 *     printf("Processing: %d\n", *(int*)arg);
 *     return NULL;
 * }
 * 
 * void* calculate_square(void* arg) {
 *     int val = *(int*)arg;
 *     printf("Square: %d\n", val * val);
 *     return NULL;
 * }
 * 
 * int main() {
 *     // Initialize pipeline
 *     FunctionPipeline pipe;
 *     FunctionPipelineInfo info = {0};
 *     mentalCreatePipeline(info, &pipe);
 * 
 *     // Add functions
 *     addF(&pipe, print_int);
 *     addF(&pipe, calculate_square);
 * 
 *     // Execute with different arguments
 *     int data = 5;
 *     executeAll(&pipe, &data);
 * 
 *     // Execute specific function
 *     data = 10;
 *     executeByIndex(&pipe, 1, &data); // Only calculate_square
 * 
 *     mentalDestroyPipeline(&pipe);
 *     return 0;
 * }
 * @endcode
 * 
 * Output:
 * @code
 * Processing: 5
 * Square: 25
 * Square: 100
 * @endcode
 */

/**
 * @example AdvancedPipeline.c
 * 
 * Advanced usage with different argument types:
 * @code
 * #include "Pipes.h"
 * #include <string.h>
 * 
 * typedef struct {
 *     int id;
 *     char name[20];
 * } User;
 * 
 * void* process_user(void* arg) {
 *     User* u = (User*)arg;
 *     printf("User %d: %s\n", u->id, u->name);
 *     return NULL;
 * }
 * 
 * void* process_stats(void* arg) {
 *     int* count = (int*)arg;
 *     printf("Total users processed: %d\n", *count);
 *     return NULL;
 * }
 * 
 * int main() {
 *     FunctionPipeline pipe;
 *     FunctionPipelineInfo info = {0};
 *     mentalCreatePipeline(info, &pipe);
 * 
 *     // Add heterogeneous functions
 *     addF(&pipe, process_user);
 *     addF(&pipe, process_stats);
 * 
 *     // Prepare data
 *     User u1 = {1, "Alice"};
 *     int user_count = 42;
 * 
 *     // Execute pipeline
 *     executeByIndex(&pipe, 0, &u1);  // process_user
 *     executeByIndex(&pipe, 1, &user_count); // process_stats
 * 
 *     mentalDestroyPipeline(&pipe);
 *     return 0;
 * }
 * @endcode
 * 
 * Output:
 * @code
 * User 1: Alice
 * Total users processed: 42
 * @endcode
 */
#endif // PIPES_H