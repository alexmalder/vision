run:
	mkdir -p build
	cd build && cmake .. && make && ./vision

tarantool:
	ansible-playbook .ansible/main.yml
