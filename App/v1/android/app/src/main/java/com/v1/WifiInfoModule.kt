package com.v1

import android.content.Context
import android.net.wifi.WifiManager
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReactMethod

class WifiInfoModule(reactContext: ReactApplicationContext)
    : ReactContextBaseJavaModule(reactContext) {

  override fun getName(): String = NAME

  // Returns the device's current Wi-Fi IPv4 address as a dotted string,
  // or rejects if Wi-Fi isn't connected. WifiManager.connectionInfo is
  // deprecated on API 31+ but still functional; the modern path
  // (ConnectivityManager.getNetworkCapabilities) is more code for the
  // same thing.
  @ReactMethod
  fun getIpAddress(promise: Promise) {
    try {
      val wifiManager = reactApplicationContext.applicationContext
          .getSystemService(Context.WIFI_SERVICE) as WifiManager
      val raw = wifiManager.connectionInfo.ipAddress
      if (raw == 0) {
        promise.reject("WIFI_NOT_CONNECTED", "no Wi-Fi IP available")
        return
      }
      // WifiManager returns the IP as a little-endian int.
      val ip = "${raw and 0xff}.${(raw shr 8) and 0xff}.${(raw shr 16) and 0xff}.${(raw shr 24) and 0xff}"
      promise.resolve(ip)
    } catch (e: Exception) {
      promise.reject("WIFI_INFO_ERROR", e)
    }
  }

  companion object {
    const val NAME = "WifiInfo"
  }
}
