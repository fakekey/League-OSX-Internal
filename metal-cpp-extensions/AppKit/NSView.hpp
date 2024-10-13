/*
 *
 * Copyright 2020-2021 Apple Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// AppKit/NSView.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "AppKitPrivate.hpp"
#include <Foundation/NSObject.hpp>
#include <CoreGraphics/CGGeometry.h>
#include <QuartzCore/CAMetalDrawable.hpp>

namespace NS
{
	class View : public NS::Referencing< View >
	{
		public:
			View*		init( CGRect frame );
			CGRect      frame() const;
			CGRect      bounds() const;
			void        addSubview( View *view );
			CA::MetalLayer*  layer() const;
	};
}


_NS_INLINE NS::View* NS::View::init( CGRect frame )
{
	return Object::sendMessage< View* >( _APPKIT_PRIVATE_CLS( NSView ), _APPKIT_PRIVATE_SEL( initWithFrame_ ), frame );
}

_NS_INLINE CGRect NS::View::frame() const
{
	return Object::sendMessage< CGRect >( this, _APPKIT_PRIVATE_SEL( frame ) );
}

_NS_INLINE CGRect NS::View::bounds() const
{
	return Object::sendMessage< CGRect >( this, _APPKIT_PRIVATE_SEL( bounds ) );
}

_NS_INLINE CA::MetalLayer* NS::View::layer() const
{
	return Object::sendMessage< CA::MetalLayer* >( this, _APPKIT_PRIVATE_SEL( layer ) );
}


_NS_INLINE void NS::View::addSubview( View *view )
{
    Object::sendMessage< void >( this, _APPKIT_PRIVATE_SEL( addSubview_ ), view );
}