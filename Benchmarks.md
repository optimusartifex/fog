# Introduction #

This page contains output from FogBench tool, which was designed mainly to improve Fog-Framework; other libraries were added simply for comparison. All tests are very synthetic which is often called as micro-benchmarks.

Benchmarks are designed to check speed of:

  * **Fog** (st/mt) - These two benchmarks should reveal differences between single-threaded and multi-threaded renderer.
  * **Cairo** - Only image benchmark (painting to image surface).
  * **GDI+** - Only image benchmark (painting to Gdiplus::Bitmap).
  * **Qt4** - Only image benchmark (painting to QImage).
  * **Quartz** - Only image benchmark (painting to CGImageRef).

Platform specific engines are always run on that platform, cross-platform engines are usually run only where the library is dominant (for example Cairo is often run only on Linux).

If configured, FogBench is able to store generated images for comparison. It's very important to check whether the output of all tested engines is the same to be sure that benchmarks were fair. There are, however, some imperfections which can't be fixed:

  * **GDI+** - All benchmarks with 'src' operator contains wrong pixels at the shape borders. It's definitely an GDI+ issue which is there from the beginning. I have no idea how to configure the Gdip::Graphics to render shapes/images correctly when 'src' operator is set. I think that this is not a performance issue, just bug in GDI+.
  * Others - To be added

# Test Descriptions #

Each test should reveal performance of a specific fast-path or generic code. Graphics libraries always contain fast-paths used only in particular case. The reason is that optimized function for a specific operation (for example filling aligned rectangle) could be many times faster than general case.

Each test contains its name, source-type, and compositing opearator.

List of tests:

  * **Create/Destroy** - Time needed to create and destroy painting context. This test comparing only this, destination surface is always cached for this operation. This test is not so important, but it's good for comparison.
  * **FillRectI** - Fill rectangle (aligned to pixels).
  * **FillRectF** - Fill rectangle (not aligned).
  * **FillRectR** - Fill rectangle (rotated).
  * **FillRound** - Fill rounded rectangle (not aligned to pixels).
  * **FillPolygon** - Fill polygon which consists of 10 vertices.
  * **FillComplex** - Fill polygon which consists of 100 vertices.
  * **BlitImageI** - Blit image (aligned to pixels and not scaled).
  * **BlitImageF** - Blit image (not aligned, but not scaled).
  * **BlitImageR** - Blit image (rotated).

List of source-types:

  * **Solid** - Solid color is used as a source.
  * **LinGr** - Linear gradient of 3 color stops is used as a source.

List of operators:

  * **Copy** - Source operator.
  * **Over** - Source-Over operator.

The tests are usually configured to do 10.000 operations and result is time in [ms](ms.md) for each. Size of destination image is usually 640x480. FogBench allows to test more pixel formats, so if there are more pixel formats supported by the engine, they are usually tested.

The (mt) comment in Fog tests means that multi-threading was enabled - number of threads is always determined as number of CPUs/cores.

There are more tests planned in the future. Current test-suite was mainly used when improving Fog-Framework pipeline, comparison is just bonus to get information how good are the other tested engines.

# Results #

Results of running FogBench tool on various hardware and platforms. We show only PRGB32 pixel format here, because it's usually the most optimized and used pixel format.

**TODO**

**This section will be updated soon. The old benchmarks published here were related to older version of Fog and outdated. It will be unfair to keep them here**.

# Discussion #

You can see that Fog-Framework is really fast compared to others, but can be even faster in future. There are various fast-paths hit in the benchmark (FillRectI/FillRectF and BlitImageI/BlitImageF). The performance of painting aligned rectangles to a pixel grid is near the borders. The performance of path rasterization and pattern rendering could be improved.

**TODO**

There is also evidence that multi-threading helps mainly with painting larger objects. Most tests where size of object is small (about 16x16 pixels and less) were faster in single-threaded mode (here the interesting thing is that under Windows the difference is much lower that under Linux). There is currently no way how to detect small shape and paint small objects single-threaded - multi-threading is asynchronous and cost of synchronization may be more expensive than serialization.

# Conclusion #

The Fog-Framework is currently one of the fastest libraries around while keeping hiqh-quality rendering. This is caused by the modern library architecture and optimization techniques designed to accelerate shape transformations, rasterization, and pixel compositing.

The only disadvantage of Fog-Framework is that it's still an experimental project and not each part of the library is complete.