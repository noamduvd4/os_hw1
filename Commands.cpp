#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <climits>
#include "Commands.h"


using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

/*
int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}
*/

int _parseCommandLine(const char* cmd_line, string* args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = s; //(char*)malloc(s.length()+1);
    //memset(args[i], 0, s.length()+1);
    //strcpy(args[i], s.c_str());
    //args[++i] = nullptr;
    i++;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:

  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  if(firstWord.compare("chprompt") == 0)
  {
    string args [25];
    int size;
    size=_parseCommandLine(cmd_line,args);
    if(size==0) // empty line
    {
      return nullptr;
    }
    if(size>1)
      SmallShell::getInstance().prompt_title=args[1];
    else
      SmallShell::getInstance().prompt_title=prompt_title="smash";
   return nullptr;
   
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if (firstWord.compare("pwd") == 0) {
     return new GetCurrDirCommand(cmd_line);
  }
  
  else if (firstWord.compare("cd") == 0) {
    return new ChangeDirCommand(cmd_line,&(SmallShell::getInstance().prv_path));
  }
  
  else if (firstWord.compare("jobs") == 0) {
    return new JobsCommand(cmd_line);
  }
  //TODO bugs enter by "sleep &100 "
  else if(firstWord.compare("kill") == 0)
  {
    return new KillCommand(cmd_line);
  }
  else {
    return new ExternalCommand(cmd_line);
  }
  return nullptr;
}


void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here
  // for example:
  Command* cmd = CreateCommand(cmd_line);
  if(cmd)
    cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}
void ShowPidCommand::execute()
{
  cout<<"smash pid is "<<getpid()<<'\n';
}
void GetCurrDirCommand::execute()
{
  char buff[PATH_MAX];
  getcwd(buff,PATH_MAX);
  string str (buff);
  cout<<str<<'\n';
}

void ChangeDirCommand::execute()
{
  string args [25];
  int size;
  size=_parseCommandLine(cmd_line, args);
  if (size>2) // too many arguments
  {
    cerr<<"smash error: cd: too many arguments"<<'\n';
    return;
  }
  else if (size==1)
    return;
  char buff[PATH_MAX];
  getcwd(buff,PATH_MAX);
  string str (buff);
  if (args[1]=="-")
  {
    if((*pastPwd).compare("")==0)
    {
      cerr<<"smash error: cd: OLDPWD not set"<<'\n';
      return;
    }
    else
    {
      if(chdir(pastPwd->c_str())==-1)
      {
       perror("smash error: chdir failed");//FIXME
       return; 
      }//TODO check return error or try catch
    }
    
  }
  else
  {
    if(chdir(args[1].c_str())==-1)
    {
      perror("smash error: chdir failed");//FIXME 
    }
  }
  *(pastPwd)=str;
  return;

}

void KillCommand::execute() //FIXME is order matter? + if job id  is not a number- invalid job id or argument?
{
  string args [25];
  int job_id_to_kill;
  int signal_number;
  int size;
  size=_parseCommandLine(cmd_line,args);
  if(size==3)
  {
    if(args[2].find_first_not_of("0123456789")==string::npos) // job id is a number
    {
      job_id_to_kill=stoi(args[2]);
      auto the_job = SmallShell::getInstance().all_jobs.getJobById(job_id_to_kill);
      if (the_job==nullptr) // job id not exist
      {
        cerr<<"smash error: kill: job-id "<<job_id_to_kill<<" does not exist\n";
        return;
      }
      else if (args[1][0]!='-')
      {
        cerr<<"smash error: kill: invalid arguments\n";
        return;
      }
      args[1].erase(args[1].begin());
      if(args[1].find_first_not_of("0123456789")!=string::npos)
      {
        cerr<<"smash error: kill: invalid arguments\n";
        return;
      }
      signal_number=stoi(args[1]);
      /*
      if(signal_number<1||signal_number>31)
      {
        cerr<<"smash error: kill: invalid arguments\n";
        return;
      }
      */
    }
    else//args[2] is not a number
    {
      cerr<<"smash error: kill: invalid arguments\n";
      return;
    }
  }
  else // not 3 arguments
  {
    cerr<<"smash error: kill: invalid arguments\n";
    return;
  }

  //valid input:
  auto the_job = SmallShell::getInstance().all_jobs.getJobById(job_id_to_kill);
  pid_t the_job_pid = the_job->job_pid;
  if (kill(the_job_pid, signal_number)==-1)//failed
    perror("smash error: kill failed");
  if (signal_number==SIGTSTP || signal_number==SIGSTOP)
  {
    the_job->job_status = STOPPED;
  }
  if (signal_number==SIGCONT)
  {
    the_job->job_status = BACKGROUND;
  }

}

void JobsCommand::execute()
{
  SmallShell::getInstance().all_jobs.printJobsList();
}

void ExternalCommand::execute()
{
  int status=0;
  bool back_command=_isBackgroundComamnd(this->cmd_line);
  char args[COMMAND_ARGS_MAX_LENGTH];
  strcpy(args,cmd_line);
  pid_t pid=fork();
  if(pid<0)//fork failed
  {
    perror("smash error: ");//FIXME return error is not okay.
  }
  else if(pid==0)//Son
  {
    setpgrp();
    _trim(args);
    _removeBackgroundSign(args);
    _trim(args);
    char path[11];
    char op[3];
    strcpy(path,"/bin/bash");
    strcpy(op,"-c");
    char* excv_arry[]={path,op,args,NULL};
    if(execv(excv_arry[0],excv_arry)==-1)//failed to execv
    {
      perror("smash erorr: execv failed");
     //FIXME fix error massage after understanding
    }

    exit(0);
  }
  else//Father
  {
    if(!back_command)
    {
      SmallShell::getInstance().current_foreground_pid=pid;
      waitpid(pid,&status,WUNTRACED);
      SmallShell::getInstance().current_foreground_pid=0;
    }
    else{
      SmallShell::getInstance().all_jobs.addJob(args,pid);
    }
  }
}


  void JobsList::addJob(const char* cmd_line,pid_t job_pid)
  {
    this->removeFinishedJobs();
    int job_id;
    if(jobs_list.empty())
      job_id = 1;
    else
      job_id = jobs_list.back()->job_id +1;  
    char copy_cmd [COMMAND_ARGS_MAX_LENGTH];
    strcpy(copy_cmd,cmd_line);
    JobEntry job_to_add(job_id, job_pid, cmd_line);
    std::shared_ptr<JobEntry> job_ptr = std::make_shared<JobEntry>(job_to_add);
    jobs_list.push_back(job_ptr);
  }

  void JobsList::printJobsList()
  {
    this->removeFinishedJobs();
    if(jobs_list.empty())
      return;
    for(auto job:jobs_list)
    {
        string cmd_str=string(job->job_cmd_line);
        double time_str=difftime(time(NULL),job->start_time);
        cout<<"["<<job->job_id<<"]"<<" "<<cmd_str<<" : "<<job->job_pid<<" ";
        cout<<time_str<<" secs";
        if(job->job_status==STOPPED)
          cout<<" (stopped)";
        cout<<"\n";
    }
  }
  void JobsList::removeFinishedJobs()
  {
    auto it=jobs_list.begin();
    while(it!=jobs_list.end())
    {
      if(waitpid((*it)->job_pid,NULL,WNOHANG)!=0)//childe finished and removed
      {
        auto temp=it;
        temp++;
        jobs_list.erase(it);
        it=temp;
      }
      else
        it++;
    }
  }
  
std::shared_ptr<JobsList::JobEntry> JobsList::getJobById(int jobId)
{
  for (auto job: jobs_list)
  {
    if (job->job_id==jobId)
      return job;
  }
  return nullptr;
}
