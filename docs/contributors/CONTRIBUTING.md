# Contributing to Dream vs. Dream

## Issues

- Submit issue tickets for bug reports.
  - Please try to check for duplicate issues and if the issue has been addressed by another issue or development.
  - Include as much information as possible. For example:
    - Error type: CMake generation error, build error, runtime crash error, unintended runtime behavior, incorrect documentation, etc.
    - Runtime distribution: where you got the distribution, version, etc.
    - Your computer: operating system, architecture, etc.
    - Steps to reproduce the issue if possible
- Suggestions and ideas are welcome for now but should be clearly marked as such.
  - If you are a developer, submitting an issue under suggestions or ideas can also allow you to receive feedback about a proposition and let other developers know what you are working on.

## Pull requests

- This git repo roughly follows [git-flow](https://nvie.com/posts/a-successful-git-branching-model/).
You are advised to somewhat follow this workstyle.
  - The `master` branch should **always** reflect the latest stable release or version.
  - The `develop` branch should **always** reflect a stable build (e.g. builds without error, no incomplete features). It does not have to be as polished as the `master` branch.
  - The `release/` branches should be branched from the `develop` branch and should only contain small changes, like bug fixes, until it is deemed stable and merged into the `master` branch.
  - Other branches such as `feature/`, `fix/`, `docs/`, or any other branch can reflect anything (WIP or complete). The only real requirement is that the name of the branch is descriptive of the changes you made.
- To contribute to this git repo, you should fork, perform your changes, and then submit a pull request.
- Incomplete features merged with the `master`, `develop`, or `release/` branches will most likely be rejected.
If you have an incomplete feature that you want other developers to look at, submit a pull request to any other branch along with your progress and plans for the feature.
- Complete features should still be in a pull request to a feature branch as that makes it easier for the feature to be saved for later.
Features merged with `develop` or `release/` branches are less likely to be accepted.
- Small compatible changes like modernizing code is more likely to be accepted than larger incompatible changes such as adding a new third-party library. If in doubt, communicate with other developers.
- Make sure that your changes at least works on your computer.
