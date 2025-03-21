#ifndef MM2S_H
#define MM2S_H

#include <cstdint>
#include <hls_stream.h>

#include "../typedefs.h"

void mm2s(uint64_t *training_data_mem, hls::stream<digit> training_data_stream);

#endif /* MM2S_H */
