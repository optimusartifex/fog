# Introduction #

Compositing is raster based image manipulation. In 2d pipeline it's usually the last step invoked when painting graphics. There are two pixels (Destination and Source) which are composited to the resulting pixel (Result). The compositing operator tells how the pixels are be combined and the mask tells how much the composition takes effect (when replacing the destination).

> <b><code>Dca</code></b> - Destination color-component (premultiplied).
> <b><code>Da</code></b> - Destination alpha-component.

> <b><code>Sca</code></b> - Source color-component (premultiplied).
> <b><code>Sa</code></b> - Source alpha-component.

> <b><code>Mca</code></b> - Mask color-component (only for LCD compositing).
> <b><code>Ma</code></b> - Mask alpha-component.

> <b><code>Dca'</code></b> - Result color-component (premultiplied).
> <b><code>Da'</code></b> - Result alpha-component.

```
  Dca' = CompositeC(Dca, Da, Sca, Sa)
  Da'  = CompositeA(Dca, Da, Sca, Sa)

  Msk (A):

  Dca' = CompositeC(Dca, Da, Sca, Sa).Ma + Dca.(1 - Ma)
  Da'  = CompositeA(Dca, Da, Sca, Sa).Ma + Dca.(1 - Ma)

  Msk (ARGB):

  Dca' = CompositeC(Dca, Da, Sca, Sa).Mc + Dca.(1 - Mc)
  Da'  = CompositeA(Dca, Da, Sca, Sa).Ma + Dca.(1 - Ma)
```

# Specifications #

  * [SVG Compositing Primer](http://dev.w3.org/SVG/modules/compositing/master/SVGCompositingPrimer.html)

# Operators #

## SRC ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SRC):
  Dca' = Sca.Da + Sca.(1 - Da) = Sca
  Da'  = Sa .Da + Sa .(1 - Da) = Sa

  Msk:

  Dca' = Sca.m + Dca.(1 - m)
  Da'  = Sa .m + Da .(1 - m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC):
  Dca' = Sc
  Da'  = 1

  Msk:

  Dca' = Sc.m + Dca.(1 - m)
  Da'  = 1 .m + Da .(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (SRC):
  Dc'  = Sca

  Msk:

  Dc'  = Sca.m + Dc.(1 - m)

Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  Dc'  = Sc

  Msk:

  Dc'  = Sc.m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC):
  Da'  = Sa

  Msk:

  Da'  = Sa.m + Da.(1 - m)
```
## SRC\_OVER ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  Dca' = Sca.Da + Sca.(1 - Da) + Dca.(1 - Sa)
       = Sca + Dca.(1 - Sa)
  Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
       = Sa + Da.(1 - Sa)
       = Sa + Da - Sa.Da

  Msk:

  Dca' = (Sca + Dca.(1 - Sa)).m + Dca.(1 - m)
       = Sca.m + Dca.(1 - Sa.m)
  Da'  = (Sa + Da.(1 - Sa)).m + Da .(1 - m)
       = Sa.m + Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC):
  Dca' = Sc
  Da'  = 1

  Msk:

  Dca' = Sc.m + Dca.(1 - m)
  Da'  = 1 .m + Da .(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  Dc'  = Sca + Dc.(1 - Sa)

  Msk:

  Da'  = Sca.m + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  Dc'  = Sc

  Msk:

  Da'  = Sc.m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## SRC\_IN ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_IN):
  Dca' = Sca.Da
  Da'  = Sa.Da

  Msk:

  Dca' = Sca.Da.m + Dca.(1 - m)
  Da'  = Sa .Da.m + Da .(1 - m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_IN):
  Dca' = Sc.Da
  Da'  = Da

  Msk:

  Dca' = Sc.Da.m + Dca.(1 - m)
  Da'  = 1 .Da.m + Da .(1 - m)
       = Da

Formulas for XRGB(dst), PRGB(src) colorspaces (SRC):
  Dc'  = Sca

  Msk:

  Dc'  = (Sca.m + Dc.(1 - m))

Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  Dc'  = Sc

  Msk:

  Dc'  = (Sc.m + Dc.(1 - m))

Formulas for A(dst), A(src) colorspaces (MUL):
  Da'  = Sa.Da

  Msk:

  Da'  = Sa.Da.m + Da.(1 - m)
       = Da.(Sa.m + 1 - m)
```
## SRC\_OUT ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OUT):
  Dca' = Sca.(1 - Da)
  Da'  = Sa.(1 - Da)

  Msk:

  Dca' = Sca.(1 - Da).m + Dca.(1 - m)
  Da'  = Sa.(1 - Da).m + Da.(1 - m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_OUT):
  Dca' = Sc.(1 - Da)
  Da'  = (1 - Da)

  Msk:

  Dca' = Sc.(1 - Da).m + Dca.(1 - m)
  Da'  = (1 - Da).m + Da.(m - 1)

Formulas for XRGB(dst), PRGB(src) colorspaces (CLEAR):
  Dc'  = 0

  Msk:

  Dc'  = Dca.(1 - m)

Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  Dc'  = 0

  Msk:

  Dc'  = Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OUT):
  Da'  = Sa.(1 - Da)

  Msk:

  Da'  = Sa.(1 - Da).m + Da.(1 - m)
```
## SRC\_ATOP ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_ATOP):
  Dca' = Sca.Da + Dca.(1 - Sa)
  Da'  = Sa.Da + Da.(1 - Sa) 
       = Sa.Da + Da - Da.Sa
       = Da

  Msk:

  Dca' = (Sca.Da + Dca.(1 - Sa)).m + Dca.(1 - m)
       = Sca.Da.m + Dca.(1 - Sa).m + Dca.(1 - m)
       = Sca.Da.m + Dca.(1 - Sa.m)
  Da'  = Sa.Da.m + Da.(1 - Sa.m)
       = Da

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_IN):
  Dca' = Sc.Da
  Da'  = Da

  Msk:

  Dca' = (Sc.Da + Dca.(1 - Sa)).m - Dca.(1 - m)
       = Sc.Da.m + Dca.(1 - Sa).m - Dca.(1 - m)
       = Sc.Da.m + Dca.(1 - Sa.m)
  Da'  = (Da + Da.(1 - 1)).m - Da.(1 - m)
       = Da.m + Da.(1 - m)
       = Da

Formulas for XRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  Dc'  = Sca + Dc.(1 - Sa)

  Msk:

  Dc'  = Sca.m + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  Dc'  = Sc

  Msk:

  Dc'  = Sc.m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (NOP):
  Da'  = Da
```
## DST ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (NOP):
  Dca' = Dca
  Da'  = Da

Formulas for PRGB(dst), XRGB(src) colorspaces (NOP):
  Dca' = Dca
  Da'  = Da

Formulas for XRGB(dst), PRGB(src) colorspaces (NOP):
  Dc'  = Dc

Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  Dc'  = Dc

Formulas for A(dst), A(src) colorspaces (NOP):
  Da'  = Da
```
## DST\_OVER ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OVER):
  Dca' = Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
       = Dca + Sca.(1 - Da)
  Da'  = Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
       = Da + Sa.(1 - Da)
       = Da + Sa - Da.Sa

  Msk:

  Dca' = (Dca + Sca.(1 - Da)).m + Dca.(1 - m)   => Dca + Sca.(1 - Da).m
  Da'  = (Da  + Sa .(1 - Da)).m + Da .(1 - m)   => Da  + Sa .(1 - Da).m

Formulas for PRGB(dst), XRGB(src) colorspaces (DST_OVER):
  Dca' = Dca + Sc.(1 - Da)
  Da'  = 1

  Msk:

  Dca' = Dca + Sc.m.(1 - Da)
  Da'  = Da + m.(1 - Da)

Formulas for XRGB(dst), PRGB(src) colorspaces (NOP):
  Dc'  = Dc

  Msk:

  Dc ' = Dc

Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  Dc'  = Dc

  Msk:

  Dc ' = Dc

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## DST\_IN ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DST_IN):
  Dca' = Dca.Sa
  Da'  = Da.Sa

  Msk:

  Dca' = Dca.Sa.m + Dca.(1 - m) = Dca.(Sa.m + (1 - m))
  Da'  = Da .Sa.m + Da .(1 - m) = Da .(Sa.m + (1 - m))

Formulas for PRGB(dst), XRGB(src) colorspaces (NOP):
  Dca' = Dca
  Da'  = Da

Formulas for XRGB(dst), PRGB(src) colorspaces (DST_IN):
  Dc'  = Dc.Sa

  Msk:

  Dc'  = Dc.Sa.m + Dc.(m - 1)

Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  Dc'  = Dc

Formulas for A(dst), A(src) colorspaces (MUL):
  Da'  = Sa.Da

  Msk:

  Da'  = Sa.Da.m + Da.(1 - m)
       = Da.(Sa.m + 1 - m)
```
## DST\_OUT ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OUT):
  Dca' = Dca.(1 - Sa)
  Da'  = Da.(1 - Sa)

  Msk:

  Dca' = Dca.(1 - Sa).m + Dca.(1 - m)
       = Dca.(1 - Sa.m)
  Da'  = Da.(1 - Sa).m + Da.(1 - m)
       = Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (CLEAR):
  Dca' = 0
  Da'  = 0

  Msk:

  Dca' = Dca.(m - 1)
  Da'  = Da.(m - 1)

Formulas for XRGB(dst), PRGB(src) colorspaces (DST_OUT):
  Dc'  = Dc.(1 - Sa)

  Msk:

  Dc'  = Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  Dc'  = 0

  Msk:

  Dc'  = Dc.(m - 1)
  Da'  = Da.(m - 1)

Formulas for A(dst), A(src) colorspaces (DST_OUT):
  Da'  = Da.(1 - Sa)
```
## DST\_ATOP ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DST_ATOP):
  Dca' = Dca.Sa + Sca.(1 - Da)
  Da'  = Da.Sa + Sa.(1 - Da)
       = Sa.(Da + 1 - Da)
       = Sa

  Msk:

  Dca' = (Dca.Sa + Sca.(1 - Da)).m + Dca.(1 - m)
  Da'  = (Da.Sa + Sa.(1 - Da)).m + Da.(1 - m)

Formulas for PRGB(dst), XRGB(src) colorspaces (DST_OVER):
  Dca' = Dca + Sc.(1 - Da)
  Da'  = 1

  Msk:

  Dca' = Dca + Sc.m.(1 - Da)
  Da'  = Da + m.(1 - Da)

Formulas for XRGB(dst), PRGB(src) colorspaces (DST_IN):
  Dc'  = Dc.Sa

  Msk:

  Dca' = Dc.Sa.m + Dc.(1 - m)

Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  Dc'  = Dc

Formulas for A(dst), A(src) colorspaces (SRC):
  Da'  = Sa

  Msk:

  Da'  = Sa.m + Da.(1 - m)
```
## XOR ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (XOR):
  Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  Da'  = Sa.(1 - Da) + Da.(1 - Sa)
       = Sa + Da - 2.Sa.Da

  Msk:

  Dca' = Sca.m.(1 - Da) + Dca.(1 - Sa.m)
  Da'  = Sa.m.(1 - Da) + Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_OUT):
  Dca' = Sc.(1 - Da)
  Da'  = (1 - Da)

  Msk:

  Dca' = Sc.m.(1 - Da) + Dca.(1 - m)
  Da'  = 1.m.(1 - Da) + Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (DST_OUT):
  Dc'  = Dc.(1 - Sa)

  Msk:

  Dc'  = Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  Dc'  = 0

Formulas for A(dst), A(src) colorspaces (XOR):
  Da'  = Sa.(1 - Da) + Da.(1 - Sa)
```
## CLEAR ##
```
Formulas for ARGB(dst) colorspace (CLEAR):
  Dc'  = 0
  Da'  = 0

  Msk:

  Dca' = Dc
  Da'  = Da.(1 - m)

Formulas for PRGB(dst) colorspace (CLEAR):
  Dca' = 0
  Da'  = 0

  Msk:

  Dca' = Dca.(1 - m)
  Da'  = Da .(1 - m)

Formulas for XRGB(dst) colorspace (CLEAR):
  Dc'  = 0

  Msk:

  Dc'  = Dc.(1 - m)

Formulas for A(dst) colorspace (CLEAR):
  Da'  = 0

  Msk:

  Da'  = Da.(1 - m)
```
## ADD ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (ADD):
  Dca' = Sca.Da + Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
       = Dca + Sca
  Da'  = Sa.Da + Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
       = Da + Sa

  Msk:

  Dca' = Sca.m + Dca.m + Dca.(1 - m)
       = Dca + Sca.m
  Da'  = Sa.m + Da.m + Da.(1 - m)
       = Da + Sa.m

Formulas for PRGB(dst), XRGB(src) colorspaces (ADD):
  Dca' = Dca + Sc
  Da'  = 1

  Msk:

  Dca' = Dca + Sc.m
  Da'  = Da + m

Formulas for XRGB(dst), PRGB(src) colorspaces (ADD):
  Dc'  = Dc + Sca

  Msk:

  Dc'  = Dc + Sca.m

Formulas for XRGB(dst), XRGB(src) colorspaces (ADD):
  Dc'  = Dc + Sc

  Msk:

  Dc'  = Dc + Sc.m

Formulas for A(dst), A(src) colorspaces (ADD):
  Da'  = Da + Sa

  Msk:

  Da'  = Da + Sa.m
```
## SUBTRACT ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  Dca' = Dca - Sca
  Da'  = 1 - (1 - Sa).(1 - Da)
       = Sa + Da - Sa.Da

  Msk:

  Dca' = Dca.m - Sca.m + Dca.(1 - m)
       = Dca - Sca.m
  Da'  = Da + Sa.m - Sa.m.Da
       = Da + Sa.m - (1 - Da)

Formulas for PRGB(dst), XRGB(src) colorspaces (SUBTRACT):
  Dca' = Dca - Sc
  Da'  = 1

Formulas for XRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  Dc'  = Dc - Sca

Formulas for XRGB(dst), XRGB(src) colorspaces (SUBTRACT):
  Dc'  = Dc - Sc

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## MULTIPLY ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  Dca' = Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)

  Msk:

  Dca' = (Dca.(Sca + 1 - Sa) + Sca.(1 - Da)).m + Dca.(1 - m)
         (Dca.(Sca + 1 - Sa).m + Sca.(1 - Da).m + Dca.(1 - m)

  Da'  = (Da.(Sa + 1 - Sa) + Sa.(1 - Da)).m + Da.(1 - m)

Formulas for PRGB(dst), XRGB(src) colorspaces (MULTIPLY):
  Dca' = Sc.Dca + Sc.(1 - Da)
  Da'  = 1

  Msk:

  Dca' = (Sc.Dca + Sc.(1 - Da)).m + Dca.(1 - m)
  Da'  = m + Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  Dc'  = Sca.Dc + Dc.(1 - Sa)

  Msk:

  Dc'  = Dc + Dc.(Sca.m - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (MULTIPLY):
  Dc'  = Sc.Dc

  Msk:

  Dc'  = (Sc.Dc.m) + Dc.(1 - m)
       = Dc.(Sc.m + 1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## SCREEN ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (SCREEN):
  Dca' = Sca + Dca.(1 - Sca)
  Da'  = Sa + Da.(1 - Sa)

  Msk:

  Dca' = Sca.m + Dca.(1 - Sca.m)
  Da'  = Sa.m + Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (SCREEN):
  Dca' = Sc + Dca.(1 - Sc)
  Da'  = 1

  Msk:

  Dca' = Sc.m + Dca.(1 - Sc.m)
  Da'  = m + Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (SCREEN):
  Dc'  = Sca + Dc.(1 - Sca)

  Msk:

  Dca' = Sca.m + Dc.(1 - Sca.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (SCREEN):
  Dc'  = Sc + Dc.(1 - Sc)

  Msk:

  Dc'  = Sc.m + Dc.(1 - Sc.m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## OVERLAY ##
```
```
## DARKEN ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DARKEN):
  Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
       = Sa.Da + Sa - Sa.Da + Da - Sa.Da
       = Sa + Da - Sa.Da

  ALTERNATIVE: if (Sca.Da < Dca.Sa) Src-Over() else Dst-Over()

  Msk:

  Dca' = (min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
         min(Sca.Da.m, Dca.Sa.m) + Sca.m.(1 - Da) +  Dca.(1 - Sa.m)
  Da'  = (min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
         min(Sa.Da.m, Da.Sa.m) + Sa.m.(1 - Da) +  Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (DARKEN):
  Dca' = min(Sc.Da, Dca) + Sc.(1 - Da)
  Da'  = 1

  Msk:

  Dca' = min(Da.m, Dca.m) + Sc.m.(1 - Da) +  Dca.(1 - m)
  Da'  = min(Da.m, Da.m) + 1.m.(1 - Da) +  Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (DARKEN):
  Dc'  = min(Sca, Dc.Sa) + Dc.(1 - Sa)

  Msk:

  Dc'  = min(Sca.m, Dc.Sa.m) + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (DARKEN):
  Dc'  = min(Sc, Dc)

  Msk:

  Dc'  = (min(Sc, Dc)).m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## LIGHTEN ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  Da'  = max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
       = Sa.Da + Sa - Sa.Da + Da - Sa.Da
       = Sa + Da - Sa.Da

  ALTERNATIVE: if (Sca.Da > Dca.Sa) Src-Over() else Dst-Over()

  Msk:

  Dca' = (max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
         max(Sca.Da.m, Dca.Sa.m) + Sca.m.(1 - Da) +  Dca.(1 - Sa.m)
  Da'  = (max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
         max(Sa.Da.m, Da.Sa.m) + Sa.m.(1 - Da) +  Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (LIGHTEN):
  Dca' = max(Sc.Da, Dca) + Sc.(1 - Da)
  Da'  = 1

  Msk:

  Dca' = max(Da.m, Dca.m) + Sc.m.(1 - Da) +  Dca.(1 - m)
  Da'  = max(Da.m, Da.m) + 1.m.(1 - Da) +  Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  Dc'  = max(Sca, Dc.Sa) + Dc.(1 - Sa)

  Msk:

  Dc'  = max(Sca.m, Dc.Sa.m) + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (LIGHTEN):
  Dc'  = max(Sc, Dc)

  Msk:

  Dc'  = (max(Sc, Dc)).m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## COLOR\_DODGE ##
```
```
## COLOR\_BURN ##
```
```
## SOFT\_LIGHT ##
```
```
## HARD\_LIGHT ##
```
```
## DIFFERENCE ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  Dca' = abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
       = Dca + Sca - 2.min(Sca.Da, Dca.Sa)
  Da'  = abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)
       = Sa + Da - min(Sa.Da, Da.Sa)
       = Sa + Da - Sa.Da

  Msk:

  Dca' = (abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
       = (Sca + Dca - 2.min(Sca.Da, Dca.Sa)).m + Dca.(1 - m)
       = Dca + (Sca - 2.min(Sca.Da, Dca.Sa)).m
       = Dca + Sca.m - 2.min(Sca.Da.m, Dca.Sa.m)
  Da'  = (Sa + Da - min(Sa.Da, Da.Sa)).m + Da.(1 - m)
       = Da + (Sa - min(Sa.Da, Da.Sa)).m
       = Da + Sa.m - min(Sa.Da.m, Da.Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (DIFFERENCE):
  Dca' = abs(Dca.Sa - Sc.Da) + Sca.(1 - Da) + Dca
       = Dca + Sc - 2.min(Sc.Da, Dca.1)
  Da'  = abs(Da.1 - 1.Da) + 1.(1 - Da) + Da
       = 1 + Da - min(1.Da, Da.1)
       = 1

  Msk:

  Dca' = (abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
       = (Sca + Dca - 2.min(Sca.Da, Dca.Sa)).m + Dca.(1 - m)
       = Dca + (Sca - 2.min(Sca.Da, Dca.Sa)).m
       = Dca + Sca.m - 2.min(Sca.Da.m, Dca.Sa.m)
  Da'  = (abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
       = (Sa + Da - 2.min(Sa.Da, Da.Sa)).m + Da.(1 - m)
       = Da + (Sa - 2.min(Sa.Da, Da.Sa)).m
       = Da + Sa.m - 2.min(Sa.Da.m, Da.Sa.m)

Formulas for XRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  Dc'  = abs(Dc.Sa - Sca) + Dc.(1 - Sa)
       = Sca + Dc - 2.min(Sca, Dc.Sa)

  Msk:

  Dc'  = (abs(Dc.Sa - Sca) + Dc.(1 - Sa)).m + Dc.(1 - m)
       = abs(Dc.Sa.m - Sca.m) + Dc.(1 - Sa.m)
  Dc'  = (Sca + Dc - 2.min(Sca, Dc.Sa)).m + Dc.(1 - m)
       = Sca.m + Dc - 2.min(Sca.m, Dc.Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (DIFFERENCE):
  Dc'  = Dc + Sc - 2.min(Sc, Dc)
       = abs(Dc - Sc)

  Msk:

  Dc'  = abs(Dc - Sc).m + Dc.(1 - m)
       = abs(Dc.m - Sc.m) + Dc.(1 - m)
  Dc'  = (Sc + Dc - 2.min(Sc, Dc)).m + Dc.(1 - m)
       = Sc.m + Dc - 2.min(Sc.m, Dc.m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## EXCLUSION ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  Dca' = Sca.Da + Dca - 2.Sca.Dca
  Da'  = Sa + Da - Sa.Da

  Msk:

  Dca' = Sca.m.Da + Dca - 2.Sca.m.Dca
  Da'  = Sa.m + Da - Sa.m.Da

Formulas for PRGB(dst), XRGB(src) colorspaces (EXCLUSION):
  Dca' = Sc.Da + Dca - 2.Sc.Dca
  Da'  = 1

  Msk:

  Dca' = Sc.m.Da + Dca - 2.Sc.m.Dca
  Da'  = m + Da - m.Da

Formulas for XRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  Dc'  = Sca + Dc - 2.Sca.Dc

  Msk:

  Dc'  = Sca.m + Dc - 2.Sca.m.Dc

Formulas for XRGB(dst), XRGB(src) colorspaces (EXCLUSION):
  Dc'  = Sc + Dc - 2.Sc.Dc

  Msk:

  Dc'  = Sc.m.Da + Dc - 2.Sc.m.Dc

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## INVERT ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT):
  Dca' = (Da - Dca) * Sa + Dca.(1 - Sa)
  Da'  = (1) * Sa + Da.(1 - Sa)

  Msk:

  Dca' = ((Da - Dca) * Sa + Dca.(1 - Sa)).m + Dca.(1 - m)
       = (Da - Dca) * Sa.m + Dca.(1 - Sa.m)
  Da'  = (1) * Sa.m + Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (INVERT):
  Dca' = (Da - Dca)
  Da'  = 1

  Msk:

  Dca' = (Da - Dca).m + Dca.(1 - m)
         Da.m - Dca.m + Dca.(1 - m)
  Da'  = m - Da.(1 - m)

Formulas for XRGB(dst), PRGB(src) colorspaces (INVERT):
  Dc'  = (1 - Dc) * Sa + Dc.(1 - Sa)

  Msk:

  Dc'  = (1 - Dc).Sa.m + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (INVERT):
  Dc'  = 1 - Dc

  Msk:

  Dc'  = (1 - Dc).m + Dc.(1 - m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```
## INVERT\_RGB ##
```
Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  Dca' = (Da - Dca) * Sca + Dca.(1 - Sa)
  Da'  = (1) * Sa + Da.(1 - Sa)

  Msk:

  Dca' = (Da - Dca) * Sca.m + Dca.(1 - Sa.m)
  Da'  = (1) * Sa.m + Da.(1 - Sa.m)

Formulas for PRGB(dst), XRGB(src) colorspaces (INVERT_RGB):
  Dca' = (Da - Dca) * Sc
  Da'  = 1

  Msk:

  Dca' = (Da - Dca) * Sc.m + Dca.(1 - Sa.m)
  Da'  = (1) * Sa.m + Da.(1 - Sa.m)

Formulas for XRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  Dc'  = (1 - Dc) * Sca + Dc.(1 - Sa)

  Msk:

  Dc'  = (1 - Dc) * Sca.m + Dc.(1 - Sa.m)

Formulas for XRGB(dst), XRGB(src) colorspaces (INVERT_RGB):
  Dc'  = (1 - Dc) * Sc

  Msk:

  Dca' = (1 - Dc) * Sc.m + Dc.(1 - Sa.m)

Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  Da'  = Da + Sa.(1 - Da) (standard formula used many times)

  Msk:

  Da'  = Da + Sa.m.(1 - Da)
```