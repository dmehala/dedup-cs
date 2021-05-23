#include "dedup/csv.hpp"

#include "dedup/helper.hpp"

namespace csv {

Row::Row()
    : m_rows()
    , m_header(nullptr)
{}

Row::Row(std::vector<std::string>&& rows, std::shared_ptr<Header> header)
    : m_rows(std::move(rows))
    , m_header(header)
{}

bool Row::empty() const noexcept
{
    return m_rows.empty();
}

bool Row::operator==(const Row& rhs) const noexcept
{
    return m_rows == rhs.m_rows;
}

std::string& Row::operator[](const std::string& col_name)
{
    if (m_rows.empty())
        throw std::runtime_error("Empty row");

    auto it = m_header->find(col_name);
    if (it == m_header->cend())
        throw std::runtime_error(std::string("Column ") + col_name + " not found.");

    return m_rows[it->second];
}

void Reader::set_header()
{
    // read header and build an utility map
    std::getline(*m_input, m_buffer);
    m_content_beg = m_input->tellg();

    auto headers = helper::split(m_buffer, m_separator);
    m_cols = headers.size();

    for (int i = 0; i < m_cols; ++i)
        (*m_header_map)[headers[i]] = i;
}

Row Reader::read_row() noexcept
{
    while (!m_input->eof()) {
        std::getline(*m_input, m_buffer);

        auto split = helper::split(m_buffer, m_separator, m_cols);
        if (split.size() == m_cols)
            return Row(std::move(split), m_header_map);
    }

    return Row();
}

Reader::iterator Reader::begin() noexcept
{
    if (m_input->eof())
        reset();

    return Reader::iterator(this, read_row());
}

Reader::iterator Reader::end() noexcept
{
    return Reader::iterator(this, Row());
}

} // namespace csv