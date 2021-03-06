#include <Box2D/Dynamics/Contacts/b2GridAndPolygonContact.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Common/b2BlockAllocator.h>
#include <Box2D/Collision/b2TimeOfImpact.h>
#include <Box2D/Collision/Shapes/b2GridShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>

#include <new>
using namespace std;

b2Contact* b2GridAndPolygonContact::Create(b2Fixture* fixtureA, int32 indexA, b2Fixture* fixtureB, int32, b2BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(b2GridAndPolygonContact));
	return new (mem) b2GridAndPolygonContact(fixtureA, indexA, fixtureB);
}

void b2GridAndPolygonContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
	((b2GridAndPolygonContact*)contact)->~b2GridAndPolygonContact();
	allocator->Free(contact, sizeof(b2GridAndPolygonContact));
}

b2GridAndPolygonContact::b2GridAndPolygonContact(b2Fixture* fixtureA, int32 indexA, b2Fixture* fixtureB)
	: b2Contact(fixtureA, indexA, fixtureB, 0)
{
	b2Assert(m_fixtureA->GetType() == b2Shape::e_grid);
	b2Assert(m_fixtureB->GetType() == b2Shape::e_polygon);
}

void b2GridAndPolygonContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2GridShape* gridShape = (b2GridShape*)m_fixtureA->GetShape();
    if (!gridShape->m_enabled)
    {
    	return;
    }
    b2PolygonShape* polyB = (b2PolygonShape*)m_fixtureB->GetShape();

    manifold->pointCount = 0;

    const b2GridShape::Cell& cell = gridShape->m_cells[m_indexA];
    if (cell.m_Index == 0xffffffff)
    {
        return;
    }

    b2PolygonShape polyA;
    gridShape->GetPolygonShapeForCell(m_indexA, polyA);
    b2CollidePolygons(manifold, &polyA, xfA, polyB, xfB);
}
