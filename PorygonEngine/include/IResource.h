#pragma once
#include "Prerequisites.h"

enum class
	ResourceType {
	Unknown,
	Model3D,
	Texture,
	Sound,
	Shader,
	Material
};

enum class
	ResourceState {
	UnLoaded,
	Loading,
	Loaded,
	Failed
};

class IResource {
public:
	IResource(const std::string& name) : m_name(name) {}
	virtual ~IResource() = default;

	virtual bool init() = 0;	

	virtual bool load(const std::string& filename) = 0;

	virtual void unload() = 0;

	virtual size_t getSizeInBytes() const = 0;

	const std::string& getName() const { return m_name; }
	const std::string& getFilePath() const { return m_filePath; }
	ResourceType getType() const { return m_type; }	
	ResourceState getState() const { return m_state; }
	uint64_t getID() const { return m_id; }

protected:
	std::string m_name;
	std::string m_filePath;
	ResourceType m_type;
	ResourceState m_state;
	uint64_t m_id;

private:
	static uint64_t GenerateID()
	{
				static uint64_t nextID = 1;
				return	nextID++;
	}
};	