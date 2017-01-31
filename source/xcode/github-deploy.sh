#!/bin/bash
echo "CIRCLETAG=$CIRCLE_TAG"
if [ "$CIRCLE_TAG" ]; then
  TAG_ID=$CIRCLE_TAG
  SOURCE_NAME=./build/Release/actionfps.dmg
  TARGET_NAME=actionfps.dmg
  RELEASE_ID=$(curl https://api.github.com/repos/ActionFPS/ActionFPS-Game/releases/tags/64_bit_windows | jq '.id')
  curl -H "Authorization: token $GITHUB_RELEASE_TOKEN" \
       -H "Accept: application/vnd.github.manifold-preview" \
       -H "Content-Type: application/octet-stream" \
       --data-binary @$SOURCE_NAME \
       "https://uploads.github.com/repos/ActionFPS/ActionFPS-Game/releases/$RELEASE_ID/assets?name=$TARGET_NAME"
fi
