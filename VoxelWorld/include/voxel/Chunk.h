#include "../Tensor3.h"

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = 64;

class Chunk : public Tensor3<char, CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE> {
public:
    bool dirty = false;
};
