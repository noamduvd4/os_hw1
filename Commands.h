#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <list>
#include <time.h>
#include <memory>
const std::string WHITESPACE = " \n\r\t\f\v";
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
 protected:
  const char* cmd_line;
 public:
  Command(const char* cmd_line):cmd_line(cmd_line){}
  virtual ~Command() {};
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line):Command(cmd_line){}
  virtual ~BuiltInCommand(){}
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line):Command(cmd_line){}
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
  public:
  std::string* pastPwd;
  ChangeDirCommand(const char* cmd_line, std::string* pastPwd):BuiltInCommand(cmd_line),pastPwd(pastPwd){};
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
  virtual~GetCurrDirCommand(){} 
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
  virtual ~ShowPidCommand(){}
  void execute() override;
};

class JobsList; //FIXME: what. why do we need it?
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};
enum Status {BACKGROUND, STOPPED, FINISHED};


class JobsList {

  class JobEntry {
    public:
      int job_id;
      pid_t job_pid;
      Status job_status;
      std::string job_cmd_line;
      time_t start_time;
      JobEntry(int job_id,pid_t job_pid,const char* job_cmd_line):
      job_id(job_id),job_pid(job_pid),job_status(BACKGROUND),job_cmd_line(job_cmd_line){
        start_time = time(NULL); //FIXME if it returns error
      }
  };
 // TODO: Add your data members
 public:
  std::list <std::shared_ptr<JobsList::JobEntry>> jobs_list ;
  JobsList():jobs_list(){}
  ~JobsList(){}
  void addJob(const char* job_cmd_line,pid_t job_pid);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  std::shared_ptr<JobEntry> getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  //JobsCommand(const char* cmd_line, JobsList* jobs);
  JobsCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class CatCommand : public BuiltInCommand {
 public:
  CatCommand(const char* cmd_line);
  virtual ~CatCommand() {}
  void execute() override;
};


class SmallShell {
 private:
  // TODO: Add your data members
  SmallShell();
 public:
  std::string prompt_title="smash";
  std::string prv_path="";
  JobsList all_jobs;
  pid_t current_foreground_pid=0;
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
