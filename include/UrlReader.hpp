//
// Created by dmitrykersh on 18.01.2021.
//

#ifndef CRAWLER_URLREADER_HPP
#define CRAWLER_URLREADER_HPP

#include <boost/beast/version.hpp>
#include <optional>
#include <string>
#include <utility>

#include <definitions.hpp>

typedef http::response<http::string_body> http_response_t;

std::optional<std::pair<std::string, std::string>> parse_url_parts(std::string const& url);

[[nodiscard]] std::optional<http_response_t> read_from_url(std::string const& host, std::string const& port,
                                                             std::string const& target);

#endif  // CRAWLER_URLREADER_HPP
