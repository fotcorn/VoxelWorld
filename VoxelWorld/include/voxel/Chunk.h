#include "../Tensor3.h"

const int CHUNK_SIZE = 8;
const int CHUNK_HEIGHT = 8;

class Chunk : public Tensor3<char, CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE> {
public:
    bool changed = true; // permanently changed, rebuild render data and put data into cache
    bool tempChanged = true; // only temporarily changed, rebuild render data but do not put into cache
};
