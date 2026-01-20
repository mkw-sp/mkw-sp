# Contributing to MKW-SP

## Setting up GitHub and Git

The first step is to create a fork of the MKW-SP repo, which can be done from the web UI. You then need to configure remotes in your local repo: typically, `origin` will point to your fork, while `upstream` will point to the one on the MKW-SP organization. You can add a remote with `git remote add <name> <URL>`.

## Opening an issue

Before writing any code, it is strongly advised to check if there is any existing issue for it on GitHub. If that's not the case, you can create one. In any case, please comment to inform other developers that you are working on it.

## Creating a branch

It is not recommended to work on your `main` branch. Instead, the practice is to create a branch for your feature. This can be done with `git switch -c <new-branch>`. The branch name should be all lower case, with kebab case.

## Writing code

For new code running on the Wii, C++ and the newer `REPLACE`/`REPLACED` patching system should generally be used. Standard library and SDK patches can optionally be written in C, and for low-level code, assembly can sometimes be used, but for the remaining cases these languages are being phased out.

Build tools are written in Python while standalone programs are written in Rust.

The codebase is automatically formatted using `clang-format` (15) and `cargo fmt`, this will be checked by CI and must be run before merge.

If you need a unoptimised build with debugging information, use `python3 build.py -- debug`.

## Testing your feature

All submitted patches must be tested. You can do it either on console or on Dolphin. When the behavior is expected to differ, you will have to test both.

## Getting your code upstream

When your branch is ready, you can open a pull request from the GitHub web UI. The continuous integration will run several checks and other developers will be able to make comments.

Usually, a PR should stay open at least 24 hours to give everyone a chance to review it. For larger PRs (at least 1000 lines or 5 commits), this is extended to 48 hours. This rule doesn't apply to urgent fixes.

In case of a disagreement, maintainers settle. If maintainers disagree with each other, @stblr settles.

To be merged, a PR should have no open comment, be approved by at least one maintainer, and pass the continuous integration. Ideally, PRs are merged as a fast-forward (not available in the GitHub web UI). If the history is clean (no merge or fixup commits) but the branch is out-of-date, we are falling back to a rebase merge, and a squash merge in the remaining cases. The `main` branch should never be force-pushed by maintainers, and should never contain merge commits (an exception is made for `git subtree`).
