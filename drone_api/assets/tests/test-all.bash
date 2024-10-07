#!/bin/bash

echo "INFO: test_area:"
python -m unittest tests.test_area
echo "INFO: test_area_prop:"
python -m unittest tests.test_area_prop
echo "INFO: test_asset:"
python -m unittest tests.test_asset
