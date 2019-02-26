#ifndef REPOMANAGER_H
#define REPOMANAGER_H

#include <atomic>
#include <map>
#include <future>
#include <vector>
extern "C" {
#include <git2.h>
}

namespace ygo {

class RepoManager {
public:

	class GitRepo {
	public:
		std::string url = "";
		std::string repo_path = "";
		std::string data_path = "";
		std::string script_path = "script";
		std::string pics_path = "pics";
		std::string core_path = "";
		bool should_update = true;
		bool has_core = false;
		bool ready = false;
		std::string error = "";
		void Sanitize();
	};


	RepoManager() {
		git_libgit2_init();
	}
	~RepoManager() {
		git_libgit2_shutdown();
	}

	std::vector<GitRepo> GetReadyRepos();

	size_t GetUpdatingRepos() {
		return working_repos.size();
	};

	void AddRepo(GitRepo repo);

private:

	std::vector<GitRepo> available_repos;

	std::string CloneorUpdateThreaded(GitRepo repo);

	bool CloneorUpdate(GitRepo repo);

	void UpdateReadyRepos();

	std::map<std::string, std::future<std::string>> working_repos;
};

extern RepoManager repoManager;

}

#endif // REPOMANAGER_H
