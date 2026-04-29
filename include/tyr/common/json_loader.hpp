/*
 * Copyright (C) 2025-2026 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_JSON_LOADER_HPP_
#define TYR_COMMON_JSON_LOADER_HPP_

#include <boost/json.hpp>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace tyr::common
{

inline std::filesystem::path root_path()
{
    return std::filesystem::path(std::string(ROOT_DIR));
}

inline std::filesystem::path data_path(std::string_view relative_path)
{
    return root_path() / "data" / relative_path;
}

inline std::filesystem::path profiling_path(std::string_view relative_path)
{
    return root_path() / "profiling" / relative_path;
}

inline std::string read_file(const std::filesystem::path& path)
{
    auto stream = std::ifstream(path);
    if (!stream)
        throw std::runtime_error("Could not open file: " + path.string());

    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

inline boost::json::value load_json_file(const std::filesystem::path& path)
{
    return boost::json::parse(read_file(path));
}

}

#endif
