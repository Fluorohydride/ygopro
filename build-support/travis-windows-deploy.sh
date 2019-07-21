#!/usr/bin/env bash

echo Starting deployment of $DEPLOY_BRANCH to GitHub.
cd deploy
git init
echo Created new local repo in deploy/
git checkout --orphan $DEPLOY_BRANCH
echo Orphan branch $DEPLOY_BRANCH created.
git config user.email deploy@travis-ci.org
git config user.name "Deployment Bot (from Travis CI)"
echo Preparing to deploy $DEPLOY_BRANCH.
git add -A .
git commit -qm "Deploy $DEPLOY_REPO to $DEPLOY_REPO:$DEPLOY_BRANCH"
git show --stat-count=10 HEAD
echo Pushing to remote.
git push --force https://$DEPLOY_TOKEN@github.com/$DEPLOY_REPO.git $DEPLOY_BRANCH:$DEPLOY_BRANCH
