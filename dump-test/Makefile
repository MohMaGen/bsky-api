json-test2:
	clang -o test-json2 test-json2.c
	./test-json2 > ./tmp

	@echo "    result:"
	cat ./tmp

	@echo "    run jq:"
	cat ./tmp | jq

json-test:
	clang -o test-json test-json.c
	./test-json > ./tmp

	@echo "    result:"
	cat ./tmp

	@echo "    run jq:"
	cat ./tmp | jq

