SUBDIRS = src

.PHONY: lint test test-legacy test-small test-medium test-large

lint:
	git ls-files '*.cc' '*.h' | grep -v pb | xargs cpplint --filter -build/c++11,-build/include_order,-build/include_subdir

test:
	bash -c "./scripts/test.sh $(TEST_TYPE)"
test-legacy:
	bash -c "./scripts/test.sh legacy"
test-small:
	bash -c "./scripts/test.sh small"
test-medium:
	bash -c "./scripts/test.sh medium"
test-large:
	bash -c "./scripts/test.sh large"

