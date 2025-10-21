/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBinaryEnvSerializeH
#define UBinaryEnvSerializeH

#include "UItemData.h"
#include "../Serialize/USerStorageBinary.h"
#include "UELockVar.h"
#include "UEPtr.h"
#include "ModernSmartPointers.h"
#include <filesystem>
#include <span>
#include <memory>
#include <mutex>

namespace RDK {

// UBMColorModel
//USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data);
//USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data);

// UItemData
//RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UItemData &data);
//RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UItemData &data);


// UELockVar
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const UELockVar<T> &data)
{
 storage<<data.operator T();

 return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, UELockVar<T> &data)
{
 T temp;
 storage>>temp;
 data=temp;

 return storage;
}

// Modern C++20 serialization with std::span and move semantics
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, const std::span<T>& data)
{
    storage << static_cast<uint64_t>(data.size());
    for (const auto& item : data) {
        storage << item;
    }
    return storage;
}

template<typename T>
USerStorageBinary& operator >> (USerStorageBinary& storage, std::span<T>& data)
{
    uint64_t size;
    storage >> size;
    
    if (size > data.size()) {
        throw std::runtime_error("Span size mismatch during deserialization");
    }
    
    for (uint64_t i = 0; i < size; ++i) {
        storage >> data[i];
    }
    return storage;
}

// Modern file operations with std::filesystem
template<typename T>
bool SerializeToFile(const std::filesystem::path& filepath, const T& data)
{
    try {
        USerStorageBinary storage;
        storage << data;
        return storage.SaveToFile(filepath.string());
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
bool DeserializeFromFile(const std::filesystem::path& filepath, T& data)
{
    try {
        USerStorageBinary storage;
        if (!storage.LoadFromFile(filepath.string())) {
            return false;
        }
        storage >> data;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Move semantics for large data structures
template<typename T>
USerStorageBinary& operator << (USerStorageBinary& storage, T&& data)
{
    storage << std::forward<T>(data);
    return storage;
}


}
#endif
