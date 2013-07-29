#ifndef BANDIT_REPORTERS_SINGLE_LINE_REPORTER_H
#define BANDIT_REPORTERS_SINGLE_LINE_REPORTER_H

namespace bandit {

  struct single_line_reporter : public reporter
  {
    single_line_reporter(std::ostream& stm,
        const failure_formatter& failure_formatter)
      : stm_(stm), failure_formatter_(failure_formatter)
    {}

    single_line_reporter(const failure_formatter& failure_formatter)
      : stm_(std::cout), failure_formatter_(failure_formatter)
    {}

    void test_run_starting() 
    {
      specs_run_ = 0;
      specs_succeeded_ = 0;
      specs_failed_ = 0;
      failures_.clear();
      contexts_.clear();
    }

    void test_run_complete() 
    {
      stm_ << std::endl;

      if(specs_run_ == 0 && test_run_errors_.size() == 0)
      {
        stm_ << "Could not find any tests." << std::endl;
        return;
      }

      if(test_run_errors_.size() > 0)
      {
        std::for_each(test_run_errors_.begin(), test_run_errors_.end(),
            [&](const std::string& error){
            stm_ << error << std::endl;
            });
      }


      if(specs_failed_ > 0)
      {
        stm_ << "There were failures!" << std::endl;
        std::for_each(failures_.begin(), failures_.end(), 
            [&](const std::string& failure) {
              stm_ << failure << std::endl;
            });
        stm_ << std::endl;
      }

      stm_ << "Test run complete. " << specs_run_ << " tests run. " << 
        specs_succeeded_ << " succeeded.";
      if(specs_failed_ > 0)
      {
        stm_ << " " << specs_failed_ << " failed.";
      }

      if(test_run_errors_.size() > 0)
      {
        stm_ << " " << test_run_errors_.size() << " test run errors.";
      }

      stm_ << std::endl;
    }

    void context_starting(const char* desc) 
    {
      contexts_.push_back(std::string(desc));
    }

    void context_ended(const char*) 
    {
      contexts_.pop_back();
    }

    void test_run_error(const char*, const struct test_run_error& err)
    {
      std::stringstream ss;
      ss << std::endl;
      ss << "Failed to run \"" << current_context_name() << "\": error \"" << err.what() << "\"" << std::endl;

      test_run_errors_.push_back(ss.str());
    }

    void it_starting(const char*) 
    {
      print_status_line();
    }

    void it_succeeded(const char*) 
    {
      specs_run_++;
      specs_succeeded_++;
      print_status_line();
    }

    void it_failed(const char* desc, const assertion_exception& ex)
    {
      specs_run_++;
      specs_failed_++;

      std::stringstream ss;
      ss << std::endl;
      ss << current_context_name() << " " << desc << ":" << std::endl;
      ss << failure_formatter_.format(ex);
      ss << ex.what();
      ss << std::endl;

      failures_.push_back(ss.str());

      print_status_line();
    }

    void it_unknown_error(const char* desc)
    {
      specs_run_++;
      specs_failed_++;

      std::stringstream ss;
      ss << std::endl;
      ss << current_context_name() << " " << desc << ":" << std::endl;
      ss << "Unknown exception";
      ss << std::endl;

      failures_.push_back(ss.str());

      print_status_line();
    }

    bool did_we_pass() const
    {
      return specs_failed_ == 0 && test_run_errors_.size() == 0;
    }

    private:
    void print_status_line()
    {
      stm_ << '\r';
      stm_ << "Executed " << specs_run_ << " tests.";

      if(specs_failed_)
      {
        stm_ << " " << specs_succeeded_ << " succeeded. " << specs_failed_ << " failed.";
      }
      stm_.flush();
    }

    std::string current_context_name()
    {
      std::string name;

      std::for_each(contexts_.begin(), contexts_.end(), [&](const std::string context){
          if(name.size() > 0)
          {
          name += " ";
          }

          name += context;
          });

      return name;
    }

    private:
    std::ostream& stm_;
    const failure_formatter& failure_formatter_;
    std::deque<std::string> contexts_;
    std::list<std::string> failures_;
    std::list<std::string> test_run_errors_;
    int specs_run_;
    int specs_succeeded_;
    int specs_failed_;
  };
}

#endif