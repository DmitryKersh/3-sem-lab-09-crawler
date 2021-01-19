//
// Created by dmitrykersh on 18.01.2021.
//

#ifndef CRAWLER_CRAWLER_HPP
#define CRAWLER_CRAWLER_HPP

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <ResponseParser.hpp>
#include <UrlReader.hpp>

class Crawler{
 public:
  void join();
  // creates new crawler
  static Crawler from(std::string const& url, size_t depth, size_t downloader_threads,
                         size_t parser_threads, std::string const& output_file_name);

 private:
  // default initializer with no given jobs
  Crawler(std::size_t depth, std::string const& root_url, size_t downloader_threads,
  size_t parser_threads, std::string const& output_file_name);

  // depth of search
  const size_t depth_;

  // filestream
  std::ofstream output_stream_;

  // downloader threads
  asio::thread_pool download_threadpool_;

  // parser threads
  asio::thread_pool parser_threadpool_;

  // file-writer thread & queue
  std::thread file_writer_thread_;
  std::queue<std::string> file_writer_queue_ {};

  // thread utility
  std::atomic_size_t running_jobs_ = 0;
  std::mutex running_jobs_mtx_{};
  std::condition_variable running_jobs_cv_{};
  std::mutex file_writer_queue_mtx_ {};
  std::condition_variable file_writer_queue_cv_ {};

  struct DownloaderJob{
    std::string url;
    size_t depth;
  };

  struct ParserJob{
    http_response_t response;
    size_t depth;
  };

  void notify_start_job_();

  void notify_finish_job_();

  void publish_downloader_job(DownloaderJob&& job);

  void publish_parser_job(ParserJob&& job);

  void publish_file_writer_job(::std::string&& job);

};

#endif  // CRAWLER_CRAWLER_HPP
