/*
 * This file registers the FreeType modules compiled into the library.
 *
 * YGOPro only uses modules that are needed for TrueType fonts.
 *
 */

FT_USE_MODULE( FT_Driver_ClassRec, tt_driver_class )
FT_USE_MODULE( FT_Module_Class, sfnt_module_class )
FT_USE_MODULE( FT_Renderer_Class, ft_smooth_renderer_class )

/* EOF */
