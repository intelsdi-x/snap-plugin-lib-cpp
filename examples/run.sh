#! /bin/bash

snapctl plugin load snap-collector-rando
snapctl plugin load snap-processor-graf
snapctl plugin load snap-publisher-log
snapctl task create -t task.json
