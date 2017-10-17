APP_ABI := armeabi-v7a

APP_PLATFORM:=android-21

ifeq ($(findstring x86,$(TARGET_ARCH_ABI)),)
	APP_STL := stlport_static
endif
