# -*- indent-tabs-mode: nil; js-indent-level: 2 -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

XPCOMUtils.defineLazyGetter(this, "gEMEBundle", function() {
  return document.getElementById("bundle_eme");
});

let gEMEHandler = {
  ensureEMEEnabled: function(browser, keySystem) {
    Services.prefs.setBoolPref("media.eme.enabled", true);
    if (keySystem) {
      if (keySystem.startsWith("com.adobe") &&
          Services.prefs.getPrefType("media.gmp-eme-adobe.enabled") &&
          !Services.prefs.getBoolPref("media.gmp-eme-adobe.enabled")) {
        Services.prefs.setBoolPref("media.gmp-eme-adobe.enabled", true);
      } else if (keySystem == "org.w3.clearkey" &&
                 Services.prefs.getPrefType("media.eme.clearkey.enabled") &&
                 !Services.prefs.getBoolPref("media.eme.clearkey.enabled")) {
        Services.prefs.setBoolPref("media.eme.clearkey.enabled", true);
      }
    }
    browser.reload();
  },
  getLearnMoreLink: function(msgId) {
    let text = gEMEBundle.getString("emeNotifications." + msgId + ".learnMoreLabel");
    let baseURL = Services.urlFormatter.formatURLPref("app.support.baseURL");
    return "<label class='text-link' href='" + baseURL + "drm-content'>" +
           text + "</label>";
  },
  onDontAskAgain: function(menuPopupItem) {
    let button = menuPopupItem.parentNode.anchorNode;
    let bar = button.parentNode;
    Services.prefs.setBoolPref("browser.eme.ui." + bar.value + ".disabled", true);
    bar.close();
  },
  onNotNow: function(menuPopupItem) {
    let button = menuPopupItem.parentNode.anchorNode;
    button.parentNode.close();
  },
  receiveMessage: function({target: browser, data: data}) {
    let parsedData;
    try {
      parsedData = JSON.parse(data);
    } catch (ex) {
      Cu.reportError("Malformed EME video message with data: " + data);
      return;
    }
    let {status: status, keySystem: keySystem} = parsedData;
    // Don't need to show if disabled
    if (!Services.prefs.getBoolPref("browser.eme.ui.enabled")) {
      return;
    }

    let notificationId;
    let buttonCallback;
    let params = [];
    switch (status) {
      case "available":
      case "cdm-created":
        this.showPopupNotificationForSuccess(browser, keySystem);
        // ... and bail!
        return;

      case "api-disabled":
      case "cdm-disabled":
        notificationId = "drmContentDisabled";
        buttonCallback = gEMEHandler.ensureEMEEnabled.bind(gEMEHandler, browser, keySystem)
        params = [this.getLearnMoreLink(notificationId)];
        break;

      case "cdm-not-supported":
        notificationId = "drmContentCDMNotSupported";
        params = [this._brandShortName, this.getLearnMoreLink(notificationId)];
        break;

      case "cdm-insufficient-version":
        notificationId = "drmContentCDMInsufficientVersion";
        params = [this._brandShortName];
        break;

      case "cdm-not-installed":
        notificationId = "drmContentCDMInstalling";
        params = [this._brandShortName];
        break;

      case "error":
        // Fall through and do the same for unknown messages:
      default:
        let typeOfIssue = status == "error" ? "error" : "message ('" + status + "')";
        Cu.reportError("Unknown " + typeOfIssue + " dealing with EME key request: " + data);
        return;
    }

    this.showNotificationBar(browser, notificationId, keySystem, params, buttonCallback);
  },
  showNotificationBar: function(browser, notificationId, keySystem, labelParams, callback) {
    let box = gBrowser.getNotificationBox(browser);
    if (box.getNotificationWithValue(notificationId)) {
      return;
    }

    // If the user turned these off, bail out:
    try {
      if (Services.prefs.getBoolPref("browser.eme.ui." + notificationId + ".disabled")) {
        return;
      }
    } catch (ex) { /* Don't care if the pref doesn't exist */ }

    let msgPrefix = "emeNotifications." + notificationId + ".";
    let msgId = msgPrefix + "message";

    // Specialcase Adobe's CDM on unsupported platforms to be more informative:
    if (notificationId == "drmContentCDMNotSupported" &&
        keySystem.startsWith("com.adobe")) {
      let os = Services.appinfo.OS.toLowerCase();
      if (os.startsWith("win") && Services.appinfo.XPCOMABI.startsWith("x86_64")) {
        msgId = msgPrefix + "64bit.message";
      } else if (os.startsWith("linux") || os.startsWith("darwin")) {
        msgId = msgPrefix + "unsupportedOS.message";
        labelParams.splice(1, 0, os.startsWith("linux") ? "Linux" : "Mac OS X");
      }
    }

    let message = labelParams.length ?
                  gEMEBundle.getFormattedString(msgId, labelParams) :
                  gEMEBundle.getString(msgId);

    let buttons = [];
    if (callback) {
      let btnLabelId = msgPrefix + "button.label";
      let btnAccessKeyId = msgPrefix + "button.accesskey";
      buttons.push({
        label: gEMEBundle.getString(btnLabelId),
        accessKey: gEMEBundle.getString(btnAccessKeyId),
        callback: callback
      });

      let optionsId = "emeNotifications.optionsButton";
      buttons.push({
        label: gEMEBundle.getString(optionsId + ".label"),
        accessKey: gEMEBundle.getString(optionsId + ".accesskey"),
        popup: "emeNotificationsPopup"
      });
    }

    let iconURL = "chrome://browser/skin/drm-icon.svg#chains-black";

    // Do a little dance to get rich content into the notification:
    let fragment = document.createDocumentFragment();
    let descriptionContainer = document.createElement("description");
    descriptionContainer.innerHTML = message;
    while (descriptionContainer.childNodes.length) {
      fragment.appendChild(descriptionContainer.childNodes[0]);
    }

    box.appendNotification(fragment, notificationId, iconURL, box.PRIORITY_WARNING_MEDIUM,
                           buttons);
  },
  showPopupNotificationForSuccess: function(browser, keySystem) {
    // Don't bother creating it if it's already there:
    if (PopupNotifications.getNotification("drmContentPlaying", browser)) {
      return;
    }

    let msgPrefix = "emeNotifications.drmContentPlaying.";
    let msgId = msgPrefix + "message2";
    let btnLabelId = msgPrefix + "button.label";
    let btnAccessKeyId = msgPrefix + "button.accesskey";

    let message = gEMEBundle.getFormattedString(msgId, [this._brandShortName]);
    let anchorId = "eme-notification-icon";

    let mainAction = {
      label: gEMEBundle.getString(btnLabelId),
      accessKey: gEMEBundle.getString(btnAccessKeyId),
      callback: function() { openPreferences("paneContent"); },
      dismiss: true
    };
    let options = {
      dismissed: true,
      eventCallback: aTopic => aTopic == "swapping",
    };
    PopupNotifications.show(browser, "drmContentPlaying", message, anchorId, mainAction, null, options);
  },
  QueryInterface: XPCOMUtils.generateQI([Ci.nsIMessageListener])
};

XPCOMUtils.defineLazyGetter(gEMEHandler, "_brandShortName", function() {
  return document.getElementById("bundle_brand").getString("brandShortName");
});

window.messageManager.addMessageListener("EMEVideo:ContentMediaKeysRequest", gEMEHandler);
window.addEventListener("unload", function() {
  window.messageManager.removeMessageListener("EMEVideo:ContentMediaKeysRequest", gEMEHandler);
}, false);
