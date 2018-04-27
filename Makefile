BUILD=build

default: all

$(BUILD):
	mkdir -p $(BUILD)

all: $(BUILD)
	cd $(BUILD) && cmake ../ && make

test: all
	cd $(BUILD) && make test

clean:
	rm -rf $(BUILD)

veryclean: clean
	rm -rf gitdeps

.PHONY:
