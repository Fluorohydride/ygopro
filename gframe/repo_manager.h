#ifndef REPOMANAGER_H
#define REPOMANAGER_H

#include <atomic>
#include <map>
#include <forward_list>
#include <future>
#include <string>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>
extern "C" {
#include <git2.h>
}

namespace ygo {

class GitRepo {
public:
	std::string url = "";
	std::string repo_name = "";
	std::string repo_path = "";
	std::string data_path = "";
	std::string script_path = "script";
	std::string pics_path = "pics";
	std::string core_path = "";
	bool should_update = true;
	bool has_core = false;
	bool ready = false;
	std::string error = "";
	std::vector<std::string> commit_history_partial;
	std::vector<std::string> commit_history_full;
	bool Sanitize();
};

class RepoManager {
public:
	struct RepoPayload {
		RepoManager* repo_manager = nullptr;
		git_repository* repo = nullptr;
		std::string path;
	};


	RepoManager() {}
	~RepoManager() {}

	std::vector<const GitRepo*> GetReadyRepos();

	std::vector<const GitRepo*> GetAllRepos();

	std::map<std::string, int> GetRepoStatus();

	size_t GetUpdatingRepos() {
		return working_repos.size();
	};

	void LoadRepositoriesFromJson(const nlohmann::json& configs);

	bool AddRepo(GitRepo repo);

	void UpdateStatus(std::string repo, int percentage);

private:

	std::forward_list<GitRepo> all_repos;

	std::vector<GitRepo*> available_repos;

	std::pair<std::vector<std::string>, std::vector<std::string>> CloneorUpdateThreaded(GitRepo repo);

	int jsgitpull(git_repository * repo, std::string repo_path, git_oid* id);

	bool CloneorUpdate(GitRepo repo);

	void UpdateReadyRepos();
	
	std::map<std::string, std::future<std::pair<std::vector<std::string>, std::vector<std::string>>>> working_repos;

	std::map<std::string, int> repos_status;
	std::mutex repos_status_mutex;
};

extern RepoManager* gRepoManager;

}

#endif // REPOMANAGER_H
