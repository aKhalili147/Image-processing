SOURCES=$(wildcard *.c)

extractStrip: $(SOURCES)
	gcc $^ -o $@ -g

clean: 
	rm $(SOURCES) 
