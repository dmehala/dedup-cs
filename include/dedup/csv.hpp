#pragma once

#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace csv {

using Header = std::unordered_map<std::string, int>;

// @brief Utility class to manipulate csv rows and provides nice way to query by column
//        Example: row["year"] -> 1993
class Row final
{
public:
    Row() = default;
    Row(std::vector<std::string>&& rows, std::shared_ptr<Header> header);

    bool operator==(const Row& rhs) const noexcept;

    std::string& operator[](const std::string& col_name);

private:
    std::vector<std::string>    m_rows;
    std::shared_ptr<Header>     m_header;
};

class Reader final
{
    class iterator
    {
    public:
        iterator() = default;
        iterator(Reader* reader, Row&& row) : parent(reader), current_row(std::move(row)) {}

        Row& operator*() { return current_row; }
        Row* operator->() { return &(current_row); }

        iterator& operator++()     ///< Pre increment iterator
        {
            current_row = parent->read_row();
            return *this;
        }

        iterator  operator++(int)  ///< Post increment iterator
        {
            auto tmp = *this;
            current_row = parent->read_row();
            return tmp;
        }

        iterator& operator--();

        bool operator==(const iterator& rhs) const noexcept
        {
            return parent == rhs.parent && current_row == rhs.current_row;
        }

        bool operator!=(const iterator& rhs) const noexcept
        {
            return !operator==(rhs);
        }

    private:
        Row     current_row;
        Reader* parent;
    };

public:
    Reader(std::ifstream& input, const char delimiter = ',');

    // Reader(const Reader&)  = delete;
    // Reader&(const Reader&) = delete;

    inline void reset() noexcept { m_input.clear(); m_input.seekg(0, m_input.beg); }
    inline bool is_eof() const noexcept { return m_input.eof(); }

    Row read_row() noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;

private:
    int                     m_cols;
    bool                    m_eof;
    const char              m_delimiter;
    std::string             m_buffer;
    std::ifstream&          m_input;
    std::shared_ptr<Header> m_header_map;
};

}