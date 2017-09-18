template<class T>
T* ee::Renderer::addTexturePack(std::string name, T&& pack)
{
    if (impl::checkTextPackMap(name))
    {
        T* ptr = new T(std::move(pack));
        impl::insertTextPackIntoMap(name, ptr);
        return ptr;
    }

    return nullptr;
}
