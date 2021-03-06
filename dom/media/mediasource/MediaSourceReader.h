/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_MEDIASOURCEREADER_H_
#define MOZILLA_MEDIASOURCEREADER_H_

#include "mozilla/Attributes.h"
#include "mozilla/ReentrantMonitor.h"
#include "nsCOMPtr.h"
#include "nsError.h"
#include "nsString.h"
#include "nsTArray.h"
#include "MediaDecoderReader.h"

namespace mozilla {

class MediaSourceDecoder;
class SourceBufferDecoder;
class TrackBuffer;

namespace dom {

class MediaSource;

} // namespace dom

class MediaSourceReader : public MediaDecoderReader
{
public:
  explicit MediaSourceReader(MediaSourceDecoder* aDecoder);

  nsresult Init(MediaDecoderReader* aCloneDonor) MOZ_OVERRIDE
  {
    // Although we technically don't implement anything here, we return NS_OK
    // so that when the state machine initializes and calls this function
    // we don't return an error code back to the media element.
    return NS_OK;
  }

  // Indicates the point in time at which the reader should consider
  // registered TrackBuffers essential for initialization.
  void PrepareInitialization();

  bool IsWaitingMediaResources() MOZ_OVERRIDE;

  nsRefPtr<AudioDataPromise> RequestAudioData() MOZ_OVERRIDE;
  nsRefPtr<VideoDataPromise>
  RequestVideoData(bool aSkipToNextKeyframe, int64_t aTimeThreshold) MOZ_OVERRIDE;

  virtual size_t SizeOfVideoQueueInFrames() MOZ_OVERRIDE;
  virtual size_t SizeOfAudioQueueInFrames() MOZ_OVERRIDE;

  virtual bool IsDormantNeeded() MOZ_OVERRIDE;
  virtual void ReleaseMediaResources() MOZ_OVERRIDE;

  void OnAudioDecoded(AudioData* aSample);
  void OnAudioNotDecoded(NotDecodedReason aReason);
  void OnVideoDecoded(VideoData* aSample);
  void OnVideoNotDecoded(NotDecodedReason aReason);

  void DoVideoSeek();
  void DoAudioSeek();
  void OnVideoSeekCompleted(int64_t aTime);
  void OnVideoSeekFailed(nsresult aResult);
  void OnAudioSeekCompleted(int64_t aTime);
  void OnAudioSeekFailed(nsresult aResult);

  virtual bool IsWaitForDataSupported() MOZ_OVERRIDE { return true; }
  virtual nsRefPtr<WaitForDataPromise> WaitForData(MediaData::Type aType) MOZ_OVERRIDE;
  void MaybeNotifyHaveData();

  bool HasVideo() MOZ_OVERRIDE
  {
    return mInfo.HasVideo();
  }

  bool HasAudio() MOZ_OVERRIDE
  {
    return mInfo.HasAudio();
  }

  void NotifyTimeRangesChanged();

  virtual void DisableHardwareAcceleration() MOZ_OVERRIDE {
    if (GetVideoReader()) {
      GetVideoReader()->DisableHardwareAcceleration();
    }
  }

  // We can't compute a proper start time since we won't necessarily
  // have the first frame of the resource available. This does the same
  // as chrome/blink and assumes that we always start at t=0.
  virtual int64_t ComputeStartTime(const VideoData* aVideo, const AudioData* aAudio) MOZ_OVERRIDE { return 0; }

  // Buffering heuristics don't make sense for MSE, because the arrival of data
  // is at least partly controlled by javascript, and javascript does not expect
  // us to sit on unplayed data just because it may not be enough to play
  // through.
  bool UseBufferingHeuristics() MOZ_OVERRIDE { return false; }

  bool IsMediaSeekable() MOZ_OVERRIDE { return true; }

  nsresult ReadMetadata(MediaInfo* aInfo, MetadataTags** aTags) MOZ_OVERRIDE;
  void ReadUpdatedMetadata(MediaInfo* aInfo) MOZ_OVERRIDE;
  nsRefPtr<SeekPromise>
  Seek(int64_t aTime, int64_t aEndTime) MOZ_OVERRIDE;

  void CancelSeek() MOZ_OVERRIDE;

  // Acquires the decoder monitor, and is thus callable on any thread.
  nsresult GetBuffered(dom::TimeRanges* aBuffered) MOZ_OVERRIDE;

  already_AddRefed<SourceBufferDecoder> CreateSubDecoder(const nsACString& aType,
                                                         int64_t aTimestampOffset /* microseconds */);

  void AddTrackBuffer(TrackBuffer* aTrackBuffer);
  void RemoveTrackBuffer(TrackBuffer* aTrackBuffer);
  void OnTrackBufferConfigured(TrackBuffer* aTrackBuffer, const MediaInfo& aInfo);

  nsRefPtr<ShutdownPromise> Shutdown() MOZ_OVERRIDE;

  virtual void BreakCycles() MOZ_OVERRIDE;

  bool IsShutdown()
  {
    ReentrantMonitorAutoEnter decoderMon(mDecoder->GetReentrantMonitor());
    return mDecoder->IsShutdown();
  }

  // Return true if all of the active tracks contain data for the specified time.
  bool TrackBuffersContainTime(int64_t aTime);

  // Mark the reader to indicate that EndOfStream has been called on our MediaSource
  void Ended();

  // Return true if the Ended method has been called
  bool IsEnded();
  bool IsNearEnd(int64_t aTime /* microseconds */);

  // Set the duration of the attached mediasource element.
  void SetMediaSourceDuration(double aDuration /* seconds */);

#ifdef MOZ_EME
  nsresult SetCDMProxy(CDMProxy* aProxy);
#endif

  virtual bool IsAsync() const MOZ_OVERRIDE {
    return (!GetAudioReader() || GetAudioReader()->IsAsync()) &&
           (!GetVideoReader() || GetVideoReader()->IsAsync());
  }

  // Returns true if aReader is a currently active audio or video
  bool IsActiveReader(MediaDecoderReader* aReader);

  // Returns a string describing the state of the MediaSource internal
  // buffered data. Used for debugging purposes.
  void GetMozDebugReaderData(nsAString& aString);

private:
  // Switch the current audio/video source to the source that
  // contains aTarget (or up to aTolerance after target). Both
  // aTarget and aTolerance are in microseconds.
  // Search can be made using a fuzz factor. Should an approximated value be
  // found instead, aTarget will be updated to the actual target found.
  enum SwitchSourceResult {
    SOURCE_NONE = -1,
    SOURCE_EXISTING = 0,
    SOURCE_NEW = 1,
  };

  SwitchSourceResult SwitchAudioSource(int64_t* aTarget);
  SwitchSourceResult SwitchVideoSource(int64_t* aTarget);

  void DoAudioRequest();
  void DoVideoRequest();

  void CompleteAudioSeekAndDoRequest()
  {
    mAudioSeekRequest.Complete();
    DoAudioRequest();
  }

  void CompleteVideoSeekAndDoRequest()
  {
    mVideoSeekRequest.Complete();
    DoVideoRequest();
  }

  void CompleteAudioSeekAndRejectPromise()
  {
    mAudioSeekRequest.Complete();
    mAudioPromise.Reject(DECODE_ERROR, __func__);
  }

  void CompleteVideoSeekAndRejectPromise()
  {
    mVideoSeekRequest.Complete();
    mVideoPromise.Reject(DECODE_ERROR, __func__);
  }

  MediaDecoderReader* GetAudioReader() const;
  MediaDecoderReader* GetVideoReader() const;
  int64_t GetReaderAudioTime(int64_t aTime) const;
  int64_t GetReaderVideoTime(int64_t aTime) const;

  // Will reject the MediaPromise with END_OF_STREAM if mediasource has ended
  // or with WAIT_FOR_DATA otherwise.
  void CheckForWaitOrEndOfStream(MediaData::Type aType, int64_t aTime /* microseconds */);

  // Return a decoder from the set available in aTrackDecoders that has data
  // available in the range requested by aTarget.
  already_AddRefed<SourceBufferDecoder> SelectDecoder(int64_t aTarget /* microseconds */,
                                                      int64_t aTolerance /* microseconds */,
                                                      const nsTArray<nsRefPtr<SourceBufferDecoder>>& aTrackDecoders);
  bool HaveData(int64_t aTarget, MediaData::Type aType);

  void AttemptSeek();
  bool IsSeeking() { return mPendingSeekTime != -1; }

  nsRefPtr<SourceBufferDecoder> mAudioSourceDecoder;
  nsRefPtr<SourceBufferDecoder> mVideoSourceDecoder;

  nsTArray<nsRefPtr<TrackBuffer>> mTrackBuffers;
  nsTArray<nsRefPtr<TrackBuffer>> mShutdownTrackBuffers;
  nsTArray<nsRefPtr<TrackBuffer>> mEssentialTrackBuffers;
  nsRefPtr<TrackBuffer> mAudioTrack;
  nsRefPtr<TrackBuffer> mVideoTrack;

  MediaPromiseConsumerHolder<AudioDataPromise> mAudioRequest;
  MediaPromiseConsumerHolder<VideoDataPromise> mVideoRequest;

  MediaPromiseHolder<AudioDataPromise> mAudioPromise;
  MediaPromiseHolder<VideoDataPromise> mVideoPromise;

  MediaPromiseHolder<WaitForDataPromise> mAudioWaitPromise;
  MediaPromiseHolder<WaitForDataPromise> mVideoWaitPromise;
  MediaPromiseHolder<WaitForDataPromise>& WaitPromise(MediaData::Type aType)
  {
    return aType == MediaData::AUDIO_DATA ? mAudioWaitPromise : mVideoWaitPromise;
  }

#ifdef MOZ_EME
  nsRefPtr<CDMProxy> mCDMProxy;
#endif

  // These are read and written on the decode task queue threads.
  int64_t mLastAudioTime;
  int64_t mLastVideoTime;

  MediaPromiseConsumerHolder<SeekPromise> mAudioSeekRequest;
  MediaPromiseConsumerHolder<SeekPromise> mVideoSeekRequest;
  MediaPromiseHolder<SeekPromise> mSeekPromise;

  // Temporary seek information while we wait for the data
  // to be added to the track buffer.
  int64_t mPendingSeekTime;
  bool mWaitingForSeekData;

  int64_t mTimeThreshold;
  bool mDropAudioBeforeThreshold;
  bool mDropVideoBeforeThreshold;

  bool mAudioDiscontinuity;
  bool mVideoDiscontinuity;

  bool mEnded;
  double mMediaSourceDuration;

  bool mHasEssentialTrackBuffers;

  void ContinueShutdown();
  MediaPromiseHolder<ShutdownPromise> mMediaSourceShutdownPromise;
#ifdef MOZ_FMP4
  nsRefPtr<SharedDecoderManager> mSharedDecoderManager;
#endif
};

} // namespace mozilla

#endif /* MOZILLA_MEDIASOURCEREADER_H_ */
