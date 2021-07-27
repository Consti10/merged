APNRING_SAMPLE_VERSION = 4.8.5
APNRING_SAMPLE_SITE_METHOD = local
APNRING_SAMPLE_SITE = $(TOPDIR)/package/rockchip/apnring_sample/src
APNRING_SAMPLE_INSTALL_TARGET = YES

define APNRING_SAMPLE_INSTALL_TARGET_CMDS
        cp -rfd  $(@D)/lib/*  ${TARGET_DIR}/usr/lib/
	cp -rf $(@D)/app ${TARGET_DIR}/
	cp -rf $(@D)/ko ${TARGET_DIR}/
	mv ${TARGET_DIR}/app/S98_yourself ${TARGET_DIR}/etc/init.d/
	mv ${TARGET_DIR}/app/S98_lunch_init  ${TARGET_DIR}/etc/init.d/
endef

$(eval $(generic-package))
