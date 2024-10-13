//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// QuartzCore/CAMetalDrawable.hpp
//
// Copyright 2020-2021 Apple Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "../Metal/MTLDrawable.hpp"
#include "../Metal/MTLTexture.hpp"

#include "CADefines.hpp"
#include "CAPrivate.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace CA
{

class Layer : public NS::Referencing<Layer>
{
    public:
        Boolean isOpaque() const;
        void setOpaque(Boolean isOpaque);
};

_CA_INLINE Boolean CA::Layer::isOpaque() const
{
    return Object::sendMessage<Boolean>(this, _MTL_PRIVATE_SEL( isOpaque ));
}

_CA_INLINE void CA::Layer::setOpaque(Boolean isOpaque)
{
    Object::sendMessage<void>(this, _MTL_PRIVATE_SEL( setOpaque_ ), isOpaque);
}

class MetalLayer : public NS::Referencing<MetalLayer, CA::Layer>
{
public:
    MTL::Device* device() const;
};

class MetalDrawable : public NS::Referencing<MetalDrawable, MTL::Drawable>
{
public:
    CA::MetalLayer*   layer() const;
    MTL::Texture*     texture() const;
};
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

_CA_INLINE CA::MetalLayer* CA::MetalDrawable::layer() const
{
    return Object::sendMessage<MetalLayer*>(this, _MTL_PRIVATE_SEL(layer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

_CA_INLINE MTL::Device* CA::MetalLayer::device() const
{
    return Object::sendMessage<MTL::Device*>(this, _MTL_PRIVATE_SEL(device));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

_CA_INLINE MTL::Texture* CA::MetalDrawable::texture() const
{
    return Object::sendMessage<MTL::Texture*>(this, _MTL_PRIVATE_SEL(texture));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
