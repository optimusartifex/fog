# Compositing #

Image compositing is the process of mixing two pixels to produce the final pixel. The first pixel is usually pixel stored in destination buffer and the second pixel is usually a source, which can be solid or generated. Fog-Framework implements several compositing operators required by SVG and PDF standards, and used by other open source or commercial libraries (or applications). Compositing can be applied to fill, stroke, or blit operation.

<img src='http://fog.googlecode.com/svn/wiki/images/compositing.png' alt='Image Compositing' />

The image above shows result of compositing two images using various compositing operators.

# Source #

Source in Fog-Framework describes pixels which will be used in fill or stroke operations. Fog-Framework currently supports solid, gradient, texture, and filtered fill or stroke.

<img src='http://fog.googlecode.com/svn/wiki/images/gradients.png' alt='Gradients' />

The image above shows combinations or various gradients and spreads.

# Groups #

Groups are temporary images which are used to paint a bunch of paint operations before the final composition. After rendering to a group is complete you can composite the group to the parent. It is also possible to apply filters to a group; and groups can be nested.

<img src='http://fog.googlecode.com/svn/wiki/images/groups.png' alt='Groups' />

The image above shows difference between grouped painting (left) and painting without group (right) using half opacity.
