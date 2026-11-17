/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "gtest/gtest.h"

#include "test/codec_factory.h"
#include "test/encode_test_driver.h"
#include "test/i420_video_source.h"
#include "test/util.h"
#include "test/video_source.h"
#include "test/y4m_video_source.h"
#include "test/yuv_video_source.h"
#include "vpx/vp8cx.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_encoder.h"

#define MASK_WIDTH 40
#define MASK_HEIGHT 30
#define MASK_SIZE MASK_WIDTH *MASK_HEIGHT

#if CONFIG_VP9_DECODER
namespace {

const int mask[MASK_SIZE] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0
};

class RoiMaskBackgroundSkip : public ::libvpx_test::EncoderTest,
                              public ::testing::Test {
 protected:
  RoiMaskBackgroundSkip() : EncoderTest(&::libvpx_test::kVP9) {}
  ~RoiMaskBackgroundSkip() override { free(roi_.roi_map); }

  void SetUp() override {
    InitializeConfig();
    SetMode(::libvpx_test::kRealTime);
    SetRoi();
  }

  void SetRoi() {
    const int block_size = 8;
    unsigned int i, j;
    roi_.rows = (cfg_.g_h + block_size - 1) / block_size;
    roi_.cols = (cfg_.g_w + block_size - 1) / block_size;
    memset(&roi_.skip, 0, sizeof(roi_.skip));
    memset(&roi_.delta_q, 0, sizeof(roi_.delta_q));
    memset(&roi_.delta_lf, 0, sizeof(roi_.delta_lf));
    memset(roi_.ref_frame, -1, sizeof(roi_.ref_frame));
    roi_.ref_frame[1] = 1;
    // Use segment 3 for skip.
    roi_.skip[3] = 1;
    roi_.roi_map =
        (uint8_t *)calloc(roi_.rows * roi_.cols, sizeof(*roi_.roi_map));
    for (i = 0; i < roi_.rows; ++i) {
      for (j = 0; j < roi_.cols; ++j) {
        const int idx = i * roi_.cols + j;
        if (mask[idx] == 1) roi_.roi_map[idx] = 3;
      }
    }
  }

  void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
                          ::libvpx_test::Encoder *encoder) override {
    if (video->frame() == 0) {
      encoder->Control(VP8E_SET_CPUUSED, 7);
      encoder->Control(VP9E_SET_AQ_MODE, 3);
    }
    encoder->Control(VP9E_SET_ROI_MAP, &roi_);
  }

 private:
  vpx_roi_map_t roi_;
};

TEST_F(RoiMaskBackgroundSkip, RoiMaskNoMismatch) {
  cfg_.rc_buf_initial_sz = 500;
  cfg_.rc_buf_optimal_sz = 500;
  cfg_.rc_buf_sz = 1000;
  cfg_.rc_undershoot_pct = 20;
  cfg_.rc_undershoot_pct = 20;
  cfg_.rc_dropframe_thresh = 10;
  cfg_.rc_min_quantizer = 0;
  cfg_.rc_max_quantizer = 50;
  cfg_.rc_end_usage = VPX_CBR;
  cfg_.rc_target_bitrate = 200;
  cfg_.g_lag_in_frames = 0;
  cfg_.kf_max_dist = 9999;

  ::libvpx_test::I420VideoSource video("desktopqvga.320_240.yuv", 320, 240, 30,
                                       1, 0, 150);
  ASSERT_NO_FATAL_FAILURE(RunLoop(&video));
}

namespace {

constexpr int kRoiWidth = 96;
constexpr int kRoiHeight = 96;
constexpr int kRoiBlockSize = 8;
constexpr int kRoiFrames = 6;
constexpr int kRoiX0 = 32;
constexpr int kRoiY0 = 32;
constexpr int kRoiX1 = 64;
constexpr int kRoiY1 = 64;

enum RoiInputType {
  kNoRoi,
  kRoiMask,
  kRoiQpOffsetMap,
};

uint8_t RoiTexture(int x, int y, int frame) {
  uint32_t value = static_cast<uint32_t>(x) * 73856093u ^
                   static_cast<uint32_t>(y) * 19349663u ^
                   static_cast<uint32_t>(frame) * 83492791u;
  value ^= value >> 13;
  value *= 0x5bd1e995u;
  value ^= value >> 15;
  return static_cast<uint8_t>(value);
}

void FillRoiSource(vpx_image_t *img, int frame) {
  for (int plane = 0; plane < 3; ++plane) {
    const int width = plane == 0 ? kRoiWidth : kRoiWidth / 2;
    const int height = plane == 0 ? kRoiHeight : kRoiHeight / 2;
    uint8_t *row = img->planes[plane];
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        if (plane != 0) {
          row[x] = 128;
          continue;
        }
        const bool in_roi = x >= kRoiX0 && x < kRoiX1 && y >= kRoiY0 &&
                            y < kRoiY1;
        row[x] = in_roi ? RoiTexture(x, y, frame) : 128;
      }
      row += img->stride[plane];
    }
  }
}

double RoiPsnr(const vpx_image_t *source, const vpx_image_t *decoded) {
  uint64_t sse = 0;
  for (int y = kRoiY0; y < kRoiY1; ++y) {
    const uint8_t *source_row = source->planes[VPX_PLANE_Y] +
                                y * source->stride[VPX_PLANE_Y] + kRoiX0;
    const uint8_t *decoded_row = decoded->planes[VPX_PLANE_Y] +
                                 y * decoded->stride[VPX_PLANE_Y] + kRoiX0;
    for (int x = kRoiX0; x < kRoiX1; ++x) {
      const int diff = source_row[x - kRoiX0] - decoded_row[x - kRoiX0];
      sse += diff * diff;
    }
  }
  if (sse == 0) return 99.0;
  const double samples = (kRoiX1 - kRoiX0) * (kRoiY1 - kRoiY0);
  return 10.0 * log10(255.0 * 255.0 * samples / sse);
}

double EncodeRoiAndMeasurePsnr(RoiInputType input_type, bool good_quality,
                               int speed) {
  vpx_codec_ctx_t encoder;
  vpx_codec_ctx_t decoder;
  vpx_codec_enc_cfg_t cfg;
  vpx_codec_dec_cfg_t dec_cfg = {};
  vpx_image_t source;
  double total_psnr = 0.0;
  int decoded_frames = 0;

  EXPECT_EQ(vpx_codec_enc_config_default(vpx_codec_vp9_cx(), &cfg, 0),
            VPX_CODEC_OK);
  cfg.g_w = kRoiWidth;
  cfg.g_h = kRoiHeight;
  cfg.g_lag_in_frames = 0;
  cfg.g_pass = VPX_RC_ONE_PASS;
  cfg.g_threads = 1;
  cfg.rc_end_usage = VPX_CQ;
  cfg.rc_min_quantizer = 40;
  cfg.rc_max_quantizer = 40;
  cfg.rc_target_bitrate = 5000;
  dec_cfg.threads = 1;
  dec_cfg.w = kRoiWidth;
  dec_cfg.h = kRoiHeight;

  if (vpx_codec_enc_init(&encoder, vpx_codec_vp9_cx(), &cfg, 0) !=
      VPX_CODEC_OK) {
    ADD_FAILURE() << "Failed to initialize encoder";
    return 0.0;
  }
  EXPECT_EQ(vpx_codec_control(&encoder, VP8E_SET_CQ_LEVEL, 40), VPX_CODEC_OK);
  EXPECT_EQ(vpx_codec_control(&encoder, VP8E_SET_CPUUSED, speed),
            VPX_CODEC_OK);
  if (vpx_codec_dec_init(&decoder, vpx_codec_vp9_dx(), &dec_cfg, 0) !=
      VPX_CODEC_OK) {
    ADD_FAILURE() << "Failed to initialize decoder";
    EXPECT_EQ(vpx_codec_destroy(&encoder), VPX_CODEC_OK);
    return 0.0;
  }
  if (vpx_img_alloc(&source, VPX_IMG_FMT_I420, kRoiWidth, kRoiHeight, 32) ==
      nullptr) {
    ADD_FAILURE() << "Failed to allocate source image";
    EXPECT_EQ(vpx_codec_destroy(&decoder), VPX_CODEC_OK);
    EXPECT_EQ(vpx_codec_destroy(&encoder), VPX_CODEC_OK);
    return 0.0;
  }

  vpx_roi_map_t roi = {};
  uint8_t roi_mask[(kRoiHeight / kRoiBlockSize) *
                   (kRoiWidth / kRoiBlockSize)];
  signed char qp_offset_map[(kRoiHeight / kRoiBlockSize) *
                            (kRoiWidth / kRoiBlockSize)];
  const int rows = kRoiHeight / kRoiBlockSize;
  const int cols = kRoiWidth / kRoiBlockSize;

  memset(roi_mask, 0, sizeof(roi_mask));
  memset(qp_offset_map, 0, sizeof(qp_offset_map));
  for (int row = kRoiY0 / kRoiBlockSize; row < kRoiY1 / kRoiBlockSize;
       ++row) {
    for (int col = kRoiX0 / kRoiBlockSize; col < kRoiX1 / kRoiBlockSize;
         ++col) {
      roi_mask[row * cols + col] = 1;
      qp_offset_map[row * cols + col] = -30;
    }
  }

  for (int i = 0; i < 8; ++i) roi.ref_frame[i] = -1;
  roi.rows = rows;
  roi.cols = cols;
  if (input_type == kRoiMask) {
    roi.roi_mask = roi_mask;
    roi.roi_qp_offset = -30;
  } else if (input_type == kRoiQpOffsetMap) {
    roi.qp_offset_map = qp_offset_map;
  }

  if (input_type != kNoRoi) {
    EXPECT_EQ(vpx_codec_control(&encoder, VP9E_SET_ROI_MAP, &roi),
              VPX_CODEC_OK);
  }

  for (int frame = 0; frame < kRoiFrames; ++frame) {
    FillRoiSource(&source, frame);
    EXPECT_EQ(vpx_codec_encode(&encoder, &source, frame, 1, 0,
                               good_quality ? VPX_DL_GOOD_QUALITY
                                            : VPX_DL_REALTIME),
              VPX_CODEC_OK);

    vpx_codec_iter_t enc_iter = nullptr;
    while (const vpx_codec_cx_pkt_t *pkt =
               vpx_codec_get_cx_data(&encoder, &enc_iter)) {
      if (pkt->kind != VPX_CODEC_CX_FRAME_PKT) continue;
      EXPECT_EQ(vpx_codec_decode(
                    &decoder,
                    static_cast<const uint8_t *>(pkt->data.frame.buf),
                    static_cast<unsigned int>(pkt->data.frame.sz), nullptr, 0),
                VPX_CODEC_OK);
      vpx_codec_iter_t dec_iter = nullptr;
      vpx_image_t *decoded = vpx_codec_get_frame(&decoder, &dec_iter);
      if (decoded == nullptr) {
        ADD_FAILURE() << "Decoder did not return a frame";
        continue;
      }
      total_psnr += RoiPsnr(&source, decoded);
      ++decoded_frames;
    }
  }

  vpx_img_free(&source);
  EXPECT_EQ(vpx_codec_destroy(&decoder), VPX_CODEC_OK);
  EXPECT_EQ(vpx_codec_destroy(&encoder), VPX_CODEC_OK);
  return decoded_frames > 0 ? total_psnr / decoded_frames : 0.0;
}

void TestRoiQpOffsetComparison(bool good_quality, int speed) {
  const double base_psnr =
      EncodeRoiAndMeasurePsnr(kNoRoi, good_quality, speed);
  const double mask_psnr =
      EncodeRoiAndMeasurePsnr(kRoiMask, good_quality, speed);
  const double map_psnr =
      EncodeRoiAndMeasurePsnr(kRoiQpOffsetMap, good_quality, speed);

  EXPECT_GT(mask_psnr, base_psnr + 3.0)
      << "base: " << base_psnr << " mask: " << mask_psnr;
  EXPECT_GT(map_psnr, base_psnr + 3.0)
      << "base: " << base_psnr << " qp map: " << map_psnr;
  EXPECT_NEAR(mask_psnr, map_psnr, 1.0);
}

}  // namespace

TEST(Vp9RoiQpOffset, RealTimeComparison) {
  TestRoiQpOffsetComparison(/*good_quality=*/false, /*speed=*/7);
}

TEST(Vp9RoiQpOffset, RealTimeLowSpeedComparison) {
  TestRoiQpOffsetComparison(/*good_quality=*/false, /*speed=*/4);
}

TEST(Vp9RoiQpOffset, GoodQualityComparison) {
  TestRoiQpOffsetComparison(/*good_quality=*/true, /*speed=*/0);
}

}  // namespace
#endif  // CONFIG_VP9_DECODER
