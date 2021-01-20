//
// Created by dmitrykersh on 18.01.2021.
//

#ifndef CRAWLER_RESPONSEPARSER_HPP
#define CRAWLER_RESPONSEPARSER_HPP

#include <UrlReader.hpp>
#include <iostream>
#include <set>
#include <string>

struct ParsingResult {
    std::set<std::string> image_urls;
    std::set<std::string> child_urls;

    [[nodiscard]] bool operator==(ParsingResult const& other) const {
        return this == &other || (image_urls == other.image_urls && child_urls == other.child_urls);
    }
};

std::ostream& operator<<(std::ostream& os, ParsingResult const& result);

[[nodiscard]] ParsingResult parse_response(http_response_t const& response, bool parse_children);

#endif // CRAWLER_RESPONSEPARSER_HPP
