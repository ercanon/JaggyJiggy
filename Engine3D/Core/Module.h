#ifndef _MODULE_H_
#define _MODULE_H_
#include "Globals.h"

#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"

class Application;
struct PhysBody3D;

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JSONWriter;
typedef rapidjson::Value JSONReader;

class Module
{
private :
	bool enabled;

public:
	Application* App;

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~Module()
	{}
	
	virtual void OnLoad(const JSONReader& reader)
	{		
	}
	
	virtual void OnSave(JSONWriter& writer) const
	{

	}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnGui() {}

	virtual void OnCollision(ComponentCollider* body1, ComponentCollider* body2) {}

};

#endif