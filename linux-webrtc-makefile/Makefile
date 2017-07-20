CROSS_COMPILE := arm-poky-linux-gnueabi-
CC ?= $(CROSS_COMPILE)gcc
AR      = ar
RANLIB  = ranlib
STRIP := $(CROSS_COMPILE)strip
MACRO ?= -DWEBRTC_HAS_NEON
HEADER_CFLAGS ?= -I./src
CFLAGS := -Wall -O2 $(HEADER_CFLAGS) $(MACRO) -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a7 -pthread
CXX ?= $(CROSS_COMPILE)g++
CXXFLAGS := -Wall -O2 $(HEADER_CXXFLAGS) $(MACRO) -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a7 -pthread
TARGET ?= libwebrtc.a
TARGET_CC ?= $(CC)
TARGET_LDFLAGS = $(LDFLAGS) $(CXXLDFLAGS)
#修改这里
common_SRC_FILES := \
	src/webrtc/modules/audio_processing/aec/aec_core.c                    \
	src/webrtc/modules/audio_processing/aec/aec_rdft.c                    \
	src/webrtc/modules/audio_processing/aec/aec_resampler.c               \
	src/webrtc/modules/audio_processing/aec/echo_cancellation.c           \
	src/webrtc/modules/audio_processing/aecm/aecm_core.c                  \
	src/webrtc/modules/audio_processing/aecm/echo_control_mobile.c        \
	src/webrtc/modules/audio_processing/ns/noise_suppression.c            \
	src/webrtc/modules/audio_processing/ns/noise_suppression_x.c          \
	src/webrtc/modules/audio_processing/ns/ns_core.c                      \
	src/webrtc/modules/audio_processing/ns/nsx_core.c                     \
	src/webrtc/modules/audio_processing/utility/delay_estimator_wrapper.c \
	src/webrtc/modules/audio_processing/utility/delay_estimator.c         \
	src/webrtc/common_audio/fft4g.c                                       \
	src/webrtc/common_audio/ring_buffer.c                                 \
	src/webrtc/common_audio/signal_processing/complex_bit_reverse.c       \
	src/webrtc/common_audio/signal_processing/complex_fft.c               \
	src/webrtc/common_audio/signal_processing/copy_set_operations.c       \
	src/webrtc/common_audio/signal_processing/cross_correlation.c         \
	src/webrtc/common_audio/signal_processing/division_operations.c       \
	src/webrtc/common_audio/signal_processing/downsample_fast.c           \
	src/webrtc/common_audio/signal_processing/energy.c                    \
	src/webrtc/common_audio/signal_processing/get_scaling_square.c        \
	src/webrtc/common_audio/signal_processing/min_max_operations.c        \
	src/webrtc/common_audio/signal_processing/randomization_functions.c   \
	src/webrtc/common_audio/signal_processing/real_fft.c                  \
	src/webrtc/common_audio/signal_processing/spl_init.c                  \
	src/webrtc/common_audio/signal_processing/spl_sqrt.c                  \
	src/webrtc/common_audio/signal_processing/spl_sqrt_floor.c            \
	src/webrtc/common_audio/signal_processing/vector_scaling_operations.c \
	src/webrtc/modules/audio_processing/aec/aec_core_neon.c               \
	src/webrtc/modules/audio_processing/aec/aec_rdft_neon.c               \
	src/webrtc/modules/audio_processing/aecm/aecm_core_c.c                \
	src/webrtc/modules/audio_processing/aecm/aecm_core_neon.c             \
	src/webrtc/modules/audio_processing/ns/nsx_core_c.c                   \
	src/webrtc/modules/audio_processing/ns/nsx_core_neon.c                \
	src/webrtc/common_audio/signal_processing/cross_correlation_neon.c    \
	src/webrtc/common_audio/signal_processing/downsample_fast_neon.c      \
	src/webrtc/common_audio/signal_processing/min_max_operations_neon.c
#以下就不要修改了

SRC = $(common_SRC_FILES)
OBJS = $(SRC:.c=.o)
$(TARGET):  $(OBJS) $(CXXOBJS)
#TODO
ifeq (.so,$(findstring .so,$(TARGET)))
	$(TARGET_CC) -shared  $(OBJS) $(CXXOBJS) $(TARGET_LDFLAGS) -o $(TARGET)
endif

ifeq (.a,$(findstring .a,$(TARGET)))
	$(warning "the value of LOCAL_PATH is$(LOCAL_SRC_FILES)")  
	$(AR) cru $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
else
	$(TARGET_CC)  $(OBJS) $(CXXOBJS) $(TARGET_LDFLAGS) -o $(TARGET)
endif
	$(STRIP) $(TARGET)

sinclude $(SRC:.c=.d)
%.o: %.c
ifeq (,$(findstring .so,$(TARGET)))
	$(CC) $(CFLAGS) -o $@ -c $<
endif

ifeq (.a, $(findstring .a,$(TARGET)))
	$(CC) $(CFLAGS) -o $@ -c $<
else
	$(CC) -fPIC $(CFLAGS) -o $@ -c $<
endif

sinclude $(CXXSRC:.cpp=.dd)
%.oo: %.cpp
ifeq (,$(findstring .so,$(TARGET)))
	$(CXX) $(CXXFLAGS) -o $@ -c $<
else
	$(CXX) -fPIC $(CXXFLAGS) -o $@ -c $<
endif

%.d: %.c
	    @$(CC) -M $(CFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

%.dd: %.cpp
	    @$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.oo $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$


.PHONY: clean
clean:
	rm -rf $(SRC:.c=.d) $(CXXSRC:.cpp=.dd) $(OBJS) $(CXXOBJS)
	rm -f $(TARGET)
	
.PHONY: show_src
show_src:
	@echo $(SRC) $(CXXSRC); \
	echo $(OBJS) $(CXXOBJS)
