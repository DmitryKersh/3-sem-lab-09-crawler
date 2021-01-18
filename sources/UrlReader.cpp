//
// Created by dmitrykersh on 18.01.2021.
//

#include <UrlReader.hpp>

std::optional<std::pair<std::string, std::string>> parse_url_parts(std::string const& url){
  size_t protocol_offset = 0;
  auto const protocol_delim = url.find("://");

  if (protocol_delim != std::string::npos){
    if (url.substr(0, protocol_delim) != "http") { // if protocol is http
      return std::nullopt;
    } else {
      protocol_offset = 7; // length of "http://"
    }
  }

  auto const path_delimiter_index = url.find('/', protocol_offset);
  return path_delimiter_index == ::std::string::npos
         ? std::make_pair(url.substr(protocol_offset), "/")
         : std::make_pair(url.substr(protocol_offset, path_delimiter_index - protocol_offset),
                            url.substr(path_delimiter_index));
}
