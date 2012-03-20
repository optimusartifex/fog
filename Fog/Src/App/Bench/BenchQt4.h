// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BENCHQT4_H
#define _FOG_BENCHQT4_H

// [Dependencies]
#include "BenchApp.h"

// [Dependencies - Qt4]
#include <QtCore>
#include <QtGui>

// ============================================================================
// [BenchQt4]
// ============================================================================

struct BenchQt4 : public BenchModule
{
  BenchQt4(BenchApp& app);
  virtual ~BenchQt4();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual Fog::StringW getModuleName() const;
  virtual Fog::List<uint32_t> getSupportedPixelFormats() const;

  virtual void bench(BenchOutput& output, const BenchParams& params);

  virtual void prepareSprites(int size);
  virtual void freeSprites();

  // --------------------------------------------------------------------------
  // [Fog-Specific]
  // --------------------------------------------------------------------------

  void configurePainter(QPainter& p, const BenchParams& params);

  QBrush createLinearGradient(
    const Fog::PointF& pt0, const Fog::PointF& pt1,
    const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2);

  // --------------------------------------------------------------------------
  // [Run]
  // --------------------------------------------------------------------------

  virtual void runCreateDestroy(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectI(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectF(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectRotate(BenchOutput& output, const BenchParams& params);
  virtual void runFillRound(BenchOutput& output, const BenchParams& params);
  virtual void runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity);
  virtual void runBlitImageI(BenchOutput& output, const BenchParams& params);
  virtual void runBlitImageF(BenchOutput& output, const BenchParams& params);
  virtual void runBlitImageRotate(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  QImage* screenQt;
  Fog::List<QImage*> spritesQt;
};

// [Guard]
#endif // _FOG_BENCHQT4_H
