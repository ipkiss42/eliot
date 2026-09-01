#!/bin/bash
set -e

# Extract the builtin-baseline hash from vcpkg.json
BASELINE_SHA=$(grep -o '"builtin-baseline": *"[^"]*"' vcpkg.json | sed 's/.*"builtin-baseline": *"\([^"]*\)".*/\1/')
echo "Target Baseline SHA: $BASELINE_SHA"

# Initialize a local repository folder
mkdir -p $VCPKG_ROOT
git init $VCPKG_ROOT

# Fetch only the exact baseline commit directly from GitHub (shallow fetch)
git -C $VCPKG_ROOT remote add origin https://github.com/microsoft/vcpkg.git
git -C $VCPKG_ROOT fetch --depth 1 origin "$BASELINE_SHA"
git -C $VCPKG_ROOT checkout FETCH_HEAD
