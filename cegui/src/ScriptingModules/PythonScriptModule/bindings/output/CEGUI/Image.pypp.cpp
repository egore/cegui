// This file has been generated by Py++.

#include "boost/python.hpp"
#include "python_CEGUI.h"
#include "Image.pypp.hpp"

namespace bp = boost::python;

void register_Image_class(){

    { //::CEGUI::Image
        typedef bp::class_< CEGUI::Image > Image_exposer_t;
        Image_exposer_t Image_exposer = Image_exposer_t( "Image", "*!\n\
        \n\
           Class that represents a single Image of an Imageset.\n\
        *\n", bp::init< >("*************************************************************************\n\
           Construction and Destruction\n\
        *************************************************************************\n\
        *!\n\
        \n\
           Default constructor (only used by std.map)\n\
        *\n") );
        bp::scope Image_scope( Image_exposer );
        Image_exposer.def( bp::init< CEGUI::Imageset const *, CEGUI::String const &, CEGUI::Rect const &, CEGUI::Point const &, bp::optional< float, float > >(( bp::arg("owner"), bp::arg("name"), bp::arg("area"), bp::arg("render_offset"), bp::arg("horzScaling")=1.0e+0f, bp::arg("vertScaling")=1.0e+0f ), "*!\n\
           \n\
              Constructor for Image objects.  This is not normally used directly by client code, use the\
              Imageset interface instead.\n\
        \n\
           @param owner\n\
              Pointer to a Imageset object that owns this Image.  This must not be NULL.\n\
              \n\
           @param name\n\
              String object describing the name of the image being created.\n\
              \n\
           @param area\n\
              Rect object describing an area that will be associated with this image.\n\
              \n\
           @param render_offset\n\
              Point object that describes the offset to be applied when rendering this image.\n\
              \n\
           @param horzScaling\n\
              float value indicating the initial horizontal scaling to be applied to this image.\n\
        \n\
           @param vertScaling\n\
              float value indicating the initial vertical scaling to be applied to this image.\n\
        \n\
           @exception NullObjectException   Thrown if  owner was NULL.\n\
           *\n") );
        Image_exposer.def( bp::init< CEGUI::Image const & >(( bp::arg("image") ), "*!\n\
        \n\
           Copy constructor\n\
        *\n") );
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Vector2 const &,::CEGUI::Size const &,::CEGUI::Rect const *,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("position"), bp::arg("size"), bp::arg("clip_rect"), bp::arg("top_left_colour")=4294967295u, bp::arg("top_right_colour")=4294967295u, bp::arg("bottom_left_colour")=4294967295u, bp::arg("bottom_right_colour")=4294967295u, bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Rect const &,::CEGUI::Rect const *,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("dest_rect"), bp::arg("clip_rect"), bp::arg("top_left_colour")=4294967295u, bp::arg("top_right_colour")=4294967295u, bp::arg("bottom_left_colour")=4294967295u, bp::arg("bottom_right_colour")=4294967295u, bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Vector2 const &,::CEGUI::Size const &,::CEGUI::Rect const *,::CEGUI::ColourRect const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("position"), bp::arg("size"), bp::arg("clip_rect"), bp::arg("colours"), bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Vector2 const &,::CEGUI::Rect const *,::CEGUI::ColourRect const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("position"), bp::arg("clip_rect"), bp::arg("colours"), bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Vector2 const &,::CEGUI::Rect const *,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::colour const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("position"), bp::arg("clip_rect"), bp::arg("top_left_colour")=4294967295u, bp::arg("top_right_colour")=4294967295u, bp::arg("bottom_left_colour")=4294967295u, bp::arg("bottom_right_colour")=4294967295u, bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::draw
        
            typedef void ( ::CEGUI::Image::*draw_function_type )( ::CEGUI::GeometryBuffer &,::CEGUI::Rect const &,::CEGUI::Rect const *,::CEGUI::ColourRect const &,::CEGUI::QuadSplitMode ) const;
            
            Image_exposer.def( 
                "draw"
                , draw_function_type( &::CEGUI::Image::draw )
                , ( bp::arg("buffer"), bp::arg("dest_rect"), bp::arg("clip_rect"), bp::arg("colours"), bp::arg("quad_split_mode")=::CEGUI::TopLeftToBottomRight ) );
        
        }
        { //::CEGUI::Image::getHeight
        
            typedef float ( ::CEGUI::Image::*getHeight_function_type )(  ) const;
            
            Image_exposer.def( 
                "getHeight"
                , getHeight_function_type( &::CEGUI::Image::getHeight )
                , "*!\n\
               \n\
                  Return the pixel height of the image.\n\
            \n\
               @return\n\
                  Height of this Image in pixels\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getImageset
        
            typedef ::CEGUI::Imageset const * ( ::CEGUI::Image::*getImageset_function_type )(  ) const;
            
            Image_exposer.def( 
                "getImageset"
                , getImageset_function_type( &::CEGUI::Image::getImageset )
                , bp::return_value_policy< bp::reference_existing_object >()
                , "*!\n\
               \n\
                  Return the parent Imageset object that contains this Image\n\
            \n\
               @return\n\
                  The parent Imageset object.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getImagesetName
        
            typedef ::CEGUI::String const & ( ::CEGUI::Image::*getImagesetName_function_type )(  ) const;
            
            Image_exposer.def( 
                "getImagesetName"
                , getImagesetName_function_type( &::CEGUI::Image::getImagesetName )
                , bp::return_value_policy< bp::copy_const_reference >()
                , "*!\n\
               \n\
                  Return the name of the Imageset that contains this Image\n\
            \n\
               @return\n\
                  String object containing the name of the Imageset which this Image is a part of.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getName
        
            typedef ::CEGUI::String const & ( ::CEGUI::Image::*getName_function_type )(  ) const;
            
            Image_exposer.def( 
                "getName"
                , getName_function_type( &::CEGUI::Image::getName )
                , bp::return_value_policy< bp::copy_const_reference >()
                , "*!\n\
               \n\
                  Return the name of this Image object.\n\
            \n\
               @return\n\
                  String object containing the name of this Image\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getOffsetX
        
            typedef float ( ::CEGUI::Image::*getOffsetX_function_type )(  ) const;
            
            Image_exposer.def( 
                "getOffsetX"
                , getOffsetX_function_type( &::CEGUI::Image::getOffsetX )
                , "*!\n\
               \n\
                  Return the X rendering offset\n\
            \n\
               @return\n\
                  X rendering offset.  This is the number of pixels that the image is offset by when rendering\
                  at any given location.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getOffsetY
        
            typedef float ( ::CEGUI::Image::*getOffsetY_function_type )(  ) const;
            
            Image_exposer.def( 
                "getOffsetY"
                , getOffsetY_function_type( &::CEGUI::Image::getOffsetY )
                , "*!\n\
               \n\
                  Return the Y rendering offset\n\
            \n\
               @return\n\
                  Y rendering offset.  This is the number of pixels that the image is offset by when rendering\
                  at any given location.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getOffsets
        
            typedef ::CEGUI::Point ( ::CEGUI::Image::*getOffsets_function_type )(  ) const;
            
            Image_exposer.def( 
                "getOffsets"
                , getOffsets_function_type( &::CEGUI::Image::getOffsets )
                , "*!\n\
               \n\
                  Return a Point object that contains the offset applied when rendering this Image\n\
            \n\
               @return\n\
                  Point object containing the offsets applied when rendering this Image\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getSize
        
            typedef ::CEGUI::Size ( ::CEGUI::Image::*getSize_function_type )(  ) const;
            
            Image_exposer.def( 
                "getSize"
                , getSize_function_type( &::CEGUI::Image::getSize )
                , "*!\n\
               \n\
                  Return a Size object containing the dimensions of the Image.\n\
            \n\
               @return\n\
                  Size object holding the width and height of the Image.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::getSourceTextureArea
        
            typedef ::CEGUI::Rect const & ( ::CEGUI::Image::*getSourceTextureArea_function_type )(  ) const;
            
            Image_exposer.def( 
                "getSourceTextureArea"
                , getSourceTextureArea_function_type( &::CEGUI::Image::getSourceTextureArea )
                , bp::return_value_policy< bp::copy_const_reference >()
                , "*!\n\
                \n\
                    Return Rect describing the source texture area used by this Image.\n\
            \n\
                @return\n\
                    Rect object that describes, in pixels, the area upon the source texture\n\
                    which is used when rendering this Image.\n\
                *\n" );
        
        }
        { //::CEGUI::Image::getWidth
        
            typedef float ( ::CEGUI::Image::*getWidth_function_type )(  ) const;
            
            Image_exposer.def( 
                "getWidth"
                , getWidth_function_type( &::CEGUI::Image::getWidth )
                , "*!\n\
               \n\
                  Return the pixel width of the image.\n\
            \n\
               @return\n\
                  Width of this Image in pixels.\n\
               *\n" );
        
        }
        { //::CEGUI::Image::writeXMLToStream
        
            typedef void ( ::CEGUI::Image::*writeXMLToStream_function_type )( ::CEGUI::XMLSerializer & ) const;
            
            Image_exposer.def( 
                "writeXMLToStream"
                , writeXMLToStream_function_type( &::CEGUI::Image::writeXMLToStream )
                , ( bp::arg("xml_stream") )
                , "*!\n\
                \n\
                    Writes an xml representation of this Image object to  out_stream.\n\
            \n\
                @param xml_stream\n\
                    Stream where xml data should be output.\n\
            \n\
            \n\
                @return\n\
                    Nothing.\n\
                *\n" );
        
        }
    }

}