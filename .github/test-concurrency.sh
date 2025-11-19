#!/bin/bash
# Test script to verify concurrency configuration in GitHub Actions workflow

set -e

echo "Testing concurrency configuration in .github/workflows/build.yml"
echo "================================================================"
echo ""

WORKFLOW_FILE=".github/workflows/build.yml"

if [ ! -f "$WORKFLOW_FILE" ]; then
    echo "ERROR: Workflow file not found: $WORKFLOW_FILE"
    exit 1
fi

echo "✓ Workflow file exists"

# Check if concurrency block is present
if grep -q "^concurrency:" "$WORKFLOW_FILE"; then
    echo "✓ Concurrency block found"
else
    echo "✗ ERROR: Concurrency block not found"
    exit 1
fi

# Check if group is defined
if grep -q "group:.*github.workflow.*github.ref" "$WORKFLOW_FILE"; then
    echo "✓ Concurrency group correctly configured"
else
    echo "✗ ERROR: Concurrency group not properly configured"
    exit 1
fi

# Check if cancel-in-progress is set to true
if grep -q "cancel-in-progress: *true" "$WORKFLOW_FILE"; then
    echo "✓ cancel-in-progress is set to true"
else
    echo "✗ ERROR: cancel-in-progress not set to true"
    exit 1
fi

# Verify YAML syntax
echo ""
echo "Validating YAML syntax..."
if command -v python3 &> /dev/null; then
    if python3 -c "import yaml; yaml.safe_load(open('$WORKFLOW_FILE'))" 2>/dev/null; then
        echo "✓ YAML syntax is valid"
    else
        echo "✗ ERROR: YAML syntax is invalid"
        exit 1
    fi
else
    echo "⚠ Warning: Python3 not available, skipping YAML validation"
fi

echo ""
echo "================================================================"
echo "All checks passed! ✓"
echo ""
echo "The concurrency control is properly configured to:"
echo "  - Group workflow runs by workflow name and git ref"
echo "  - Cancel in-progress runs when a new run starts"
echo "  - Prevent race conditions in release tag updates"
echo ""
