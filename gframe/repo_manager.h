#ifndef REPOMANAGER_H
#define REPOMANAGER_H

#include <atomic>
#include <map>
#include <future>
#include <vector>
#include <mutex>
extern "C" {
#include <git2.h>
}

namespace ygo {

class RepoManager {
public:

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
		std::vector<std::string> commit_history;
		bool Sanitize();
	};
	struct RepoPayload {
		RepoManager* repo_manager = nullptr;
		git_repository* repo = nullptr;
		std::string path;
	};


	RepoManager() {
		git_libgit2_init();
	}
	~RepoManager() {
		git_libgit2_shutdown();
	}

	std::vector<GitRepo> GetReadyRepos();

	std::map<std::string, int> GetRepoStatus();

	size_t GetUpdatingRepos() {
		return working_repos.size();
	};

	bool AddRepo(GitRepo repo);

	void UpdateStatus(std::string repo, int percentage);

private:

	std::vector<GitRepo> available_repos;

	std::vector<std::string> CloneorUpdateThreaded(GitRepo repo);

	int jsgitpull(git_repository * repo, std::string repo_path, git_oid* id);

	bool CloneorUpdate(GitRepo repo);

	void UpdateReadyRepos();
	
	std::map<std::string, std::future<std::vector<std::string>>> working_repos;

	std::map<std::string, int> repos_status;
	std::mutex repos_status_mutex;
};

extern RepoManager repoManager;

}

#endif // REPOMANAGER_H
