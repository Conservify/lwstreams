PROTOC_VERSION = 3.11.2
PROTOC = $(PROTOC_BIN)/protoc
BUILD ?= $(abspath build)
PROTOC_BIN = $(BUILD)/bin

default: all test

all: $(BUILD)
	cd $(BUILD) && cmake ../
	$(MAKE) -C $(BUILD)

test: all
	env GTEST_COLOR=1 $(MAKE) -C $(BUILD) testcommon test ARGS=-VV

doc: all
	make -C $(BUILD) doc

clean:
	rm -rf $(BUILD)

veryclean: clean
	rm -rf gitdeps

$(BUILD): third-party/protoc-$(PROTOC_VERSION)-linux-x86_64.zip
	mkdir -p $(BUILD)
	cd $(BUILD) && unzip ../third-party/protoc-$(PROTOC_VERSION)-linux-x86_64.zip

third-party/protoc-$(PROTOC_VERSION)-linux-x86_64.zip:
	wget "https://github.com/protocolbuffers/protobuf/releases/download/v$(PROTOC_VERSION)/protoc-$(PROTOC_VERSION)-linux-x86_64.zip"

.PHONY: doc
