# VP9 region-of-interest encoding

VP9 signals quantizer changes per 8x8 block through segmentation.  The encoder
exposes this through `VP9E_SET_ROI_MAP` and `vpx_roi_map_t`.  Applications can
provide either a segment map, a per-block quantizer offset map, or a binary ROI
mask.

## Inputs

All maps use one entry per 8x8 block in raster order.  Set `rows` to
`(frame_height + 7) / 8` and `cols` to `(frame_width + 7) / 8`.

- `roi_map`: segment id in `[0, 7]` for every block.  `delta_q[]` supplies the
  quantizer offset for each segment.  This is the original VP9 interface.
- `qp_offset_map`: signed quantizer offset in `[-63, 63]` for every block.
  Negative values improve quality.  At most 8 distinct offsets are supported.
- `roi_mask`: one byte per block, where a non-zero value selects the block as
  ROI.  `roi_qp_offset` is applied to selected blocks.

`qp_offset_map` and `roi_mask` take precedence over `roi_map` and `delta_q`.
If both are set, the mask offset is added to the per-block offset.  The final
offset is clamped to `[-63, 63]`.  Set `ref_frame[]` to `-1` when no reference
frame constraint is wanted.  For the per-block input forms, an all-zero
`ref_frame[]` array is treated as "not specified".

Example:

```c
vpx_roi_map_t roi = { 0 };
signed char qp_offset[rows * cols];

memset(qp_offset, 0, sizeof(qp_offset));
for (int row = 16 / 8; row < 48 / 8; ++row)
  for (int col = 16 / 8; col < 48 / 8; ++col)
    qp_offset[row * cols + col] = -20;

roi.rows = rows;
roi.cols = cols;
roi.qp_offset_map = qp_offset;
for (int i = 0; i < 8; ++i) roi.ref_frame[i] = -1;

if (vpx_codec_control(&encoder, VP9E_SET_ROI_MAP, &roi) != VPX_CODEC_OK) {
  /* Handle invalid dimensions, offsets, or too many distinct offsets. */
}
```

The encoder copies the map when the control is called, so the application may
release its buffers after the call returns.  Call the control again to update
the map for later frames.

## Rate control compatibility

ROI offsets are relative to the frame base quantizer chosen by the normal rate
controller.  They do not replace the frame target bitrate, frame quantizer,
minimum/maximum quantizer limits, or CBR/VBR/CQ/constant-Q policy.  Rate
control therefore remains responsible for the overall bitrate.  When an AQ
mode also uses segmentation, the ROI map takes precedence for the blocks it
covers.

ROI is applied in realtime and good-quality modes at every encoder speed.
Quantizer and loop-filter segment features are also applied on key frames.
Skip and reference-frame constraints are ignored on intra-only frames.

## Tests

```sh
./test_libvpx --gtest_filter='EncodeAPI.SetRoi*:Vp9RoiQpOffset.*'
```

`Vp9RoiQpOffset.RealTimeComparison` and
`Vp9RoiQpOffset.RealTimeLowSpeedComparison` and
`Vp9RoiQpOffset.GoodQualityComparison` encode the same source three times:
without ROI, with a binary ROI mask, and with a per-block QP offset map.  They
check that both ROI inputs improve PSNR inside the ROI and produce equivalent
results at realtime speeds 4 and 7 and at good-quality speed 0.
