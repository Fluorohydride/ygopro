#include "repo_manager.h"
#include "game.h"

namespace ygo {

RepoManager repoManager;
/**
*The "fetchead_foreach_cb" and "jsgitpull" functions were taken from Peter Johan Salomonsen's code:
https://github.com/fintechneo/libgit2/blob/master/emscripten_hacks/jslib.c#L577
*/
int fetchead_foreach_cb(const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload) {
	struct payload_struct {
		RepoManager* repo_manager = nullptr;
		git_repository* repo = nullptr;
		std::string path;
		int curr = 0;
		int total;
	};
	payload_struct* _payload = static_cast<payload_struct*>(payload);
	git_repository* repo = _payload->repo;
	_payload->curr++;
	int fetch_percent = (((100 * _payload->curr) / _payload->total) / 2) + 50;
	_payload->repo_manager->UpdateStatus(_payload->path, fetch_percent);
	if(is_merge) {
		git_annotated_commit * fetchhead_annotated_commit;
		git_annotated_commit_lookup(&fetchhead_annotated_commit, repo, oid);

		git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
		merge_opts.file_flags = static_cast<git_merge_file_flag_t>(GIT_MERGE_FILE_STYLE_DIFF3 | GIT_MERGE_FILE_DIFF_MINIMAL);

		git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
		checkout_opts.checkout_strategy = (GIT_CHECKOUT_SAFE | GIT_CHECKOUT_ALLOW_CONFLICTS | GIT_CHECKOUT_CONFLICT_STYLE_DIFF3);

		const git_annotated_commit *mergeheads[] = { fetchhead_annotated_commit };

		git_merge(repo, mergeheads, 1, &merge_opts, &checkout_opts);

		git_merge_analysis_t analysis;
		git_merge_preference_t preference = GIT_MERGE_PREFERENCE_NONE;
		git_merge_analysis(&analysis, &preference, repo, mergeheads, 1);

		git_annotated_commit_free(fetchhead_annotated_commit);

		if(analysis == GIT_MERGE_ANALYSIS_NORMAL) {
			//printf("Normal merge\n");
			git_signature * signature;
			git_signature_default(&signature, repo);

			git_oid commit_oid, oid_parent_commit, tree_oid;

			git_commit * parent_commit;

			git_commit * fetchhead_commit;

			git_commit_lookup(&fetchhead_commit, repo, oid);

			git_reference_name_to_id(&oid_parent_commit, repo, "HEAD");
			git_commit_lookup(&parent_commit, repo, &oid_parent_commit);

			git_tree *tree;
			git_index *index;

			git_repository_index(&index, repo);
			if(git_index_has_conflicts(index)) {
				//printf("Index has conflicts\n");

				git_index_conflict_iterator *conflicts;
				const git_index_entry *ancestor;
				const git_index_entry *our;
				const git_index_entry *their;
				int err = 0;

				git_index_conflict_iterator_new(&conflicts, index);

				while((err = git_index_conflict_next(&ancestor, &our, &their, conflicts)) == 0) {
					fprintf(stderr, "conflict: a:%s o:%s t:%s\n",
							ancestor ? ancestor->path : "NULL",
							our->path ? our->path : "NULL",
							their->path ? their->path : "NULL");
				}

				if(err != GIT_ITEROVER) {
					fprintf(stderr, "error iterating conflicts\n");
				}

				git_index_conflict_iterator_free(conflicts);

			} else {
				//printf("No conflicts\n");
				git_index_write_tree(&tree_oid, index);
				git_tree_lookup(&tree, repo, &tree_oid);

				git_commit_create_v(&commit_oid, repo, "HEAD", signature, signature, nullptr, "Merge with remote", tree, 2, parent_commit, fetchhead_commit);

				git_repository_state_cleanup(repo);
			}

			git_index_free(index);
			git_commit_free(parent_commit);
			git_commit_free(fetchhead_commit);
		} else if(analysis == (GIT_MERGE_ANALYSIS_NORMAL | GIT_MERGE_ANALYSIS_FASTFORWARD)) {
			//printf("Fast forward\n");
			git_reference * ref = nullptr;

			git_reference_lookup(&ref, repo, "refs/heads/master");
			git_reference *newref;
			git_reference_set_target(&newref, ref, oid, "pull");

			git_reference_free(newref);
			git_reference_free(ref);

			git_repository_state_cleanup(repo);
		} else if(analysis == GIT_MERGE_ANALYSIS_UP_TO_DATE) {
			//printf("All up to date\n");
			git_repository_state_cleanup(repo);
		} else {
			//printf("Don't know how to merge\n");
		}

		//printf("Merged %s\n", remote_url);
	}
	return 0;
}
int RepoManager::jsgitpull(git_repository * repo, std::string repo_path, git_oid* id) {
	git_remote *remote = nullptr;
	const git_transfer_progress *stats;
	git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

	int res = git_remote_lookup(&remote, repo, "origin");

	if(res < 0) {
		git_remote_free(remote);
		return res;
	}

	/* Set up the callbacks (only update_tips for now) */
	/*fetch_opts.callbacks.update_tips = &update_cb;
	fetch_opts.callbacks.sideband_progress = &progress_cb;
	fetch_opts.callbacks.transfer_progress = transfer_progress_cb;
	fetch_opts.callbacks.credentials = cred_acquire_cb;*/

	fetch_opts.callbacks.transfer_progress = [](const git_transfer_progress *stats, void *payload)->int {
		int fetch_percent = ((100 * stats->received_objects) / stats->total_objects) / 2;
		RepoPayload* repo_status = static_cast<RepoPayload*>(payload);
		repo_status->repo_manager->UpdateStatus(repo_status->path, fetch_percent);
		return 0;
	};

	RepoPayload payload;
	payload.repo_manager = this;
	payload.path = repo_path;
	fetch_opts.callbacks.payload = &payload;

	/**
	 * Perform the fetch with the configured refspecs from the
	 * config. Update the reflog for the updated references with
	 * "fetch".
	 */
	res = git_remote_fetch(remote, nullptr, &fetch_opts, "fetch");
	if(res < 0) {
		git_remote_free(remote);
		return res;
	}
	/**
	 * If there are local objects (we got a thin pack), then tell
	 * the user how many objects we saved from having to cross the
	 * network.
	 */

	int count = 0;

	git_repository_fetchhead_foreach(repo, [](const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload)->int {
		(*(int*)payload)++;
		return 0;
	}, &count);

	if(id) {
		git_repository_fetchhead_foreach(repo, [](const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload)->int {
			git_oid* id = static_cast<git_oid*>(payload);
			*id = *oid;
			return 1;
		}, id);
	}

	struct {
		RepoManager* repo_manager = nullptr;
		git_repository* repo = nullptr;
		std::string path;
		int curr = 0;
		int total;
	} payload2;
	payload2.repo_manager = this;
	payload2.repo = repo;
	payload2.path = repo_path;
	payload2.total = count;

	res = git_repository_fetchhead_foreach(repo, &fetchead_foreach_cb, &payload2);

	return res;
}


git_commit * getLastCommit(git_repository * repo, git_oid* id) {
	int rc;
	git_commit * commit = nullptr; /* the result */
	git_oid oid_parent_commit;  /* the SHA1 for last commit */

	/* resolve HEAD into a SHA1 */
	rc = git_reference_name_to_id(&oid_parent_commit, repo, "HEAD");
	if(rc == 0) {
		/* get the actual commit structure */
		rc = git_commit_lookup(&commit, repo, &oid_parent_commit);
		if(rc == 0) {
			if(id)
				*id = oid_parent_commit;
			return commit;
		}
	}
	return nullptr;
}
std::vector<std::string> GetCommitsInfo(git_repository *repo, git_oid id) {
	std::vector<std::string> res;
	static git_oid tmp = { 0 };
	git_revwalk* rewalk;
	if(git_revwalk_new(&rewalk, repo) < 0) {
		const git_error *e = giterr_last();
		return { e->message };
	}
	git_revwalk_sorting(rewalk, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
	if(git_revwalk_push_head(rewalk) < 0) {
		const git_error *e = giterr_last();
		return { e->message };
	}
	/*if(git_revwalk_hide_glob(rewalk, "tags/*") < 0) {
		const git_error *e = giterr_last();
		return { e->message };
	}*/
	/*git_object *obj;
	if(git_revparse_single(&obj, repo, "HEAD~10") < 0){
		const git_error *e = giterr_last();
		return { e->message };
	}
	if(git_revwalk_hide(rewalk, git_object_id(obj)) < 0) {
		git_object_free(obj);
		const git_error *e = giterr_last();
		return { e->message };
	}*/
	git_oid oid;
	while(git_revwalk_next(&oid, rewalk) == 0) {
		git_commit *c;
		git_commit_lookup(&c, repo, &oid);
		if(!memcmp(oid.id,id.id, sizeof(oid.id)))
			break;
		std::string message = git_commit_message(c);
		std::string author = git_commit_author(c)->name;
		git_commit_free(c);
		res.push_back(message + "\n by " + author);
		if(res.size() >= 30)
			break;
	}
	res.insert(res.begin(), "");
	return res;
}
std::pair<std::vector<std::string>, std::vector<std::string>> RepoManager::CloneorUpdateThreaded(GitRepo _repo) {
	git_libgit2_init();
	git_repository *repo = nullptr;
	int res = 0;
	std::string errstring;
	git_oid id = { 0 };
	if(git_repository_open_ext(
		&repo, _repo.repo_path.c_str(), GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr) == 0) {
		if(_repo.should_update) {
			std::vector<std::string> modified;
			git_status_foreach(repo, [](const char *path, unsigned int status_flags, void *payload) -> int {
				((std::vector<std::string>*)payload)->push_back(path);
				return 0;
			}, &modified);
			for(auto& file : modified) {
				Utils::Deletefile(Utils::ParseFilename(_repo.repo_path + "/" + file));
			}
			git_commit* commit = getLastCommit(repo, &id);
			if(commit) {
				git_reset(repo, (git_object*)commit, GIT_RESET_HARD, nullptr);
				git_commit_free(commit);
			}
			res = jsgitpull(repo, _repo.repo_path, nullptr);
		} else {
			UpdateStatus(_repo.repo_path, 100);
		}
	} else {
		repo = nullptr;
		Utils::Deletedirectory(Utils::ParseFilename(_repo.repo_path + "/"));
		git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
		opts.checkout_opts.progress_cb = [](const char *path, size_t cur, size_t tot, void *payload) {
			int fetch_percent = (((100 * cur) / tot) / 2) + 50;
			RepoPayload* repo_status = static_cast<RepoPayload*>(payload);
			repo_status->repo_manager->UpdateStatus(repo_status->path, fetch_percent);
		};
		RepoPayload payload;
		payload.repo_manager = this;
		payload.path = _repo.repo_path;
		opts.checkout_opts.progress_payload = &payload;
		opts.fetch_opts.callbacks.transfer_progress = [](const git_transfer_progress *stats, void *payload) {
			int fetch_percent = ((100 * stats->received_objects) / stats->total_objects) / 2;
			RepoPayload* repo_status = static_cast<RepoPayload*>(payload);
			repo_status->repo_manager->UpdateStatus(repo_status->path, fetch_percent);
			return 0;
		};
		opts.fetch_opts.callbacks.payload = &payload;
		res = git_clone(&repo, _repo.url.c_str(), _repo.repo_path.c_str(), &opts);
	}
	std::vector<std::string> new_commits;
	if(res < 0) {
		const git_error *e = giterr_last();
		if(e)
			return { { e->message }, new_commits };
		git_libgit2_shutdown();
		return {{ "Unknown error" }, new_commits };
	}
	if(id.id) {
		new_commits = GetCommitsInfo(repo, id);
	}
	auto all_commits = GetCommitsInfo(repo, { 0 });
	git_repository_free(repo);
	git_libgit2_shutdown();
	return { all_commits, new_commits };
}

void RepoManager::UpdateReadyRepos() {
	for(auto it = working_repos.begin(); it != working_repos.end();) {
		if(it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto results = it->second.get();
			for(auto& repo : available_repos) {
				if(repo.repo_path == it->first) {
					repo.error = results.first[0];
					results.first.erase(results.first.begin());
					repo.commit_history_full = results.first;
					repo.commit_history_partial = results.second;
					repo.ready = true;
					break;
				}
			}
			it = working_repos.erase(it);
			continue;
		}
		it++;
	}
}

bool RepoManager::CloneorUpdate(GitRepo repo) {
	if(working_repos.find(repo.repo_path) != working_repos.end())
		return false;
	working_repos[repo.repo_path] = std::async(std::launch::async, &RepoManager::CloneorUpdateThreaded, this, repo);
	return true;
}

std::vector<RepoManager::GitRepo> RepoManager::GetReadyRepos() {
	std::vector<GitRepo> res;
	UpdateReadyRepos();
	for(auto it = available_repos.begin(); it != available_repos.end();) {
		if(!it->ready)
			break;
		res.push_back(*it);
		it = available_repos.erase(it);
	}
	return res;
}

std::map<std::string, int> RepoManager::GetRepoStatus() {
	return repos_status;
}

bool RepoManager::AddRepo(GitRepo repo) {
	if(CloneorUpdate(repo)) {
		available_repos.push_back(repo);
		return true;
	}
	return false;
}

void RepoManager::UpdateStatus(std::string repo, int percentage) {
	repos_status_mutex.lock();
	repos_status[repo] = percentage;
	repos_status_mutex.unlock();
}

bool RepoManager::GitRepo::Sanitize() {
	if(url.empty())
		return false;
	if(repo_name.empty() && repo_path.empty()) {
		repo_name = Utils::GetFileName(url);
		repo_path = fmt::format("./expansions/{}", repo_name);
		if(repo_name.empty() || repo_path.empty())
			return false;
	}
	if(repo_name.empty()) {
		repo_name = Utils::GetFileName(repo_path);
	}
	if(repo_path.empty()) {
		repo_path = fmt::format("./expansions/{}", repo_name);
	}
	repo_path = fmt::format("./{}", repo_path);
	data_path = Utils::NormalizePath(fmt::format("{}/{}/", repo_path, data_path));
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

}
