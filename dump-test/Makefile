json-test:
	clang -o test-json test-json.c
	./test-json > ./tmp

	@echo "    result:"
	cat ./tmp

	@echo "    run jq:"
	cat ./tmp | jq

