#include "board.h"
/**
 * Constructs a board of size NxN, initializing field positions.
 */
board::board(int N) : field_count(N * N), m_fields(field_count)
{
    columns = N;
    rows = N;
    width = N * (field_size + margin) + margin;
    height = N * (field_size + margin) + margin;

    // Initialize each field with its position
    for (LONG row = 0; row < rows; ++row) {
        for (LONG column = 0; column < columns; ++column) {
            auto& f = m_fields[row * columns + column];
            f.position.top = row * (field_size + margin) + margin;
            f.position.left = column * (field_size + margin) + margin;
            f.position.bottom = f.position.top + field_size;
            f.position.right = f.position.left + field_size;
        }
    }
}