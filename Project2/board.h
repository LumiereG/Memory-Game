#pragma once
#include <windows.h>
#include <vector>
struct field
{
    RECT position;
};

class board
{
public:
    // Margin between fields
    static constexpr LONG margin = 5;

    // Size of each field
    static constexpr LONG field_size = 90;

    // Type alias for an array of fields
    using field_array = std::vector<field>;

    /**
   * Constructs the game board with a specified size.
   * @param N - Board dimension (NxN grid).
   */
    board(int N);

    /**
    * Returns a constant reference to the array of fields.
    */
    field_array const& fields() const { return m_fields; }

    // Number of columns and rows
    LONG columns;
    LONG rows;

    // Total number of fields
    LONG field_count;
    
    // Board width and height in pixels
    LONG width;
    LONG height;
    
    // Array storing all fields on the board
    field_array m_fields;
};
