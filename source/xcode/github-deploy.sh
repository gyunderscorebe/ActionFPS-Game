#!/bin/bash
echo "CIRCLETAG=$CIRCLE_TAG"
if [ "$CIRCLE_TAG" ]; then
  TAG_ID=$CIRCLE_TAG
  SOURCE_NAME=./build/Debug/actionfps.dmg
  TARGET_NAME=actionfps.dmg
  TAG_URL=https://api.github.com/repos/ActionFPS/ActionFPS-Game/releases/tags/$TAG_ID
  curl --fail $TAG_URL || ( sleep 30 && curl --fail $TAG_URL || ( sleep 60 &&  curl --fail $TAG_URL || sleep 90 ))
  RELEASE_ID=$(curl --fail $TAG_URL | jq '.id')
  curl -H "Authorization: token $GITHUB_RELEASE_TOKEN" \
       -H "Accept: application/vnd.github.manifold-preview" \
       -H "Content-Type: application/octet-stream" \
       --data-binary @$SOURCE_NAME \
       "https://uploads.github.com/repos/ActionFPS/ActionFPS-Game/releases/$RELEASE_ID/assets?name=$TARGET_NAME"
fi
