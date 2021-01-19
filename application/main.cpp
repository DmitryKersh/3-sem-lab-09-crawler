#include <boost/program_options.hpp>
#include <cstddef>
#include <iostream>
#include <string>

#include <crawler_lib.hpp>

namespace program_options = ::boost::program_options;
using program_options::value;

static program_options::options_description create_description_() noexcept {
  program_options::options_description description{"Options"};

  description.add_options()
      ("help,h", "Shows this help message")
      ("url,u", value<::std::string>(), "HTML page address")
      ("depth,d", value<::std::size_t>()->default_value(0), "Page search depth")
      ("network_threads,n", value<::std::size_t>()->default_value(0), "Number of downloader-threads")
      ("parser_threads,p", value<::std::size_t>()->default_value(0), "Number of parser-threads")
      ("output,o", value<::std::string>(), "Path to output file")
      ;

  return description;
}

auto const DESCRIPTION = create_description_();

program_options::variables_map parse_program_options(int const arguments_count, const char* const arguments[]) {
  program_options::variables_map variables;
  program_options::store(
      program_options::parse_command_line(arguments_count, arguments, DESCRIPTION),
      variables
  );
  variables.notify();

  return variables;
}

int main(int argc, char** argv) {
  auto const program_options = parse_program_options(argc, argv);

  if (program_options.count("help")) {
    std::cout << DESCRIPTION << std::endl;

    return 0;
  }

  if (!program_options.count("url")) {
    std::cerr << "Missing option: url" << std::endl;
    return 1;
  }

  if (!program_options.count("output")) {
    ::std::cerr << "Missing option: output" << ::std::endl;
    return 1;
  }

  std::string url = program_options["url"].as<std::string>();
  std::string output = program_options["output"].as<::std::string>();
  auto const depth = program_options["depth"].as<size_t>();
  auto const network_threads = program_options["network_threads"].as<size_t>();
  auto const parser_threads = program_options["parser_threads"].as<size_t>();

  // if all program options are correct, launch crawler
  Crawler::from(url, depth, network_threads, parser_threads, output).join();

  return 0;
}