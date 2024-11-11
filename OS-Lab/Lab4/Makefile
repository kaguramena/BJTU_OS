simulate: simulator.c
	gcc -o $@ $<

test: simulate
	./simulate BestMatch > BestMatch.txt
	./simulate FirstMatch > FirstMatch.txt

eval: BestMatch.txt FirstMatch.txt
	@echo "FirstMatch"
	@grep "Memory Avg" < FirstMatch.txt
	@grep "Avg Search Number" < FirstMatch.txt
	@echo "BestMatch"
	@grep "Memory Avg" < BestMatch.txt
	@grep "Avg Search Number" < BestMatch.txt