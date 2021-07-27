APNRING_SAMPLE_V12_VERSION = 4.8.5
APNRING_SAMPLE_V12_SITE_METHOD = local
APNRING_SAMPLE_V12_SITE = $(TOPDIR)/package/rockchip/apnring_sample_v12/src
APNRING_SAMPLE_V12_INSTALL_TARGET = YES

define APNRING_SAMPLE_V12_INSTALL_TARGET_CMDS
        cp -rfd  $(@D)/lib/*  ${TARGET_DIR}/usr/lib/
	cp -rf $(@D)/app ${TARGET_DIR}/
	cp -rf $(@D)/ko ${TARGET_DIR}/
	mv ${TARGET_DIR}/app/S98_autoset_rtc  ${TARGET_DIR}/etc/init.d/

endef

$(eval $(generic-package))
