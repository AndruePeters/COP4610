all:
	gcc -o part1.x part1.c
	gcc -o empty.x empty.c
	@bash -c "printf \"\n\n\""
	bash -c "strace -o part1.log ./part1.x"
	@bash -c "printf \"\n\n\""
	bash -c "strace -o empty.log ./empty.x"
	@bash -c "printf \"\n\nEmpty: \""
	bash -c "wc -l < empty.log"
	@bash -c "printf \"\npart1.x: \""
	bash -c "wc -l < part1.log"
