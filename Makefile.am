SUBDIRS = src

.PHONY: lint

lint:
	git ls-files '*.cc' '*.h' | grep -v pb | xargs cpplint --filter -build/c++11,-build/include_order,-build/include_subdir
