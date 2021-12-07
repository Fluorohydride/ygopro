// Copyright (c) 2019-2020 Edoardo Lolletti <edoardo762@gmail.com>
// Copyright (c) 2020 Dylam De La Torre <dyxel04@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-or-later
// Refer to the COPYING file included.

#ifndef REPOMANAGER_H
#define REPOMANAGER_H
#include <atomic>
#include <map>
#include <forward_list>
#include <string>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>
#include <git2/version.h>
#include <queue>
#include <condition_variable>
#include <thread>

// libgit2 forward declarations
struct git_repository;
#if LIBGIT2_VER_MAJOR>0 || LIBGIT2_VER_MINOR>=99
struct git_indexer_progress;
#else
struct git_transfer_progress;
using git_indexer_progress = git_transfer_progress;
#endif

namespace ygo {

class GitRepo {
public:
	// first = all changes history, second = only HEAD..FETCH_HEAD changes
	struct CommitHistory {
		std::vector<std::string> full_history;
		std::vector<std::string> partial_history;
		std::string error;
		std::string warning;
	};
	std::string url{};
	std::string repo_name{};
	std::string repo_path{};
	std::string data_path{};
	std::string lflist_path{"lflists"};
	std::string script_path{"script"};
	std::string pics_path{"pics"};
	std::string core_path{};
	std::string language{};
	bool should_update{true};
	bool has_core{false};
	bool ready{false};
	bool is_language{false};
	CommitHistory history;
	bool Sanitize();
	friend class RepoManager;
private:
	bool internal_ready{ false };
};

class RepoManager {
public:

	RepoManager();	
	// Cancel fetching of repos and synchronize with futures
	~RepoManager();

	size_t GetUpdatingReposNumber() const;
	std::vector<const GitRepo*> GetAllRepos() const;
	std::vector<const GitRepo*> GetReadyRepos(); // changes available_repos
	std::map<std::string, int> GetRepoStatus(); // locks mutex

	void LoadRepositoriesFromJson(const nlohmann::json& configs);
	bool TerminateIfNothingLoaded();
private:
	void TerminateThreads();
	std::forward_list<GitRepo> all_repos{};
	size_t all_repos_count{};
	std::vector<GitRepo*> available_repos{};
	std::map<std::string, int> repos_status{};
	std::queue<GitRepo*> to_sync;
	std::mutex syncing_mutex;
	std::condition_variable cv;
	std::vector<std::thread> cloning_threads;
	// Initialized with GIT_OK (0), changed to cancel fetching
	std::atomic<int> fetchReturnValue{0};

	void AddRepo(GitRepo&& repo);
	void SetRepoPercentage(const std::string& path, int percent);
	
	// Will be started on a new thread
	void CloneOrUpdateTask();
	
	// libgit2 Callbacks stuff
	struct GitCbPayload
	{
		RepoManager* rm;
		const std::string& path;
	};
	static int FetchCb(const git_indexer_progress* stats, void* payload);
	static void CheckoutCb(const char* path, size_t completed_steps, size_t total_steps, void* payload);
};

extern RepoManager* gRepoManager;

}

#endif // REPOMANAGER_H
