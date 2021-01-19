// Copyright 2020 Your Name <your_email>

#include <Crawler.hpp>
#include <definitions.hpp>

Crawler::Crawler(std::size_t depth, const std::string &root_url,
                 size_t downloader_threads, size_t parser_threads,
                 const std::string &output_file_name)
    : depth_(depth),
      output_stream_(output_file_name),
      download_threadpool_(downloader_threads),
      parser_threadpool_(parser_threads),
      file_writer_queue_() {
  publish_downloader_job({root_url, 0});
  file_writer_thread_ = std::thread{[this] {
    while (true){
      std::unique_lock lock(file_writer_queue_mtx_);

      bool has_more_jobs;
      while (file_writer_queue_.empty() && (has_more_jobs = (running_jobs_ != 0))){
        file_writer_queue_cv_.wait(lock);
      }

      if (!has_more_jobs) break;

      auto const result = file_writer_queue_.front();
      file_writer_queue_.pop();
      lock.unlock();

      output_stream_ << result << ::std::endl;

      notify_finish_job_();

    }
  }};
}

Crawler Crawler::from(std::string const& url, ::std::size_t depth, ::std::size_t downloader_threads,
                            ::std::size_t parser_threads, std::string const& output_file_name) {
  return Crawler{depth, url,
                 downloader_threads == 0 ? ::std::thread::hardware_concurrency() : downloader_threads,
                    parser_threads == 0 ? ::std::thread::hardware_concurrency() : parser_threads,
                    output_file_name};
}

void Crawler::notify_start_job_() {
  ::std::lock_guard lock{running_jobs_mtx_};
  ++running_jobs_;
  running_jobs_cv_.notify_all();
}

void Crawler::notify_finish_job_() {
  ::std::lock_guard lock{running_jobs_mtx_};
  // needed for Crawler::join() to work correctly
  if (--running_jobs_ == 0) file_writer_queue_cv_.notify_one();
  running_jobs_cv_.notify_all();
}

void Crawler::publish_downloader_job(DownloaderJob &&job) {
  notify_start_job_();

  asio::post(download_threadpool_, [this, job]{
    auto const url_parts = parse_url(job.url);

    if (url_parts) {
      if (auto response = read_from_url(url_parts->first, "8080", url_parts->second))
        publish_parser_job({std::move(*response), job.depth});
    }

    notify_finish_job_();
  });
}

void Crawler::publish_parser_job(ParserJob &&job) {
  notify_start_job_();

  asio::post(download_threadpool_, [this, job]{
    // if we reach maximum depth we don't parse children
    const bool parse_children = job.depth < depth_;

    auto result = parse_response(job.response, parse_children);

    // writing image urls
    for (auto image_url : result.image_urls)
      publish_file_writer_job(std::move(image_url));

    if (parse_children) {
      for (auto child_url : result.child_urls)
        publish_downloader_job({std::move(child_url), depth_ + 1});
    }

    notify_finish_job_();
  });
}

void Crawler::publish_file_writer_job(::std::string&& job) {
  notify_start_job_();

  ::std::lock_guard lock{file_writer_queue_mtx_};
  file_writer_queue_.push(::std::move(job));
  file_writer_queue_cv_.notify_one();
}

void Crawler::join() {
  std::unique_lock lock(running_jobs_mtx_);

  // waiting all running jobs
  running_jobs_cv_.wait(lock, [this] { return running_jobs_ == 0; });

  // waiting all threads
  file_writer_thread_.join();
  download_threadpool_.join();
  parser_threadpool_.join();
}
