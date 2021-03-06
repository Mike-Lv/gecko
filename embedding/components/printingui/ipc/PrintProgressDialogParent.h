/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_embedding_PrintProgressDialogParent_h
#define mozilla_embedding_PrintProgressDialogParent_h

#include "mozilla/embedding/PPrintProgressDialogParent.h"
#include "nsIObserver.h"

class nsIPrintProgressParams;
class nsIWebProgressListener;

namespace mozilla {
namespace embedding {
class PrintProgressDialogParent MOZ_FINAL : public PPrintProgressDialogParent,
                                            public nsIObserver
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOBSERVER

  MOZ_IMPLICIT PrintProgressDialogParent();

  void SetWebProgressListener(nsIWebProgressListener* aListener);

  void SetPrintProgressParams(nsIPrintProgressParams* aParams);

  virtual bool
  RecvStateChange(
          const long& stateFlags,
          const nsresult& status);

  virtual bool
  RecvProgressChange(
          const long& curSelfProgress,
          const long& maxSelfProgress,
          const long& curTotalProgress,
          const long& maxTotalProgress);

  virtual bool
  RecvDocTitleChange(const nsString& newTitle);

  virtual bool
  RecvDocURLChange(const nsString& newURL);

  virtual void
  ActorDestroy(ActorDestroyReason aWhy);

  virtual bool
  Recv__delete__();

private:
  virtual ~PrintProgressDialogParent();

  nsCOMPtr<nsIWebProgressListener> mWebProgressListener;
  nsCOMPtr<nsIPrintProgressParams> mPrintProgressParams;
  bool mActive;
};

} // namespace embedding
} // namespace mozilla

#endif
