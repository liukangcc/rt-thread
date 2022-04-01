# ESP32C3 RT-Thread BSP
This is the Board support package for esp32c3 SoC to bring its support to RT-Thread, it glues the esp-idf hal and wireless libraries to the RT-Thread components
It is a work in progress, and has some key work to do, below some instructions on how to proceed here:

## Build system
Inside of espressif_hal/rtthread create the RT-Thread compatible SConscript plus SConstruct files, and add the following sources, they would places the basic 
support of esp32c3 hal, which can be used to develop RT-Thread standard device drivers: 

```
 folders: esp32c3/src , esp32c3/include
 folders: esp32shared/src , esp32shared/include
```

And the following files from esp-idf:

```
    ../../components/esp_common/include
    ../../components/esp_rom/include
    ../../components/esp_rom/include/esp32c3
    ../../components/esp32c3/include
    
    ../../components/esp_hw_support/port/esp32c3/private_include
    ../../components/esp_hw_support/port/esp32c3
    ../../components/esp_hw_support/include

    ../../components/riscv/include
  
    ../../components/hal/include 
    ../../components/hal/esp32c3/include
    
    ../../components/soc/esp32c3/include
    ../../components/soc/src/esp32c3/include
    ../../components/soc/include

    ../../components/driver/include

    ../../components/efuse/include
    ../../components/efuse/esp32c3/include
    ../../components/efuse/private_include/esp32c3

    ../../components/esp_system/include
    ../../components/esp_event/include
    ../../components/esp_timer/include
    ../../components/esp_timer/private_include
    ../../components/esp_netif/include
    ../../components/log/include
    ../../components/spi_flash/include
    ../../components/spi_flash/private_include
    ../../components/bootloader_support/include
    ../../components/bootloader_support/include_bootloader

    ../../components/esp_wifi/include/
    ../../components/esp_wifi/esp32c3/include

    ../../components/wpa_supplicant/include
    ../../components/wpa_supplicant/port/include
    ../../components/wpa_supplicant/src
    ../../components/wpa_supplicant/include/esp_supplicant
    ../../components/wpa_supplicant/src/crypto

    ../../components/soc/esp32c3/gpio_periph.c
    ../../components/esp_timer/src/ets_timer_legacy.c
	   ../../components/esp_hw_support/port/esp32c3/rtc_clk.c
    ../../components/esp_hw_support/port/esp32c3/rtc_init.c
    ../../components/esp_hw_support/port/esp32c3/rtc_time.c
    ../../components/esp_hw_support/port/esp32c3/rtc_clk_init.c
    ../../components/esp32c3/clk.c
    ../../components/esp_timer/src/esp_timer.c
    ../../components/esp_timer/src/esp_timer_impl_systimer.c
    ../../components/hal/esp32c3/systimer_hal.c
    ../../components/esp_hw_support/port/esp32c3/rtc_clk.c
    ../../components/driver/periph_ctrl.c
    ../../components/esp32c3/clk.c
    ../../components/log/log_noos.c
    ../../components/log/log.c

```

This will bring the basic support files from IDF that does not depend on FreeRTOS to the RT-Thread context.


## WIFI & Bluetooth:
Besides importing the files above, wifi and bluetooht needs an os_adpter file, which provides the function calls of the underlying OS to the BT and WIFI stack, so it is necessary to pick the files:

```
rtthread/esp32c3/src/bt/esp_bt_adapter.c
esp32c3/src/wifi/esp_wifi_adapter.c

```
And replaces the FreeRTOS function calls with the RT-Thread kernel functions equivalent, the current files were based on zephyr OS port, so most of the FreeRTOS code was already removed, it just need to receive the RT-Thread equivalent functions.

After providing the adapter for the OS, the wifi and bt libraries from espressif_hal needs to be linked against the final binary, they re found inside of hal:

```
     # for bluetooth:
      btbb
      btdm_app
      ../../components/bt/controller/esp32c3-bt-lib/esp32c3
      coexist
      ../../components/esp_wifi/lib/esp32c3
      

     # for wifi:
      net80211
      core
      pp
      coexist
      phy
      mesh
    
      
```
Also the following source files are needed including wpa supplicant:

```
  ../../components/esp_wifi/src/phy_init.c
 src/common/esp_read_mac.c
 
   "src/wpa_supplicant/port/os_riscv.c"
   "../../components/wpa_supplicant/src/ap/ap_config.c"
   "../../components/wpa_supplicant/src/ap/ieee802_1x.c"
   "../../components/wpa_supplicant/src/ap/wpa_auth.c"
   "../../components/wpa_supplicant/src/ap/wpa_auth_ie.c"
   "../../components/wpa_supplicant/src/common/sae.c"
   "../../components/wpa_supplicant/src/common/wpa_common.c"
   "../../components/wpa_supplicant/src/crypto/aes-ctr.c"
   "../../components/wpa_supplicant/src/crypto/aes-siv.c"
   "../../components/wpa_supplicant/src/crypto/sha256-kdf.c"
   "../../components/wpa_supplicant/src/crypto/sha256-prf.c"
   "../../components/wpa_supplicant/src/crypto/aes-cbc.c"
   "../../components/wpa_supplicant/src/crypto/aes-ccm.c"
   "../../components/wpa_supplicant/src/crypto/aes-internal-dec.c"
   "../../components/wpa_supplicant/src/crypto/aes-internal-enc.c"
   "../../components/wpa_supplicant/src/crypto/aes-internal.c"
   "../../components/wpa_supplicant/src/crypto/aes-omac1.c"
   "../../components/wpa_supplicant/src/crypto/aes-unwrap.c"
   "../../components/wpa_supplicant/src/crypto/aes-wrap.c"
   "../../components/wpa_supplicant/src/crypto/aes-omac1.c"
   "../../components/wpa_supplicant/src/crypto/sha256-tlsprf.c"
   "../../components/wpa_supplicant/src/crypto/bignum.c"
   "../../components/wpa_supplicant/src/crypto/ccmp.c"
   "../../components/wpa_supplicant/src/crypto/crypto_mbedtls-bignum.c"
   "../../components/wpa_supplicant/src/crypto/crypto_mbedtls-ec.c"
   "../../components/wpa_supplicant/src/crypto/crypto_ops.c"
   "../../components/wpa_supplicant/src/crypto/crypto_internal-cipher.c"
   "../../components/wpa_supplicant/src/crypto/crypto_internal-modexp.c"
   "../../components/wpa_supplicant/src/crypto/crypto_internal-rsa.c"
   "../../components/wpa_supplicant/src/crypto/crypto_internal.c"
   "../../components/wpa_supplicant/src/crypto/des-internal.c"
   "../../components/wpa_supplicant/src/crypto/dh_group5.c"
   "../../components/wpa_supplicant/src/crypto/dh_groups.c"
   "../../components/wpa_supplicant/src/crypto/md4-internal.c"
   "../../components/wpa_supplicant/src/crypto/md5-internal.c"
   "../../components/wpa_supplicant/src/crypto/md5.c"
   "../../components/wpa_supplicant/src/crypto/ms_funcs.c"
   "../../components/wpa_supplicant/src/crypto/rc4.c"
   "../../components/wpa_supplicant/src/crypto/sha1-internal.c"
   "../../components/wpa_supplicant/src/crypto/sha1-pbkdf2.c"
   "../../components/wpa_supplicant/src/crypto/sha1.c"
   "../../components/wpa_supplicant/src/crypto/sha256-internal.c"
   "../../components/wpa_supplicant/src/crypto/sha256.c"
   "../../components/wpa_supplicant/src/esp_supplicant/esp_hostap.c"
   "../../components/wpa_supplicant/src/esp_supplicant/esp_wpa_main.c"
   "../../components/wpa_supplicant/src/esp_supplicant/esp_wpas_glue.c"
   "../../components/wpa_supplicant/src/esp_supplicant/esp_wpa3.c"
   "../../components/wpa_supplicant/src/rsn_supp/pmksa_cache.c"
   "../../components/wpa_supplicant/src/rsn_supp/wpa.c"
   "../../components/wpa_supplicant/src/rsn_supp/wpa_ie.c"
   "../../components/wpa_supplicant/src/utils/base64.c"
   "../../components/wpa_supplicant/src/utils/common.c"
   "../../components/wpa_supplicant/src/utils/ext_password.c"
   "../../components/wpa_supplicant/src/utils/uuid.c"
   "../../components/wpa_supplicant/src/utils/wpabuf.c"
   "../../components/wpa_supplicant/src/utils/wpa_debug.c"
   "../../components/wpa_supplicant/src/utils/json.c"
```