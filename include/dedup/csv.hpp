#pragma once

#include <istream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace csv {

namespace separator {
    const char tab    = '\t';
    const char comma  = ',';
}

using Header = std::unordered_map<std::string, int>;

// @brief Utility class to manipulate csv rows and provides nice way to query by column
//        Example: row["year"] -> 1993
class Row final
{
public:
    Row();
    Row(std::vector<std::string>&& rows, std::shared_ptr<Header> header);

    bool empty() const noexcept;

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
    Reader(std::ifstream& input, const char separator = separator::comma);
    Reader(const std::string& input, const char separator = separator::comma);  ///< For testing purpose

    inline bool is_eof() const noexcept { return m_input->eof(); }

    Row read_row() noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;

private:
    void set_header();
    inline void reset() noexcept { m_input->clear(); m_input->seekg(m_content_beg, m_input->beg); }

private:
    int                             m_cols;         ///< Number of columns by row
    int                             m_content_beg; 
    const char                      m_separator;
    std::string                     m_buffer;
    std::unique_ptr<std::istream>   m_input;
    std::shared_ptr<Header>         m_header_map;
};

} // namespace csv