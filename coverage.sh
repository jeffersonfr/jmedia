#!/bin/sh


if [ "$MODE" != "--update" ]; then
  rm -rf capture html ; mkdir -p capture html
fi

lcov --capture --directory . --output-file capture/coverage.info
lcov --remove capture/coverage.info "/usr/*" ".cache/*" "*tests*" --output-file capture/coverage.info

genhtml -o html --show-details --frames --keep-descriptions --num-spaces 2 --legend capture/coverage.info
