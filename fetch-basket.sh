#!/bin/sh

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 <repository-name> [optional-branch-name] [optional-server-url]"
    echo "    repository-name: The name of the repository to fetch, e.g., 'me/my-basket-repo'."
    echo "    optional-branch-name: The branch to fetch (default is the main branch of the repository)."
    echo "    optional-server-url: The server URL (default is 'github.com')."
    echo "Examples:"
    echo "    $ $0 me/my-basket-repo main github.com"
    echo "    $ $0 me/my-basket-repo"
    exit 1
fi

repositoryName="$1"
branchName=""

if [ -n "$2" ]; then
    branchName="$2"
fi

if [ -n "$3" ]; then
    serverURL="$3"
else
    serverURL="github.com"
fi

eggHome="$HOME/.egg"
url="https://$serverURL/$repositoryName.git"
fetchDir="$eggHome/$repositoryName"

echo "Fetching '$url'"

mkdir -p "$(dirname "$eggHome/$repositoryName")"

if [ -d "$eggHome/$repositoryName" ]; then
    if [ -z "$eggHome" ]; then
        echo "Error: eggHome is not set."
        exit 1
    fi
    rm -rf "$eggHome/$repositoryName" # Remove existing directory if it exists to ensure a fresh clone
fi

if [ -n "$branchName" ]; then
    git clone --depth 1 --branch "$branchName" "$url" "$eggHome/$repositoryName"
    ret=$?
else
    git clone --depth 1 "$url" "$eggHome/$repositoryName"
    ret=$?
fi

if [ $ret -ne 0 ]; then
    echo "Failed to clone repository '$url'. Please check the repository URL."
    exit $ret
fi

echo "Baskets from '$repositoryName' have been fetched to '$fetchDir'."
