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
#include <Qt/QtCore>
#include <Qt/QtGui>

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

  virtual void bench(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Fog-Specific]
  // --------------------------------------------------------------------------

  void configurePainter(QPainter& p, const BenchParams& params);

  QBrush createLinearGradient(
    const Fog::PointF& pt0, const Fog::PointF& pt1,
    const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2);

  void runCreateDestroy(BenchOutput& output, const BenchParams& params);
  void runFillRectI(BenchOutput& output, const BenchParams& params);
  void runFillRectF(BenchOutput& output, const BenchParams& params);
  void runFillRectRotate(BenchOutput& output, const BenchParams& params);
  void runFillRound(BenchOutput& output, const BenchParams& params);
  void runFillPolygon(BenchOutput& output, const BenchParams& params);
  void runBlitImageI(BenchOutput& output, const BenchParams& params);
  void runBlitImageRotate(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  QImage* screenQt;
};

// [Guard]
#endif // _FOG_BENCHQT4_H
