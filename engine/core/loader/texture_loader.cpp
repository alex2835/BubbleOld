
#include "loader.h"
#include "lib/str.h"

namespace Bubble
{
    Ref<Texture2D> Loader::LoadTexture2DSingleColor(const std::string& name, const glm::vec4& color)
    {
        if (mLoadedTextures.count(name))
            return mLoadedTextures[name];

        Ref<Texture2D> texture = CreateRef<Texture2D>(color);
        mLoadedTextures[name] = texture;
        return texture;
    }

    Ref<Texture2D> Loader::LoadAndCacheTexture2D(std::string path, const Texture2DSpecification& spec)
    {
        if (!mProject.Valid())
            BUBBLE_CORE_ASSERT(false, "Try to load and cache texture with not valid project");

        auto rel_path = CreateRelPath(mProject.GetPath(), path);
        if (mLoadedTextures.count(rel_path))
            return mLoadedTextures[rel_path];
    
        Ref<Texture2D> texture = LoadTexture2D(path, spec);
        mLoadedTextures.emplace(rel_path, texture);
        return texture;
    }

    void Loader::LoadSystemTexture(std::string path, Texture2DSpecification spec)
    {
        auto name = MidPartLastOf(path, "/", ".");
		if (mSystemTextures.count(name))
            BUBBLE_CORE_ASSERTF(false, "System texture already exist {}", path);

		Ref<Texture2D> texture = LoadTexture2D(path, spec);
        mSystemTextures.emplace(name, texture);
    }

    Ref<Texture2D> Loader::GetSystemTexture(std::string name)
    {
		if (mSystemTextures.count(name))
			return mSystemTextures[name];
        BUBBLE_CORE_ASSERTF(false, "No such system texture {}", name);
    }


    Ref<Texture2D> Loader::LoadTexture2D(std::string path, const Texture2DSpecification& spec)
    {
        Ref<Texture2D> texture = CreateRef<Texture2D>();

        stbi_uc* data = nullptr;
        int width, height, channels;
        stbi_set_flip_vertically_on_load(spec.mFlip);
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (data == nullptr)
            throw std::runtime_error("Failed to load image!\nPath: " + path);

        texture->mSpecification = spec;
        texture->mSpecification.mWidth  = width;
        texture->mSpecification.mHeight = height;
        SetTextureSpecChanels(texture->mSpecification, channels);
        texture->mSpecification.mAnisotropicFiltering = true;

        texture->Invalidate();
        texture->SetData(data, width * height * channels);
        stbi_image_free(data);
        return texture;
    }

    std::tuple<Scope<uint8_t[]>, Texture2DSpecification>
        Loader::OpenRawImage(const std::string& path, Texture2DSpecification spec)
    {
        uint8_t* data = nullptr;
        int width, height, channels;

        stbi_set_flip_vertically_on_load(spec.mFlip);
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data == nullptr)
            throw std::runtime_error("Failed to load image!\nPath: " + path);

        spec.mWidth = width;
        spec.mHeight = height;
        SetTextureSpecChanels(spec, channels);

        return { Scope<uint8_t[]>(data), spec };
    }

    // Skysphere texture
    Ref<Texture2D> Loader::LoadSkysphere(const std::string& path)
    {
        if (mLoadedSkyspheres.count(path))
            return mLoadedSkyspheres[path];

        mLoadedSkyspheres[path] = LoadTexture2D(path);
        return mLoadedSkyspheres[path];
    }
}