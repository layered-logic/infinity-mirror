package com.v1

import android.content.Context
import android.net.nsd.NsdManager
import android.net.nsd.NsdServiceInfo
import android.os.Handler
import android.os.Looper
import com.facebook.react.bridge.Arguments
import com.facebook.react.bridge.Promise
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.bridge.WritableArray
import com.facebook.react.bridge.WritableMap
import java.net.Inet4Address
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.atomic.AtomicBoolean

// Discovers _layeredlogic._tcp. services on the local network via Android's
// NsdManager. Replaces the prior subnet-scan approach in find-mirror.ts:
// scanning 254 IPs in parallel saturates OkHttp's dispatcher with stuck-on-ARP
// requests and the mirror's response often never gets a slot before the timeout.
// mDNS sidesteps that entirely — the firmware advertises itself, we listen.
class MdnsModule(reactContext: ReactApplicationContext)
    : ReactContextBaseJavaModule(reactContext) {

  override fun getName(): String = NAME

  // Run discovery for `timeoutMs`, resolve every found service to host+port,
  // return the list. Resolves are serialized — on API < 30, NsdManager
  // documents resolveService as not safe for concurrent use, and we want
  // one code path that works back to minSdk 24.
  @ReactMethod
  fun findMirrors(timeoutMs: Int, promise: Promise) {
    val nsdManager = reactApplicationContext.applicationContext
        .getSystemService(Context.NSD_SERVICE) as NsdManager

    val results = mutableListOf<WritableMap>()
    val resolveQueue = ConcurrentLinkedQueue<NsdServiceInfo>()
    val resolving = AtomicBoolean(false)
    val finished = AtomicBoolean(false)

    val resolveListener = object : NsdManager.ResolveListener {
      override fun onServiceResolved(serviceInfo: NsdServiceInfo) {
        // Filter to IPv4 — the firmware only listens on IPv4 (port 80, http://).
        // An IPv6-only resolve would yield a host string that fetch() can't use.
        val host = serviceInfo.host
        if (host is Inet4Address) {
          synchronized(results) {
            val map = Arguments.createMap()
            map.putString("host", host.hostAddress ?: "")
            map.putInt("port", serviceInfo.port)
            val attrs = Arguments.createMap()
            serviceInfo.attributes?.forEach { (k, v) ->
              attrs.putString(k, String(v))
            }
            map.putMap("attributes", attrs)
            results.add(map)
          }
        }
        resolving.set(false)
        pumpQueue(nsdManager, resolveQueue, resolving, this)
      }

      override fun onResolveFailed(serviceInfo: NsdServiceInfo, errorCode: Int) {
        // Drop the failure on the floor — there's no useful action to take and
        // we don't want one bad responder to tank the whole scan. The next
        // resolve in the queue moves forward.
        resolving.set(false)
        pumpQueue(nsdManager, resolveQueue, resolving, this)
      }
    }

    val discoveryListener = object : NsdManager.DiscoveryListener {
      override fun onDiscoveryStarted(serviceType: String) {}
      override fun onDiscoveryStopped(serviceType: String) {}
      override fun onStartDiscoveryFailed(serviceType: String, errorCode: Int) {
        if (finished.compareAndSet(false, true)) {
          promise.reject("MDNS_START_FAILED",
              "discovery start failed code=$errorCode")
        }
      }
      override fun onStopDiscoveryFailed(serviceType: String, errorCode: Int) {}
      override fun onServiceFound(serviceInfo: NsdServiceInfo) {
        resolveQueue.add(serviceInfo)
        pumpQueue(nsdManager, resolveQueue, resolving, resolveListener)
      }
      override fun onServiceLost(serviceInfo: NsdServiceInfo) {}
    }

    try {
      nsdManager.discoverServices(SERVICE_TYPE, NsdManager.PROTOCOL_DNS_SD,
          discoveryListener)
    } catch (e: Exception) {
      if (finished.compareAndSet(false, true)) {
        promise.reject("MDNS_DISCOVER_THREW", e)
      }
      return
    }

    Handler(Looper.getMainLooper()).postDelayed({
      try { nsdManager.stopServiceDiscovery(discoveryListener) } catch (_: Exception) {}
      if (finished.compareAndSet(false, true)) {
        val arr: WritableArray = Arguments.createArray()
        synchronized(results) { results.forEach { arr.pushMap(it) } }
        promise.resolve(arr)
      }
    }, timeoutMs.toLong())
  }

  // Single-flight resolve loop. compareAndSet ensures only one resolveService
  // is in flight at a time; either the discovery callback or the previous
  // resolve callback can call this and only the first one wins.
  private fun pumpQueue(
    nsdManager: NsdManager,
    queue: ConcurrentLinkedQueue<NsdServiceInfo>,
    resolving: AtomicBoolean,
    listener: NsdManager.ResolveListener,
  ) {
    if (!resolving.compareAndSet(false, true)) return
    val next = queue.poll()
    if (next == null) {
      resolving.set(false)
      return
    }
    @Suppress("DEPRECATION")
    nsdManager.resolveService(next, listener)
  }

  companion object {
    const val NAME = "Mdns"
    // Trailing dot per Bonjour spec; NsdManager accepts both forms but the
    // dotted form matches what the firmware publishes.
    const val SERVICE_TYPE = "_layeredlogic._tcp."
  }
}
