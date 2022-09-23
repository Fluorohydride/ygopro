#!/bin/bash
set -x
set -o errexit

echo "$MACOS_CERTIFICATE" | base64 --decode --output cert.p12

security create-keychain -p "$MACOS_KEYCHAIN_PASSWORD" "$MACOS_KEYCHAIN_NAME" || true
security list-keychains -s login.keychain "$MACOS_KEYCHAIN_NAME"
security default-keychain -s "$MACOS_KEYCHAIN_NAME"
security unlock-keychain -p "$MACOS_KEYCHAIN_PASSWORD" "$MACOS_KEYCHAIN_NAME"
security import cert.p12 -P "$MACOS_CERTIFICATE_PASSWORD" -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple: -s -k "$MACOS_KEYCHAIN_PASSWORD" "$MACOS_KEYCHAIN_NAME";

CERT_IDENTITY_NAME=$(security find-identity -v -p codesigning $MACOS_KEYCHAIN_NAME | head -1 | grep -o '".*"' | tr -d '"')
/usr/bin/codesign --deep --sign "$CERT_IDENTITY_NAME" ygopro.app

security lock-keychain "$MACOS_KEYCHAIN_NAME"
