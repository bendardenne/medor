//
// Created by bdardenn on 4/30/20.
//

#include "vcs/GitClient.hpp"

using namespace medor::vcs;
GitClient::GitClient(const std::string& repoPath) {
    git_libgit2_init();
    int error = git_repository_open(&_repo, repoPath.c_str());

    if (error < 0) {
        const git_error* e = git_error_last();
        throw std::runtime_error("Could not open git repository: " + std::string(e->message));
    }
}

GitClient::~GitClient() {
    git_repository_free(_repo);
    git_libgit2_shutdown();
}

std::vector<LogEntry> GitClient::log(pt::time_period date) {
    std::vector<LogEntry> entries;

    git_revwalk* walker = nullptr;
    git_revwalk_new(&walker, _repo);
    git_revwalk_push_head(walker);

    git_oid oid;
    while (!git_revwalk_next(&oid, walker)) {
        git_commit* commit;
        git_commit_lookup(&commit, _repo, &oid);

        pt::ptime commitTime = pt::from_time_t(git_commit_time(commit));

        // We look up commits by recent first, so if the filter period is after the commit time, we've gone too far.
        if (date.is_after(commitTime)) {
            git_commit_free(commit);
            break;
        }

        if (date.contains(commitTime)) {
            entries.push_back({.summary = git_commit_summary(commit), .date = commitTime});
        }

        git_commit_free(commit);
    }
    git_revwalk_free(walker);
    // We could also ask git for commits in reverse order, but this seems simpler
    std::reverse(entries.begin(), entries.end());
    return entries;
}
