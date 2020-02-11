.PHONY: all clean stage1 stage2

shared_dirs := common SelectorFramework/core
shared_src := $(shell find $(shared_dirs) -type f)

all: stage1 stage2

clean:
	@find . -regex '.*\.\(so\|pcm\|d\)' | xargs -rt rm

stage1/stage1_main_cc.so: stage1/*.hh stage1/*.cc
stage2/stage2_main_cc.so: external/FukushimaLambertW.* stage2/*.hh stage2/*.cc

stage1: stage1/stage1_main_cc.so
stage2: stage2/stage2_main_cc.so

%_cc.so: %.cc $(shared_src)
	root -l -b -q 'cling/Build.C("$<")'
