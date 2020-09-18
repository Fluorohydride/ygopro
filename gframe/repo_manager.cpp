// Copyright (c) 2019-2020 Edoardo Lolletti <edoardo762@gmail.com>
// Copyright (c) 2020 Dylam De La Torre <dyxel04@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-or-later
// Refer to the COPYING file included.

#include "repo_manager.h"
#include <fmt/format.h>
#include "game_config.h"
#include "logging.h"
#include "utils.h"
#include "libgit2.hpp"

namespace ygo {

// GitRepo

// public

bool GitRepo::Sanitize() {
	if(url.empty())
		return false;
	if(repo_name.empty() && repo_path.empty()) {
		repo_name = Utils::GetFileName(url);
		repo_path = fmt::format("./repositories/{}", repo_name);
		if(repo_name.empty() || repo_path.empty())
			return false;
	}
	if(repo_name.empty()) {
		repo_name = Utils::GetFileName(repo_path);
	}
	if(repo_path.empty()) {
		repo_path = fmt::format("./repositories/{}", repo_name);
	}
	repo_path = fmt::format("./{}", repo_path);
	data_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, data_path));
	if(lflist_path.size())
		lflist_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, lflist_path));
	else
		lflist_path = Utils::NormalizePath(fmt::format("{}/lflists/", repo_path));
	if(script_path.size())
		script_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, script_path));
	else
		script_path = Utils::NormalizePath(fmt::format("{}/script/", repo_path));
	if(pics_path.size())
		pics_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, pics_path));
	else
		pics_path = Utils::NormalizePath(fmt::format("{}/pics/", repo_path));
	if(has_core || core_path.size()) {
		has_core = true;
		core_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, core_path));
	}
	return true;
}

// RepoManager

// public

RepoManager::~RepoManager() {
	fetchReturnValue = -1;
	for(const auto& kv : working_repos) {
		kv.second.wait();
	}
}

size_t RepoManager::GetUpdatingReposNumber() const {
	return working_repos.size();
};

std::vector<const GitRepo*> RepoManager::GetAllRepos() const {
	std::vector<const GitRepo*> res;
	for(const auto& repo : all_repos)
		res.insert(res.begin(), &repo);
	return res;
}

std::vector<const GitRepo*> RepoManager::GetReadyRepos() {
	// UpdateReadyRepos
	for(auto it = working_repos.begin(); it != working_repos.end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto results = it->second.get();
			for(auto& repo : available_repos) {
				if(repo->repo_path == it->first) {
					repo->error = results.error;
					repo->warning = results.warning;
					repo->commit_history_full = results.full_history;
					repo->commit_history_partial = results.partial_history;
					repo->ready = true;
					break;
				}
			}
			it = working_repos.erase(it);
			continue;
		}
		it++;
	}
	//
	std::vector<const GitRepo*> res;
	for(auto it = available_repos.begin(); it != available_repos.end();) {
		if(!(*it)->ready)
			break;
		res.push_back(*it);
		it = available_repos.erase(it);
	}
	return res;
}

std::map<std::string, int> RepoManager::GetRepoStatus() {
	std::lock_guard<std::mutex> lock(repos_status_mutex);
	return repos_status;
}

#define JSON_SET_IF_VALID(field, jsontype, cpptype) \
	if(obj[#field].is_##jsontype()) \
	tmp_repo.field = obj[#field].get<cpptype>();
void RepoManager::LoadRepositoriesFromJson(const nlohmann::json& configs) {
	try {
		if(configs.size() && configs.at("repos").is_array()) {
			try {
				for(auto& obj : configs["repos"].get<std::vector<nlohmann::json>>()) {
					if(obj["should_read"].is_boolean() && !obj["should_read"].get<bool>())
						continue;
					GitRepo tmp_repo;
					JSON_SET_IF_VALID(url, string, std::string);
					JSON_SET_IF_VALID(should_update, boolean, bool);
					if(tmp_repo.url == "default") {
#ifdef DEFAULT_LIVE_URL
						tmp_repo.url = DEFAULT_LIVE_URL;
#ifdef YGOPRO_BUILD_DLL
						tmp_repo.has_core = true;
#endif
#else
						continue;
#endif //DEFAULT_LIVE_URL
					} else if(tmp_repo.url == "default_anime") {
#ifdef DEFAULT_LIVEANIME_URL
						tmp_repo.url = DEFAULT_LIVEANIME_URL;
#else
						continue;
#endif //DEFAULT_LIVEANIME_URL
					} else {
						JSON_SET_IF_VALID(repo_path, string, std::string);
						JSON_SET_IF_VALID(repo_name, string, std::string);
						JSON_SET_IF_VALID(data_path, string, std::string);
						JSON_SET_IF_VALID(lflist_path, string, std::string);
						JSON_SET_IF_VALID(script_path, string, std::string);
						JSON_SET_IF_VALID(pics_path, string, std::string);
						JSON_SET_IF_VALID(is_language, boolean, bool);
						if(tmp_repo.is_language) {
							JSON_SET_IF_VALID(language, string, std::string);
						}
#ifdef YGOPRO_BUILD_DLL
						JSON_SET_IF_VALID(core_path, string, std::string);
						JSON_SET_IF_VALID(has_core, boolean, bool);
#endif
					}
					if(tmp_repo.Sanitize()) {
						AddRepo(std::move(tmp_repo));
					}
				}
			}
			catch(std::exception& e) {
				ErrorLog(fmt::format("Exception occurred: {}", e.what()));
			}
		}
	}
	catch(...) {}
}

// private

void RepoManager::AddRepo(GitRepo repo) {
	if(!TryCloneOrUpdate(repo))
		return;
	all_repos.push_front(repo);
	available_repos.push_back(&all_repos.front());
}

bool RepoManager::TryCloneOrUpdate(GitRepo repo) {
	if(working_repos.find(repo.repo_path) != working_repos.end())
		return false;
	working_repos[repo.repo_path] = std::async(std::launch::async, &RepoManager::CloneOrUpdateTask, this, repo);
	return true;
}

void RepoManager::SetRepoPercentage(const std::string& path, int percent)
{
	std::lock_guard<std::mutex> lock(repos_status_mutex);
	repos_status[path] = percent;
}

RepoManager::CommitHistory RepoManager::CloneOrUpdateTask(GitRepo _repo) {
	git_libgit2_init();
	if(gGameConfig->ssl_certificate_path.size())
		git_libgit2_opts(GIT_OPT_SET_SSL_CERT_LOCATIONS, gGameConfig->ssl_certificate_path.c_str(), "/system/etc/security/cacerts");
#ifdef _WIN32
	else
		git_libgit2_opts(GIT_OPT_SET_SSL_CERT_LOCATIONS, "SYSTEM", "");
#endif
	CommitHistory history;
	try {
		auto DoesRepoExist = [](const char* path) -> bool {
			git_repository* tmp = nullptr;
			int status = git_repository_open_ext(&tmp, path,
			             GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr);
			git_repository_free(tmp);
			return status == 0;
		};
		auto AppendCommit = [](std::vector<std::string>& v, git_commit* commit) {
			std::string message{git_commit_message(commit)};
			message.resize(message.find_last_not_of(" \n") + 1);
			auto authorName = git_commit_author(commit)->name;
			v.push_back(fmt::format("{:s}\nAuthor: {:s}\n", message, authorName));
		};
		auto QueryFullHistory = [&](git_repository* repo, git_revwalk* walker) {
			git_revwalk_reset(walker);
			// git log HEAD~1500..HEAD
			Git::Check(git_revwalk_push_head(walker));
			for(git_oid oid; git_revwalk_next(&oid, walker) == 0;)
			{
				auto commit = Git::MakeUnique(git_commit_lookup, repo, &oid);
				if(git_oid_iszero(&oid) || history.full_history.size() > 1500)
					break;
				AppendCommit(history.full_history, commit.get());
			}
		};
		auto QueryPartialHistory = [&](git_repository* repo, git_revwalk* walker) {
			git_revwalk_reset(walker);
			// git log HEAD..FETCH_HEAD
			Git::Check(git_revwalk_push_range(walker, "HEAD..FETCH_HEAD"));
			for(git_oid oid; git_revwalk_next(&oid, walker) == 0;)
			{
				auto commit = Git::MakeUnique(git_commit_lookup, repo, &oid);
				AppendCommit(history.partial_history, commit.get());
			}
		};
		const std::string& url = _repo.url;
		const std::string& path = _repo.repo_path;
		FetchCbPayload payload{this, path};
		if(DoesRepoExist(path.c_str())) {
			auto repo = Git::MakeUnique(git_repository_open_ext, path.c_str(),
			                            GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr);
			auto walker = Git::MakeUnique(git_revwalk_new, repo.get());
			git_revwalk_sorting(walker.get(), GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
			if(_repo.should_update) {
				try {
					// git fetch
					git_fetch_options fetchOpts = GIT_FETCH_OPTIONS_INIT;
					fetchOpts.callbacks.transfer_progress = &RepoManager::FetchCb;
					fetchOpts.callbacks.payload = &payload;
					auto remote = Git::MakeUnique(git_remote_lookup, repo.get(), "origin");
					Git::Check(git_remote_fetch(remote.get(), nullptr, &fetchOpts, nullptr));
					QueryPartialHistory(repo.get(), walker.get());
					// git reset --hard FETCH_HEAD
					git_oid oid;
					Git::Check(git_reference_name_to_id(&oid, repo.get(), "FETCH_HEAD"));
					auto commit = Git::MakeUnique(git_commit_lookup, repo.get(), &oid);
					Git::Check(git_reset(repo.get(), reinterpret_cast<git_object*>(commit.get()),
					                     GIT_RESET_HARD, nullptr));
				} catch(std::exception& e) {
					history.partial_history.clear();
					history.warning = e.what();
					ErrorLog(fmt::format("Warning occurred in repo {}: {}", url, e.what()));
				}
			}
			QueryFullHistory(repo.get(), walker.get());
		} else {
			Utils::DeleteDirectory(Utils::ToPathString(path + "/"));
			// git clone <url> <path>
			git_clone_options cloneOpts = GIT_CLONE_OPTIONS_INIT;
			cloneOpts.fetch_opts.callbacks.transfer_progress = &RepoManager::FetchCb;
			cloneOpts.fetch_opts.callbacks.payload = &payload;
			auto repo = Git::MakeUnique(git_clone, url.c_str(), path.c_str(), &cloneOpts);
			auto walker = Git::MakeUnique(git_revwalk_new, repo.get());
			git_revwalk_sorting(walker.get(), GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
			QueryFullHistory(repo.get(), walker.get());
		}
		SetRepoPercentage(path, 100);
	} catch(std::exception& e) {
		history.error = e.what();
		ErrorLog(fmt::format("Exception occurred in repo {}: {}", _repo.url, e.what()));
	}
	git_libgit2_shutdown();
	return history;
}

int RepoManager::FetchCb(const git_indexer_progress* stats, void* payload) {
	int percent;
	if(stats->received_objects != stats->total_objects) {
		percent = (75 * stats->received_objects) / stats->total_objects;
	} else if(stats->total_deltas == 0) {
		percent = 75;
	} else {
		percent = 75 + ((25 * stats->indexed_deltas) / stats->total_deltas);
	}
	auto pl = static_cast<FetchCbPayload*>(payload);
	pl->rm->SetRepoPercentage(pl->path, percent);
	return pl->rm->fetchReturnValue;
}

}
