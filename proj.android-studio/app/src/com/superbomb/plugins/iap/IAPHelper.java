package com.superbomb.plugins.iap;

// import android.app.Activity;
// import android.app.ProgressDialog;
// import android.content.Intent;
// import android.support.annotation.Nullable;
// import android.text.TextUtils;
// import android.util.Log;
// import android.view.View;
//
// import com.android.billingclient.api.BillingClient;
// import com.android.billingclient.api.BillingClient.BillingResponse;
// import com.android.billingclient.api.BillingClientStateListener;
// import com.android.billingclient.api.BillingFlowParams;
// import com.android.billingclient.api.Purchase;
// import com.android.billingclient.api.PurchasesUpdatedListener;
// import com.superbomb.json.SBJSON;
// import com.superbomb.plugins.PluginListener;
// import com.superbomb.series.rps.R;
//
// import org.cocos2dx.lib.Cocos2dxActivity;
//
// import java.io.IOException;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.Iterator;
// import java.util.List;
// import java.util.Map;
//
// public class IAPHelper implements PluginListener, BillingManager.BillingUpdatesListener {
//
//     private static final String TAG = "IAP";
//
//     private static IAPHelper instance = null;
//     public static IAPHelper getInstance() {
//         if( instance == null ) {
//             instance = new IAPHelper();
//         }
//
//         return instance;
//     }
//
//     private Cocos2dxActivity context;
//     private BillingManager billingManager;
//
//     private static class Item {
//         public String name;
//         public String itemId;
//         public boolean consumable;
//
//         public Item() {
//             consumable = true;
//         }
//
//     };
//
//     private final List<Item> items;
//
//     private IAPHelper() {
//         items = new ArrayList<>();
//     }
//
//     public static Cocos2dxActivity getContext() {
//         return getInstance().context;
//     }
//
//     public static BillingManager getBillingManager() {
//         return getInstance().billingManager;
//     }
//
//     /**
//      * 아이템 반환
//      */
//     public static List<Item> getItems() {
//         return getInstance().items;
//     }
//
//
// //    static Item getItemByName(const std::string &name);
//
//     public static Item getItemById(String itemId) {
//
//         List<Item> items = getItems();
//
//         for( Item item : items ) {
//             if( item.itemId.equals(itemId) ) {
//                 return item;
//             }
//         }
//
//         return null;
//     }
//
//     public static Item findItem(List<Purchase> purchases, String itemId) {
//
//         if( purchases == null ) {
//             return null;
//         }
//
//         for( Purchase purchase : purchases ) {
//             Item item = getItemById(purchase.getSku());
//
//             if( item != null && item.itemId.equals(itemId) ) {
//                 return item;
//             }
//         }
//
//         return null;
//     }
//
//     @Override
//     public void init(Cocos2dxActivity context) {
//
//         this.context = context;
//     }
//
//     public void initBillingManager(String base64Key) {
//
//         // Log.i(TAG, "initBillingManager: " + base64Key);
//
//         billingManager = new BillingManager(context, base64Key, this);
//     }
//
//     @Override
//     public void onStart() {
//     }
//
//     @Override
//     public void onStop() {
//     }
//
//     @Override
//     public void onResume() {
//
//         if( billingManager != null ) {
//             Log.i(TAG, "onResume isReady: " + billingManager.isBillingClientReady() + ", " + billingManager.isServiceConnected());
//         } else {
//             Log.i(TAG, "onResume billingManager is null");
//         }
//
//         /*
//         if( billingManager != null && billingManager.getBillingClientResponseCode() == BillingResponse.OK ) {
//             billingManager.queryPurchases();
//         }
//         */
//     }
//
//     @Override
//     public void onPause() {
//     }
//
//     @Override
//     public void onDestroy() {
//
//         if( billingManager != null ) {
//             billingManager.destroy();
//         }
//     }
//
//     @Override
//     public boolean onBackPressed() {
//         return false;
//     }
//
//     @Override
//     public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
//         return false;
//     }
//
//     /**
//      * IAP 모듈 초기화
//      */
//     public static void initIAP(String json) {
//
//         Map<String, Object> jsonObj = (Map<String, Object>)SBJSON.parse(json);
//
//         // items
//         Map<String, Object> itemsObj = (Map<String, Object>)jsonObj.get("items");
//         List<Item> items = getItems();
//
//         Iterator<String> it = itemsObj.keySet().iterator();
//
//         while( it.hasNext() ) {
//             String key = it.next();
//             Map<String, Object> obj = (Map<String, Object>)itemsObj.get(key);
//
//             Item item = new Item();
//             item.name = key;
//             item.itemId = (String)obj.get("id");
//
//             if( obj.containsKey("type") ) {
//                 String type = (String)obj.get("type");
//                 item.consumable = type.equals("consumable");
//             }
//
//             items.add(item);
//         }
//
//         // base64 key
//         final String base64Key = (String)jsonObj.get("key");
//
//         // BillingManager 초기화
//         getContext().runOnUiThread(new Runnable() {
//             @Override
//             public void run() {
//                 getInstance().initBillingManager(base64Key);
//             }
//         });
//     }
//
//     /**
//      * 인벤토리 업데이트
//      */
//     private void updateInventory(final PurchasesUpdatedListener listener) {
//
//         context.runOnUiThread(new Runnable() {
//             @Override
//             public void run() {
//
//                 billingManager.queryPurchases(new PurchasesUpdatedListener() {
//
//                     @Override
//                     public void onPurchasesUpdated(int responseCode, @Nullable List<Purchase> purchases) {
//
//                         Log.i(TAG, "updateInventory onPurchasesUpdated responseCode: " + responseCode +
//                                 ", purchases: " + purchases.size());
//
//                         // 요청 성공
//                         if( responseCode == BillingResponse.OK && purchases != null ) {
//                             for( Purchase purchase : purchases ) {
//                                 Item item = getItemById(purchase.getSku());
//
//                                 // 아이템 소모
//                                 if( item != null && item.consumable ) {
//                                     billingManager.consumeAsync(purchase.getPurchaseToken());
//                                 }
//                             }
//                         }
//
//                         if( listener != null ) {
//                             listener.onPurchasesUpdated(responseCode, purchases);
//                         }
//                     }
//                 });
//             }
//         });
//     }
//
//     /**
//      * 클라이언트 초기화 완료
//      */
//     @Override
//     public void onBillingClientSetupFinished() {
//
//         Log.i(TAG, "onBillingClientSetupFinished");
//
//         context.runOnGLThread(new Runnable() {
//             @Override
//             public void run() {
//                 nativeOnSetupFinished();
//             }
//         });
//
//         // 인벤토리 업데이트
//         updateInventory(null);
//
//         // consume for test
//         // consumeAll();
//     }
//
//     /**
//      * 아이템 소모 완료
//      */
//     @Override
//     public void onConsumeFinished(String token, @BillingResponse int responseCode) {
//
//         // Log.d(TAG, "Consumption finished. Purchase token: " + token + ", responseCode: " + responseCode);
//
//         // 소모 성공
//         if( responseCode == BillingResponse.OK )  {
//         }
//         // 소모 실패
//         else {
//         }
//     }
//
//     /**
//      * 아이템 결제 완료
//      */
//     @Override
//     public void onPurchasesUpdated(@BillingResponse final int responseCode, List<Purchase> purchases) {
//
//         Log.i(TAG, "onPurchasesUpdated responseCode: " + responseCode +
//                 ", has purchases: " + (purchases != null));
//
//         // 결제 성공
//         if( responseCode == BillingResponse.OK && purchases != null ) {
//             for( Purchase purchase : purchases ) {
//                 final Item item = getItemById(purchase.getSku());
//                 if( item == null ) {
//                     // TODO: 예외처리
//                     continue;
//                 }
//
//                 // 소모성 아이템, 구매 즉시 소모
//                 if( item.consumable ) {
//                     billingManager.consumeAsync(purchase.getPurchaseToken());
//                 }
//
//                 // 구매 완료 리스너 실행
//                 context.runOnGLThread(new Runnable() {
//                     @Override
//                     public void run() {
//                         nativeOnPurchased(item.itemId);
//                     }
//                 });
//
//                 // Log.i(TAG, "onPurchasesUpdated purchased: " + purchase);
//             }
//         }
//         // 결제 취소
//         else if( responseCode == BillingClient.BillingResponse.USER_CANCELED ) {
//             context.runOnGLThread(new Runnable() {
//                 @Override
//                 public void run() {
//                     nativeOnPurchaseCanceled();
//                 }
//             });
//         }
//         // 결제 실패
//         else {
//             context.runOnGLThread(new Runnable() {
//                 @Override
//                 public void run() {
//                     nativeOnPurchaseError("purchase error. [code : " + responseCode + "]");
//                 }
//             });
//         }
//
//         Log.i(TAG, "end onPurchasesUpdated flow.");
//     }
//
//     /**
//      * 아이템 결제
//      */
//     public static void purchase(final String itemId) {
//
//         Log.i(TAG, "purchase itemId: " + itemId);
//
//         final Item item = getItemById(itemId);
//
//         // 유효하지 않은 아이템
//         if( item == null ) {
//             getContext().runOnGLThread(new Runnable() {
//                 @Override
//                 public void run() {
//                     nativeOnPurchaseError("invalid item");
//                 }
//             });
//
//             return;
//         }
//
//         getContext().runOnUiThread(new Runnable() {
//             @Override
//             public void run() {
//                 // Step 1. 아이템 복원
//                 restore(new PurchasesUpdatedListener() {
//                     @Override
//                     public void onPurchasesUpdated(final int responseCode, @Nullable List<Purchase> purchases) {
//
//                         // 아이템 복원 요청 성공
//                         if( responseCode == BillingResponse.OK ) {
//                             // 결제 아이템이 복원된 아이템인지 체크
//                             Item restoreItem = findItem(purchases, itemId);
//
//                             // 복원된 아이템, 결제할 수 없음
//                             if( restoreItem != null ) {
//                                 getContext().runOnGLThread(new Runnable() {
//                                     @Override
//                                     public void run() {
//                                         nativeOnPurchaseError("already owned.");
//                                     }
//                                 });
//                             }
//                             // Step 2. 결제
//                             else {
//                                 Log.i(TAG, "start purchase: " + itemId);
//                                 getBillingManager().initiatePurchaseFlow(itemId, BillingClient.SkuType.INAPP);
//                             }
//                         }
//                         // 아이템 복원 요청 실패
//                         else {
//                             getContext().runOnGLThread(new Runnable() {
//                                 @Override
//                                 public void run() {
//                                     nativeOnPurchaseError("restore error. [code : " + responseCode + "]");
//                                 }
//                             });
//                         }
//                     }
//                 });
//             }
//         });
//     }
//
//     /**
//      * 아이템 복원
//      */
//     public static void restore(final PurchasesUpdatedListener listener) {
//
//         getInstance().updateInventory(new PurchasesUpdatedListener() {
//
//             @Override
//             public void onPurchasesUpdated(int responseCode, @Nullable List<Purchase> purchases) {
//
//                 final boolean result = (responseCode == BillingResponse.OK);
//
//                 if( result && purchases != null ) {
//                     for( final Purchase purchase : purchases ) {
//                         getContext().runOnGLThread(new Runnable() {
//                             @Override
//                             public void run() {
//                                 nativeOnRestored(purchase.getSku());
//                             }
//                         });
//                     }
//                 }
//
//                 getContext().runOnGLThread(new Runnable() {
//                     @Override
//                     public void run() {
//                         nativeOnRestoreFinished(result);
//                     }
//                 });
//
//                 if( listener != null ) {
//                     listener.onPurchasesUpdated(responseCode, purchases);
//                 }
//             }
//         });
//     }
//
//     public static void restore() {
//         restore(null);
//     }
//
//     public static void consumeAll() {
//
//         getInstance().updateInventory(new PurchasesUpdatedListener() {
//
//             @Override
//             public void onPurchasesUpdated(int responseCode, @Nullable List<Purchase> purchases) {
//
//                 final boolean result = (responseCode == BillingResponse.OK);
//
//                 if( result && purchases != null ) {
//                     for( final Purchase purchase : purchases ) {
//                         Log.i(TAG, "consumeAll item: " + purchase.getSku());
//                         getInstance().billingManager.consumeAsync(purchase.getPurchaseToken());
//                     }
//                 }
//             }
//         });
//     }
//
//     public static boolean isReady() {
//
//         BillingManager billingManager = getBillingManager();
//         if( billingManager == null ) {
//             return false;
//         }
//
//         return billingManager.isBillingClientReady() && billingManager.isServiceConnected();
//     }
//
//     public static native void nativeOnSetupFinished();
//     public static native void nativeOnPurchased(String itemId);
//     public static native void nativeOnPurchaseError(String errorMsg);
//     public static native void nativeOnPurchaseCanceled();
//     public static native void nativeOnRestored(String itemId);
//     public static native void nativeOnRestoreFinished(boolean result);
// }
