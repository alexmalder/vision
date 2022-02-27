run:
	mkdir -p build
	cd build && cmake .. && make && ./vision

tarantool:
	cd .ansible && \
		ansible-playbook main.yml \
		-e "TNT_USER=${TNT_USER}" \
		-e "TNT_PASSWORD=${TNT_PASSWORD}"

docs:
	cd doxygen && doxygen Doxygen
	cd doxygen && go run main.go
