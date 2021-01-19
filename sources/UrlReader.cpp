//
// Created by dmitrykersh on 18.01.2021.
//

#include <UrlReader.hpp>

std::optional<std::pair<std::string, std::string>> parse_url(std::string const& url) {
    size_t protocol_offset = 0;
    auto const protocol_delim = url.find("://");

    if (protocol_delim != std::string::npos) {
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

std::optional<http_response_t> read_from_url(std::string const& host, std::string const& port, std::string const& url) {
    try {
        asio::io_context context;
        beast::tcp_stream stream{context};
        stream.connect(tcp::resolver{context}.resolve(host, port));

        {
            http::request<http::string_body> request{http::verb::get, url, 11};
            request.set(http::field::host, host);
            request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            http::write(stream, request);
        }

        beast::flat_buffer buffer;
        http_response_t response;
        http::read(stream, buffer, response);

        return response;
    } catch (std::system_error& error) { return std::nullopt; }
}
