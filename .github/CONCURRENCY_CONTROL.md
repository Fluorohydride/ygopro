# Concurrency Control for Release Tag Race Conditions

## Problem Statement

When multiple commits are pushed to a branch in rapid succession, GitHub Actions triggers multiple workflow runs in parallel. Without concurrency control, these parallel runs can:

1. **Race to update release tags**: Multiple runs might try to update the same "latest" tag simultaneously
2. **Overwrite newer builds with older ones**: An older build that takes longer might complete after a newer build, overwriting the newer release
3. **Create duplicate or conflicting tags**: Concurrent tag creation operations can conflict

This was observed in the ygopro-core repository and addressed in [PR #803](https://github.com/Fluorohydride/ygopro-core/pull/803).

## Solution

The workflow configuration now includes a `concurrency` block that:

```yaml
concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true
```

### How It Works

- **`group`**: Workflow runs are grouped by workflow name and git reference (branch/tag)
  - Example: `Automated Test Build-refs/heads/master`
  - Each branch/ref gets its own concurrency group
  - Runs for different branches don't interfere with each other

- **`cancel-in-progress: true`**: When a new run starts in the same group:
  - Any currently running workflow in that group is automatically canceled
  - Only the most recent run proceeds to completion
  - Prevents outdated builds from completing and overwriting newer ones

## Benefits

1. **Prevents race conditions**: Only one workflow run per branch executes at a time
2. **Saves CI resources**: Older, stale runs are canceled automatically
3. **Ensures consistency**: The latest code changes always produce the final artifacts
4. **Protects release tags**: Eliminates concurrent tag update conflicts

## Verification

To verify this is working:

1. Make multiple rapid commits to the same branch
2. Check the Actions tab in GitHub
3. Observe that older workflow runs are automatically canceled when newer ones start
4. Only the most recent run completes successfully

## Migration Notes for Maintainers

- **No action required**: The change is backward compatible
- **Workflow behavior**: Developers may notice older builds being canceled automatically
- **CI time**: Overall CI time may be reduced as redundant builds are skipped
- **Monitoring**: Check the Actions tab to see cancellation in action

## References

- [GitHub Actions Concurrency Documentation](https://docs.github.com/en/actions/using-jobs/using-concurrency)
- [ygopro-core PR #803](https://github.com/Fluorohydride/ygopro-core/pull/803) - Original implementation
