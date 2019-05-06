#!/usr/bin/env bash

jupyter contrib nbextension install --user
jupyter nbextension enable codefolding/main
jupyter notebook "$@"
