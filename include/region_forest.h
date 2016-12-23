#ifndef _H_REGION_FOREST
#define _H_REGION_FOREST

#include <list>
#include <vector>


struct Base
{
	Base *parent;
	Base() : parent(NULL) {}

	bool is_finished;
	
	virtual void set_parent(Base *parent)
	{
		this->parent = parent;
	}

	bool has_finished()
	{
		return is_finished;
	}

	Base* get_parent()
	{
		return parent;
	}

	void set_finish()
	{
		is_finished = true;
	}
};

struct Faces : Base
{

};

struct Cycles: Base
{
	std::list<Base*> children;

	int face_count;

	inline int num_children()
	{
		return children.size();
	}

	void add_children(Base* ptr)
	{
		Faces* p = dynamic_cast<Faces*>(ptr);
		if(p != NULL)
		{
			face_count++;
		}
		
		children.push_back(ptr);
	}


	inline int num_faces()
	{
		return face_count;
	}

	inline int num_non_faces()
	{
		return (children.size() - face_count);
	}

	void exchange_face_with_cycle(std::list<Base*>::iterator it, Cycles *cycle)
	{
		it = children.erase(it);
		children.insert(it, cycle);

		face_count--;
		if(face_count == 1)
		{
			set_finish();
		}
	}

	inline bool is_finished()
	{
		return (face_count == 1);
	}
};

struct region_tree
{
	int face_count;
	int num_cycles;

	std::vector<Base> list_faces;
	std::vector<Base> list_cycles;

	region_tree(int face_count, int num_cycles) : face_count(face_count), num_cycles(num_cycles)
	{
		list_faces.resize(face_count);
		list_cycles.resize(num_cycles);
	}

	~region_tree()
	{
		list_faces.clear();
		list_cycles.clear();
	}
};

#endif