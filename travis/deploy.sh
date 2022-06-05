#!/usr/bin/env bash

set -euo pipefail

cd $DEPLOY_DIR
git init
git checkout --orphan $DEPLOY_BRANCH
git config user.email deploy@travis-ci.org
git config user.name "Deployment Bot (from Travis CI)"
git add -A .
git commit -qm "Deploy $DEPLOY_REPO to $DEPLOY_REPO:$DEPLOY_BRANCH"
git push -qf https://$DEPLOY_TOKEN@github.com/$DEPLOY_REPO.git $DEPLOY_BRANCH:$DEPLOY_BRANCH
